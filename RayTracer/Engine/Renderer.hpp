#pragma once

#include <atomic>
#include <stddef.h>
#include <stdint.h>
#include <thread>
#include <vector>

#include "Scene.hpp"

class Renderer
{
public:
									Renderer(size_t width, size_t height, size_t numRenderThreads);
									~Renderer();

	void							setScene(Scene scene);

	void							setRenderingEnabled(bool enabled);

	const uint32_t* 				pixels() const { return m_pixels.data(); }

private:
	void							stopRender();
	void							startRender();

private:
	size_t							m_width = 0;
	size_t							m_height = 0;

	bool							m_enabled = false;

	Scene							m_scene;

	std::vector<std::thread>		m_renderThreads;
	std::atomic_bool				m_runRenderThreads = true;

	std::vector<uint32_t>			m_pixels;
};
