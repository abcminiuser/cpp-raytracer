#include "Engine/Renderer.hpp"

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"
#include "Engine/Camera.hpp"

#include <algorithm>

#if defined(NDEBUG)
[[maybe_unused]] constexpr bool kIsDebug = false;
#else
[[maybe_unused]] constexpr bool kIsDebug = true;
#endif

namespace
{
	constexpr size_t	kMaxLinesToRenderPerChunk	= 1;
	constexpr bool		kEnableSuperSampling		= false;
}

Renderer::Renderer(size_t width, size_t height, size_t numRenderThreads)
	: m_width(width)
	, m_height(height)
	, m_pixels(width * height)
	, m_renderThreads(numRenderThreads)
{
	clear();
}

Renderer::~Renderer()
{
	stopRender();

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
	if (! m_runRenderThreads)
		return;

	m_runRenderThreads = false;
	for (auto& t : m_renderThreads)
	{
		if (t.joinable())
			t.join();
	}
}

void Renderer::startRender()
{
	if (m_runRenderThreads)
		return;

	m_lastRenderLineStart = 0;

	m_renderStartTime = std::chrono::steady_clock::now();
	m_renderEndTime = {};

	m_runRenderThreads = true;
	for (auto& t : m_renderThreads)
	{
		m_runningRenderThreads++;

		t = std::thread(
			[this]()
			{
				const double xSampleOffset = 1.0 / m_width;
				const double ySampleOffset = 1.0 / m_height;

				while (m_runRenderThreads)
				{
					const size_t startLine = m_lastRenderLineStart.fetch_add(kMaxLinesToRenderPerChunk);
					if (startLine >= m_height)
						break;

					const size_t endLine = startLine + std::min(kMaxLinesToRenderPerChunk, m_height - startLine);
					if (endLine > m_height)
						break;

					uint32_t* currentPixel = &m_pixels[startLine * m_width];

					for (size_t y = startLine; y < endLine; y++)
					{
						for (size_t x = 0; x < m_width; x++)
						{
							double r = 0, g = 0, b = 0;

							// Super-sample across a 3x3 grid, then average the results to anti-alias.
							if constexpr(kEnableSuperSampling)
							{
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

				if (--m_runningRenderThreads == 0)
					m_renderEndTime = std::chrono::steady_clock::now();
			});
	}
}

std::chrono::milliseconds Renderer::renderTime() const
{
	const auto renderTime = (isRendering() ? std::chrono::steady_clock::now() : m_renderEndTime) - m_renderStartTime;
	return std::chrono::duration_cast<std::chrono::milliseconds>(renderTime);
}
