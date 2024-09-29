#include "Renderer.hpp"

#include "Color.hpp"
#include "Vector.hpp"
#include "Camera.hpp"

namespace
{
	constexpr size_t kMaxLinesToRenderPerChunk = 10;
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
	m_scene = std::move(scene);
}

void Renderer::clear()
{
	std::fill(std::begin(m_pixels), std::end(m_pixels), Palette::kBlack.toRGBA());
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

	m_runRenderThreads = true;
	for (auto& t : m_renderThreads)
	{
		m_runningRenderThreads++;

		t = std::thread(
			[this]()
			{
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
						if (! m_runRenderThreads)
							break;

						const double cameraY = (double(y) / m_height) - .5;

						for (size_t x = 0; x < m_width; x++)
						{
							const double cameraX = (double(x) / m_width) - .5;

							*(currentPixel++) = m_scene.camera.trace(m_scene, cameraX, cameraY).toRGBA();
						}
					}
				}

				--m_runningRenderThreads;
			});
	}
}
