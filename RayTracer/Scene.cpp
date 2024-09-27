#include "Scene.hpp"

Scene::Scene(size_t width, size_t height, size_t numRenderThreads)
	: m_width(width)
	, m_height(height)
	, m_pixels(width * height)
	, m_renderThreads(numRenderThreads)
{
	startRender();
}

Scene::~Scene()
{
	stopRender();
}

void Scene::stopRender()
{
	m_runRenderThreads = false;
	for (auto& t : m_renderThreads)
	{
		if (t.joinable())
			t.join();
	}
}

void Scene::startRender()
{
	stopRender();

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
					for (size_t x = 0; x < m_width; x++)
					{
						*(currentPixel++) =
							static_cast<uint32_t>(0xFF) << 24 |
							static_cast<uint32_t>(rand()) << 16 |
							static_cast<uint32_t>(rand()) << 8 |
							static_cast<uint32_t>(rand()) << 0;
					}
				}
			});

		startLine += linesPerThread;
	}
}
