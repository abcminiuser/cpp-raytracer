#include "Renderer.hpp"

#include "Color.hpp"
#include "Vector.hpp"
#include "Camera.hpp"

Renderer::Renderer(size_t width, size_t height, size_t numRenderThreads)
	: m_width(width)
	, m_height(height)
	, m_pixels(width * height)
	, m_renderThreads(numRenderThreads)
{
	std::fill(std::begin(m_pixels), std::end(m_pixels), Palette::kBlack.toRGBA());
}

Renderer::~Renderer()
{
	stopRender();
}

void Renderer::setScene(Scene scene)
{
	m_scene = std::move(scene);
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

	std::fill(std::begin(m_pixels), std::end(m_pixels), Palette::kBlack.toRGBA());
}

void Renderer::startRender()
{
	if (m_runRenderThreads)
		return;

	const auto linesPerThread = (m_height + (m_renderThreads.size() - 1)) / m_renderThreads.size();
	size_t startLine = 0;

	m_runRenderThreads = true;
	for (auto& t : m_renderThreads)
	{
		const size_t endLine = std::min(startLine + linesPerThread, m_height);

		t = std::thread(
			[this, startLine, endLine]()
			{
				uint32_t* currentPixel = &m_pixels[startLine * m_width];

				for (size_t y = startLine; y < endLine; y++)
				{
					if (! m_runRenderThreads)
						break;

					for (size_t x = 0; x < m_width; x++)
					{
						const auto cameraX = (float(x) / m_width) - .5f;
						const auto cameraY = (float(y) / m_height) - .5f;

						*(currentPixel++) = m_scene.camera.trace(m_scene, cameraX, cameraY).toRGBA();
					}
				}
			});

		startLine += linesPerThread;
	}
}
