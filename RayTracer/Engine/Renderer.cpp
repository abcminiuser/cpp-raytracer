#include "Engine/Renderer.hpp"

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"
#include "Engine/Camera.hpp"

#include <algorithm>

namespace
{
	constexpr size_t	kMaxLinesToRenderPerChunk	= 10;
	constexpr bool		kEnableSuperSampling		= false;
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
					if (m_renderState == RenderState::Run && (startLine < m_height) && (endLine < m_height))
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
	const bool wasRendering = isRendering();
	stopRender();

	m_scene = std::move(scene);

	if (wasRendering)
		startRender();
}

void Renderer::clear()
{
	const uint32_t fillColor = Palette::kBlack.toRGBA();
	std::fill(m_pixels.begin(), m_pixels.end(), fillColor);
}

void Renderer::stopRender()
{
	std::unique_lock lock(m_lock);
	m_renderState = RenderState::Stop;

	lock.unlock();
	m_renderStateCondition.notify_all();
	lock.lock();

	m_renderStateCondition.wait(lock, [&] { return m_busyThreads == 0; });
}

void Renderer::startRender()
{
	stopRender();

	{
		std::lock_guard lock(m_lock);

		m_lastRenderLineStart = 0;

		m_renderStartTime = std::chrono::steady_clock::now();
		m_renderEndTime = {};

		m_renderState = RenderState::Run;
	}

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
		if (m_renderState.load() != RenderState::Run)
			break;

		for (size_t x = 0; x < m_width; x++)
		{
			// Super-sample across a 3x3 grid, then average the results to anti-alias.
			if constexpr(kEnableSuperSampling)
			{
				double r = 0, g = 0, b = 0;

				for (int ssY = -1; ssY <= 1; ssY++)
				{
					for (int ssX = -1; ssX <= 1; ssX++)
					{
						const double cameraY = ((y + ssY) * ySampleOffset) - .5;
						const double cameraX = ((x + ssX) * xSampleOffset) - .5;

						const Color sampleColor = m_scene.camera.trace(m_scene, cameraX, cameraY);

						r += sampleColor.red();
						g += sampleColor.green();
						b += sampleColor.blue();
					}
				}

				r /= 9;
				g /= 9;
				b /= 9;

				*(currentPixel++) = Color(uint8_t(r), uint8_t(g), uint8_t(b)).toRGBA();
			}
			else
			{
				const double cameraY = (y * ySampleOffset) - .5;
				const double cameraX = (x * xSampleOffset) - .5;

				*(currentPixel++) = m_scene.camera.trace(m_scene, cameraX, cameraY).toRGBA();
			}
		}
	}
}
