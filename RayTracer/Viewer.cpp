#include "Viewer.hpp"
#include "SceneLoader.hpp"

#include "Engine/Matrix.hpp"
#include "Engine/Scene.hpp"

#include <chrono>
#include <numbers>
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

	RenderType nextRenderType = RenderType::Preview;
	RenderType previousRenderType = RenderType::Preview;
	std::chrono::steady_clock::time_point lastCoarsePreviewTime = {};
	bool wasRendering = false;
	bool infoTextUpdatePending = false;
	bool sceneUpdatePending = false;
	uint8_t lastRenderPercent = 0;
	std::string extraInfoMessage;

	std::optional<Scene> scene;

	try
	{
		scene = SceneLoader::Load(path);
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
							scene = SceneLoader::Load(path);

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
					{
						constexpr auto kMoveDelta = .1;

						static const std::map<sf::Keyboard::Key, Vector> cameraMoveAmount
							{
								{ sf::Keyboard::Key::W, Vector(0, 0, kMoveDelta) },
								{ sf::Keyboard::Key::A, Vector(-kMoveDelta, 0, 0) },
								{ sf::Keyboard::Key::S, Vector(0, 0,-kMoveDelta) },
								{ sf::Keyboard::Key::D, Vector(kMoveDelta, 0, 0) },
								{ sf::Keyboard::Key::R, Vector(0, kMoveDelta, 0) },
								{ sf::Keyboard::Key::F, Vector(0, -kMoveDelta, 0) },
							};

						scene->camera.setPosition(scene->camera.position().add(cameraMoveAmount.at(event.key.code)));

						nextRenderType = RenderType::CoarsePreview;
						sceneUpdatePending = true;
						break;
					}

					case sf::Keyboard::Key::I:
					case sf::Keyboard::Key::J:
					case sf::Keyboard::Key::K:
					case sf::Keyboard::Key::L:
					{
						constexpr auto kRotateDelta = std::numbers::pi / 360.0;

						static const std::map<sf::Keyboard::Key, Matrix<3, 3>> cameraRotateAmount
							{
								{ sf::Keyboard::Key::I, MatrixUtils::RotationMatrix(Vector(0, 0, -kRotateDelta)) },
								{ sf::Keyboard::Key::J, MatrixUtils::RotationMatrix(Vector(0, -kRotateDelta, 0)) },
								{ sf::Keyboard::Key::K, MatrixUtils::RotationMatrix(Vector(0, 0, kRotateDelta)) },
								{ sf::Keyboard::Key::L, MatrixUtils::RotationMatrix(Vector(0, kRotateDelta, 0)) },
							};

						scene->camera.setDirection(cameraRotateAmount.at(event.key.code).multiply(scene->camera.direction()).toVector().unit());

						nextRenderType = RenderType::CoarsePreview;
						sceneUpdatePending = true;
						break;
					}

					case sf::Keyboard::Key::U:
					case sf::Keyboard::Key::O:
					{
						constexpr auto kRotateDelta = std::numbers::pi / 360.0;

						static const std::map<sf::Keyboard::Key, Matrix<3, 3>> cameraRotateAmount
							{
								{ sf::Keyboard::Key::U, MatrixUtils::RotationMatrix(Vector(kRotateDelta, 0, 0)) },
								{ sf::Keyboard::Key::O, MatrixUtils::RotationMatrix(Vector(-kRotateDelta, 0, 0)) },
							};

						scene->camera.setOrientation(cameraRotateAmount.at(event.key.code).multiply(scene->camera.orientation()).toVector().unit());

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
					scene->maxRayDepth = 1;
					scene->samplesPerPixel = 1;
					break;
				case RenderType::Preview:
					scene->maxRayDepth = 5;
					scene->samplesPerPixel = 1;
					break;
				case RenderType::Full:
					scene->maxRayDepth = 10;
					scene->samplesPerPixel = 150;
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
					// Delay trransition out of coarse preview mode so we don't hammer the system
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
				infoMessage += "Camera Position:  " + scene->camera.position().string() + "\n";
				infoMessage += "Camera Direction: " + scene->camera.direction().string() + "\n";
				infoMessage += "Camera Orientation: " + scene->camera.orientation().string() + "\n";

				if (isRendering)
					infoMessage += std::string("Rendering In Progress (" + std::string(previousRenderType != RenderType::Full ? "Preview" : "Full") + " - " + std::to_string(m_renderer.renderPercentage()) + "%)");
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
