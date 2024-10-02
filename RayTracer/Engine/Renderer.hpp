#pragma once

#include "Scene.hpp"

#include <atomic>
#include <chrono>
#include <stddef.h>
#include <stdint.h>
#include <thread>
#include <vector>

class Renderer
{
public:
											Renderer(size_t width, size_t height, size_t numRenderThreads);
											~Renderer();

	void									setScene(Scene scene);

	const uint32_t* 						pixels() const { return m_pixels.data(); }

	void									clear();
	void									stopRender();
	void									startRender();
	bool									isRendering() const { return m_runningRenderThreads.load() != 0; }
	std::chrono::milliseconds				renderTime() const;

private:
	size_t									m_width = 0;
	size_t									m_height = 0;

	std::vector<uint32_t>					m_pixels;

	Scene									m_scene;

	std::vector<std::thread>				m_renderThreads;
	std::atomic_bool						m_runRenderThreads = false;
	std::chrono::steady_clock::time_point	m_renderStartTime = {};
	std::chrono::steady_clock::time_point	m_renderEndTime = {};

	std::atomic<uint32_t>					m_runningRenderThreads = 0;
	std::atomic<uint32_t>					m_lastRenderLineStart = 0;
};
