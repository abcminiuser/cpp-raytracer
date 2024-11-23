#include "Viewer.hpp"
#include "SceneLoader.hpp"

#include "Engine/MathUtil.hpp"
#include "Engine/Transform.hpp"
#include "Engine/Scene.hpp"

#include <chrono>
#include <functional>
#include <numbers>
#include <optional>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

namespace
{
	constexpr auto kCoarsePreviewModeExitDelay = std::chrono::milliseconds(500);
}

Viewer::Viewer(size_t width, size_t height)
	: m_renderer(width, height, std::thread::hardware_concurrency())
	, m_window(sf::VideoMode(static_cast<uint32_t>(width), static_cast<uint32_t>(height)), "Ray Tracer", sf::Style::Titlebar | sf::Style::Close)
{
	m_icon.loadFromFile("Assets/Icon.png");

	m_window.setIcon(m_icon.getSize().x, m_icon.getSize().y, m_icon.getPixelsPtr());
	m_window.setFramerateLimit(60);

	m_texture.create(m_window.getSize().x, m_window.getSize().y);

	m_sprite.setTexture(m_texture);

	m_font.loadFromFile("Assets/ConsolaMono-Book.ttf");

	m_infoText.setFont(m_font);
	m_infoText.setCharacterSize(16);
	m_infoText.setOutlineColor(sf::Color::Black);
	m_infoText.setOutlineThickness(1);
	m_infoText.setFillColor(sf::Color::White);
	m_infoText.move(10, 10);

	std::string instructionsMessage;
	instructionsMessage += "(Backspace) Reload scene\n";
	instructionsMessage += "(Enter) Save Image to File\n";
	instructionsMessage += "(Space) Restart Render\n";
	instructionsMessage += "(Delete) Cancel Render\n";
	instructionsMessage += "(W/A/S/D, R/F) Move Camera\n";
	instructionsMessage += "(I/J/K/L, U/O) Rotate Camera\n";

	m_instructionsText.setFont(m_font);
	m_instructionsText.setCharacterSize(16);
	m_instructionsText.setOutlineColor(sf::Color::Black);
	m_instructionsText.setOutlineThickness(1);
	m_instructionsText.setFillColor(sf::Color::White);
	m_instructionsText.setString(instructionsMessage);
	m_instructionsText.move(width - m_instructionsText.getGlobalBounds().width - 10, 10);
}

void Viewer::view(const std::string& path)
{
	enum class RenderType { CoarsePreview, Preview, Full };

	SceneLoader sceneLoader;

	RenderType nextRenderType = RenderType::Preview;
	RenderType previousRenderType = RenderType::Preview;
	std::chrono::steady_clock::time_point lastCoarsePreviewTime = {};
	bool wasRendering = false;
	bool infoTextUpdatePending = false;
	bool sceneUpdatePending = false;
	uint8_t lastRenderPercent = 0;
	std::string extraInfoMessage;

	std::optional<Scene> scene;
	uint32_t fullQualitySamplesPerPixel = 100;

	try
	{
		scene = sceneLoader.load(path);
		fullQualitySamplesPerPixel = scene->samplesPerPixel;
		sceneUpdatePending = true;
	}
	catch (const std::exception& e)
	{
		extraInfoMessage = std::string("Failed to parse scene file: ") + e.what();
		infoTextUpdatePending = true;
	}

	m_window.setTitle("Ray Tracer - '" + path + "'");

	while (m_window.isOpen())
	{
	    sf::Event event;
	    while (m_window.pollEvent(event))
	    {
			if (event.type == sf::Event::Closed)
			{
				m_window.close();
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
					case sf::Keyboard::Key::Backspace:
					{
						m_renderer.stopRender();
						m_renderer.clear();

						try
						{
							scene = sceneLoader.load(path);
							fullQualitySamplesPerPixel = scene->samplesPerPixel;

							nextRenderType = RenderType::CoarsePreview;
							sceneUpdatePending = true;
						}
						catch (const std::exception& e)
						{
							scene.reset();

							extraInfoMessage = std::string("Failed to parse scene file: ") + e.what();
							infoTextUpdatePending = true;
						}

						break;
					}

					case sf::Keyboard::Key::Enter:
					{
						const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

						char timestampBuffer[64];
						std::strftime(timestampBuffer, std::size(timestampBuffer), "%Y-%m-%d %H%M%S", std::localtime(&now));
						timestampBuffer[std::size(timestampBuffer) - 1] = '0';

						std::string fileName = "Render Output " + std::string(timestampBuffer) + ".jpg";
						if (m_texture.copyToImage().saveToFile(fileName))
							extraInfoMessage = "Saved image to '" + fileName + "'";
						else
							extraInfoMessage = "Failed to save image to '" + fileName + "'";

						infoTextUpdatePending = true;
						break;
					}

					case sf::Keyboard::Key::Space:
					{
						m_renderer.stopRender();
						m_renderer.clear();

						nextRenderType = RenderType::CoarsePreview;
						sceneUpdatePending = true;
						break;
					}

					case sf::Keyboard::Key::Delete:
					{
						m_renderer.stopRender();

						if (wasRendering)
						{
							extraInfoMessage = "Render cancelled prematurely by user.";
							infoTextUpdatePending = true;
						}

						break;
					}

					case sf::Keyboard::Key::W:
					case sf::Keyboard::Key::A:
					case sf::Keyboard::Key::S:
					case sf::Keyboard::Key::D:
					case sf::Keyboard::Key::R:
					case sf::Keyboard::Key::F:
					case sf::Keyboard::Key::I:
					case sf::Keyboard::Key::J:
					case sf::Keyboard::Key::K:
					case sf::Keyboard::Key::L:
					case sf::Keyboard::Key::U:
					case sf::Keyboard::Key::O:
					{
						static constexpr auto kMoveDelta	= .1;
						static constexpr auto kRotateDelta	= MathUtil::DegreesToRadians(.5);

						auto MoveCamera =
							[&](const Vector& amount)
							{
								Transform newTransform = scene->camera.transform();

								newTransform.setPosition(newTransform.position() + newTransform.untransformDirection(amount));

								scene->camera.setTransform(newTransform);
							};

						auto RotateCamera =
							[&](const Vector& amount)
							{
								Transform newTransform = scene->camera.transform();

								newTransform.setRotation(newTransform.rotation() + amount);

								scene->camera.setTransform(newTransform);
							};

						static const std::map<sf::Keyboard::Key, std::function<void()>> action
							{
								{ sf::Keyboard::Key::W, [&]() { MoveCamera(StandardVectors::kUnitZ * kMoveDelta); } },
								{ sf::Keyboard::Key::A, [&]() { MoveCamera(StandardVectors::kUnitX.inverted() * kMoveDelta); } },
								{ sf::Keyboard::Key::S, [&]() { MoveCamera(StandardVectors::kUnitZ.inverted() * kMoveDelta); } },
								{ sf::Keyboard::Key::D, [&]() { MoveCamera(StandardVectors::kUnitX * kMoveDelta); } },
								{ sf::Keyboard::Key::R, [&]() { MoveCamera(StandardVectors::kUnitY * kMoveDelta); } },
								{ sf::Keyboard::Key::F, [&]() { MoveCamera(StandardVectors::kUnitY.inverted() * kMoveDelta); } },
								{ sf::Keyboard::Key::I, [&]() { RotateCamera(StandardVectors::kUnitZ.inverted() * kRotateDelta); } },
								{ sf::Keyboard::Key::J, [&]() { RotateCamera(StandardVectors::kUnitY.inverted() * kRotateDelta); } },
								{ sf::Keyboard::Key::K, [&]() { RotateCamera(StandardVectors::kUnitZ * kRotateDelta); } },
								{ sf::Keyboard::Key::L, [&]() { RotateCamera(StandardVectors::kUnitY * kRotateDelta); } },
								{ sf::Keyboard::Key::U, [&]() { RotateCamera(StandardVectors::kUnitX * kRotateDelta); } },
								{ sf::Keyboard::Key::O, [&]() { RotateCamera(StandardVectors::kUnitX.inverted() * kRotateDelta); } },
							};

						action.at(event.key.code)();

						nextRenderType = RenderType::CoarsePreview;
						sceneUpdatePending = true;
						break;
					}

					default:
					{
						break;
					}
				}
			}
		}

		if (sceneUpdatePending && scene.has_value())
		{
			switch (nextRenderType)
			{
				case RenderType::CoarsePreview:
				case RenderType::Preview:
					scene->samplesPerPixel = 1;
					break;
				case RenderType::Full:
					scene->samplesPerPixel = fullQualitySamplesPerPixel;
					break;
			}

			// If we're moving, wait for the existing coarse preview to finish before
			// starting the next, so the entire (coarse) preview is visible.
			if (nextRenderType == RenderType::CoarsePreview && previousRenderType == RenderType::CoarsePreview)
				m_renderer.waitForRenderCompletion();

			m_renderer.setScene(scene.value());
			m_renderer.setCoarsePreview(nextRenderType == RenderType::CoarsePreview);
			m_renderer.startRender();

			wasRendering = true;
			previousRenderType = nextRenderType;
			lastRenderPercent = 0;

			sceneUpdatePending = false;
			infoTextUpdatePending = true;
		}

		const bool isRendering = m_renderer.isRendering();
		if (isRendering || wasRendering)
			m_texture.update(reinterpret_cast<const sf::Uint8*>(m_renderer.pixels()));

		if (isRendering != wasRendering)
		{
			wasRendering = isRendering;

			if (previousRenderType == RenderType::CoarsePreview)
				lastCoarsePreviewTime = std::chrono::steady_clock::now();

			infoTextUpdatePending = true;
		}

		if (! isRendering)
		{
			switch (previousRenderType)
			{
				case RenderType::CoarsePreview:
					// Delay transition out of coarse preview mode so we don't hammer the system
					// with repeated higher quality renders that get quickly abandoned.
					if (std::chrono::steady_clock::now() - lastCoarsePreviewTime >= kCoarsePreviewModeExitDelay)
						nextRenderType = RenderType::Preview;
					break;

				case RenderType::Preview:
					nextRenderType = RenderType::Full;
					break;

				case RenderType::Full:
					break;
			}

			sceneUpdatePending = nextRenderType != previousRenderType;
		}

		if (lastRenderPercent != m_renderer.renderPercentage())
		{
			lastRenderPercent = m_renderer.renderPercentage();
			infoTextUpdatePending = true;
		}

		if (infoTextUpdatePending)
		{
			std::string infoMessage;

			if (scene)
			{
				infoMessage += "Camera Position: " + scene->camera.transform().position().string() + "\n";
				infoMessage += "Camera Rotation: " + scene->camera.transform().rotation().string() + "\n";
				infoMessage += "Camera Scale:    " + scene->camera.transform().scale().string() + "\n";

				if (isRendering)
					infoMessage += std::string("Rendering In Progress (" + std::string(previousRenderType != RenderType::Full ? "Preview" : "Full") + " - " + std::to_string(m_renderer.renderPercentage()) + "%, " + std::to_string(scene->samplesPerPixel) + " samples/pixel)");
				else
					infoMessage += std::string("Rendering Completed (") + std::to_string(m_renderer.renderTime().count()) + " ms)";
			}

			if (! extraInfoMessage.empty())
			{
				infoMessage += "\n\n" + extraInfoMessage;
				extraInfoMessage.clear();
			}

			m_infoText.setString(infoMessage);

			infoTextUpdatePending = false;
		}

		m_window.draw(m_sprite);
		m_window.draw(m_instructionsText);
		m_window.draw(m_infoText);
		m_window.display();
	}
}
