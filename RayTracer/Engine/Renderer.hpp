#pragma once

#include "Scene.hpp"

#include <atomic>
#include <chrono>
#include <stddef.h>
#include <stdint.h>
#include <thread>
#include <vector>
#include <condition_variable>
#include <mutex>

class Renderer
{
private:
	enum class RenderState { Stop, Run, Exit };

public:
											Renderer(size_t width, size_t height, size_t numRenderThreads);
											~Renderer();

	void									setScene(Scene scene);
	void									setCoarsePreview(bool preview);

	const uint32_t* 						pixels() const { return m_pixels.data(); }

	void									clear();

	void									waitForRenderCompletion();
	void									stopRender();
	void									startRender();

	bool									isRendering() const;
	uint8_t									renderPercentage() const;
	std::chrono::milliseconds				renderTime() const;

private:
	void									renderLines(size_t startLine, size_t endLine);

private:
	size_t									m_width = 0;
	size_t									m_height = 0;

	std::vector<uint32_t>					m_pixels;

	bool									m_coarsePreview = false;

	Scene									m_scene;

	std::mutex								m_lock;

	std::condition_variable					m_renderStateCondition;
	std::atomic<RenderState>				m_renderState = RenderState::Stop;

	std::vector<std::thread>				m_renderThreads;

	std::chrono::steady_clock::time_point	m_renderStartTime = {};
	std::chrono::steady_clock::time_point	m_renderEndTime = {};

	std::atomic<size_t>						m_busyThreads = 0;
	std::atomic<size_t>						m_lastRenderLineStart = 0;
	std::atomic<size_t>						m_finishedLines = 0;
};
