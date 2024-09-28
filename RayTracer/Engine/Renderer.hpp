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

	void						setScene(Scene scene);

	const uint32_t* 			pixels() const { return m_pixels.data(); }

	void						clear();
	void						stopRender();
	void						startRender();
	bool						isRendering() const { return m_lastRenderLineStart.load() < m_height; }

private:
	size_t						m_width = 0;
	size_t						m_height = 0;

	Scene						m_scene;

	std::vector<std::thread>	m_renderThreads;
	std::atomic_bool			m_runRenderThreads = false;

	std::atomic<uint32_t>		m_lastRenderLineStart = 0;

	std::vector<uint32_t>		m_pixels;
};
