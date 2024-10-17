#include "Engine/Renderer.hpp"

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"
#include "Engine/Camera.hpp"

#include <algorithm>
#include <random>

namespace
{
	constexpr size_t	kMaxLinesToRenderPerChunk	= 1;
	constexpr size_t	kCoarsePreviewLineSpacing	= 8;
	constexpr double	kGamma						= 1.0;
}

Renderer::Renderer(size_t width, size_t height, size_t numRenderThreads)
	: m_width(width)
	, m_height(height)
	, m_pixels(width * height)
	, m_renderThreads(numRenderThreads)
{
	clear();

	for (auto& t : m_renderThreads)
	{
		t = std::thread(
			[this]()
			{
				for (;;)
				{
					std::unique_lock lock(m_lock);

					m_renderStateCondition.wait(lock, [&] { return m_renderState != RenderState::Stop; });

					if (m_renderState == RenderState::Exit)
						return;

					m_busyThreads++;

					const size_t startLine = m_lastRenderLineStart.fetch_add(kMaxLinesToRenderPerChunk);
					const size_t endLine = startLine + std::min(kMaxLinesToRenderPerChunk, m_height - startLine);
					if (m_renderState == RenderState::Run && (startLine < m_height) && (endLine <= m_height))
					{
						lock.unlock();
						renderLines(startLine, endLine);
						lock.lock();
					}

					if (--m_busyThreads == 0)
					{
						if (m_renderState == RenderState::Run && m_lastRenderLineStart >= m_height)
						{
							m_renderState = RenderState::Stop;
							m_renderEndTime = std::chrono::steady_clock::now();
						}

						lock.unlock();
						m_renderStateCondition.notify_all();
					}
				}
			}
		);
	}
}

Renderer::~Renderer()
{
	{
		std::lock_guard lock(m_lock);
		m_renderState = RenderState::Exit;
	}
	m_renderStateCondition.notify_all();

	for (auto& t : m_renderThreads)
	{
		if (t.joinable())
			t.join();
	}
}

void Renderer::setScene(Scene scene)
{
	stopRender();

	m_scene = std::move(scene);
}

void Renderer::setCoarsePreview(bool preview)
{
	stopRender();

	if (! m_coarsePreview && preview)
		clear();

	m_coarsePreview = preview;
}

void Renderer::clear()
{
	const uint32_t fillColor = Palette::kBlack.toPackedRGBA();
	std::fill(m_pixels.begin(), m_pixels.end(), fillColor);
}

void Renderer::waitForRenderCompletion()
{
	std::unique_lock lock(m_lock);
	m_renderStateCondition.wait(lock, [&] { return m_busyThreads == 0; });
}

void Renderer::stopRender()
{
	std::unique_lock lock(m_lock);

	if (m_renderState == RenderState::Stop)
		return;

	m_renderState = RenderState::Stop;

	lock.unlock();
	m_renderStateCondition.notify_all();
	lock.lock();

	m_renderStateCondition.wait(lock, [&] { return m_busyThreads == 0; });
}

void Renderer::startRender()
{
	std::unique_lock lock(m_lock);

	if (m_renderState == RenderState::Run)
		return;

	lock.unlock();
	stopRender();
	lock.lock();

	m_lastRenderLineStart = 0;

	m_renderStartTime = std::chrono::steady_clock::now();
	m_renderEndTime = {};

	m_renderState = RenderState::Run;

	lock.unlock();
	m_renderStateCondition.notify_all();
}

std::chrono::milliseconds Renderer::renderTime() const
{
	const auto renderTime = (isRendering() ? std::chrono::steady_clock::now() : m_renderEndTime) - m_renderStartTime;
	return std::chrono::duration_cast<std::chrono::milliseconds>(renderTime);
}

void Renderer::renderLines(size_t startLine, size_t endLine)
{
	uint32_t* currentPixel = &m_pixels[startLine * m_width];

	const double xSampleOffset = 1.0 / m_width;
	const double ySampleOffset = 1.0 / m_height;

	for (size_t y = startLine; y < endLine; y++)
	{
		if (m_coarsePreview && (y % kCoarsePreviewLineSpacing != 0))
		{
			// If we're doing a coarse preview render, we only render every few lines to save time.
			currentPixel += m_width;
			continue;
		}

		if (m_renderState.load() != RenderState::Run)
			break;

		for (size_t x = 0; x < m_width; x++)
		{
			Color sampleColor;

			for (size_t i = 0; i < m_scene.sampledPerPixel; i++)
			{
				static std::mt19937 generator;
				static std::uniform_real_distribution<double> distribution(-1.0, 1.0);

				const double cameraY = ((y + distribution(generator)) * ySampleOffset) - .5;
				const double cameraX = ((x + distribution(generator)) * xSampleOffset) - .5;

				sampleColor = sampleColor.add(m_scene.camera.trace(m_scene, cameraX, cameraY));
			}

			sampleColor = sampleColor.scale(1.0 / m_scene.sampledPerPixel);
			sampleColor = Color(pow(sampleColor.red(), 1.0 / kGamma), pow(sampleColor.green(), 1.0 / kGamma), pow(sampleColor.blue(), 1.0 / kGamma));

			*(currentPixel++) = sampleColor.toPackedRGBA();
		}
	}
}
