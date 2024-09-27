#pragma once

#include <stdint.h>
#include <stddef.h>
#include <thread>
#include <vector>
#include <atomic>

class Scene
{
public:
	Scene(size_t width, size_t height, size_t numRenderThreads);
	~Scene();

	const uint32_t* pixels() const { return m_pixels.data(); }

private:
	void							stopRender();
	void							startRender();

private:
	size_t							m_width = 0;
	size_t							m_height = 0;

	std::vector<std::thread>		m_renderThreads;
	std::atomic_bool				m_runRenderThreads = true;

	std::vector<uint32_t>			m_pixels;
};
