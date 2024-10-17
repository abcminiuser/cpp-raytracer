#include "Viewer.hpp"

#include "Engine/Matrix.hpp"
#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"

#include <chrono>
#include <numbers>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

namespace
{
	constexpr uint32_t	kUpdateFps	= 30;
}

Viewer::Viewer(Renderer& renderer, size_t width, size_t height)
	: m_renderer(renderer)
	, m_window(sf::VideoMode(static_cast<uint32_t>(width), static_cast<uint32_t>(height)), "Ray Tracer", sf::Style::Titlebar | sf::Style::Close)
{
	m_icon.loadFromFile("Assets/Icon.png");

	m_window.setFramerateLimit(kUpdateFps);
	m_window.setIcon(m_icon.getSize().x, m_icon.getSize().y, m_icon.getPixelsPtr());

	m_texture.create(m_window.getSize().x, m_window.getSize().y);

	m_sprite.setTexture(m_texture);

	m_font.loadFromFile("Assets/ConsolaMono-Book.ttf");

	m_infoText.setFont(m_font);
	m_infoText.setCharacterSize(16);
	m_infoText.setOutlineColor(sf::Color::Black);
	m_infoText.setOutlineThickness(1);
	m_infoText.setFillColor(sf::Color::White);
	m_infoText.move(10, 10);
}

void Viewer::view(Scene scene)
{
	enum class RenderType { CoarsePreview, Preview, Full };

	RenderType nextRenderType = RenderType::Preview;
	RenderType previousRenderType = RenderType::Preview;
	bool wasRendering = false;
	bool infoTextUpdatePending = true;
	bool sceneUpdatePending = true;
	uint8_t lastRenderPercent = 0;

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
					case sf::Keyboard::Key::Enter:
					{
						const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

						char timestampBuffer[64];
						std::strftime(timestampBuffer, std::size(timestampBuffer), "%Y-%m-%d %H%M%S", std::localtime(&now));
						timestampBuffer[std::size(timestampBuffer) - 1] = '0';

						std::string fileName = "Render Output " + std::string(timestampBuffer) + ".jpg";
						if (m_texture.copyToImage().saveToFile(fileName))
							printf("Saved image to '%s'.\n", fileName.c_str());

						break;
					}

					case sf::Keyboard::Key::Backspace:
					{
						m_renderer.stopRender();
						m_renderer.clear();

						nextRenderType = RenderType::Full;
						sceneUpdatePending = true;
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

						scene.camera.setPosition(scene.camera.position().add(cameraMoveAmount.at(event.key.code)));

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

						scene.camera.setDirection(cameraRotateAmount.at(event.key.code).multiply(scene.camera.direction()).toVector().unit());

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

		if (sceneUpdatePending)
		{
			scene.lighting = nextRenderType != RenderType::CoarsePreview;
			scene.maxRayDepth = nextRenderType != RenderType::CoarsePreview ? 10 : 1;
			scene.sampledPerPixel = nextRenderType == RenderType::Full ? 100 : 1;

			if (nextRenderType == RenderType::CoarsePreview && previousRenderType == RenderType::CoarsePreview)
				m_renderer.waitForRenderCompletion();

			m_renderer.setScene(scene);
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

			if (! isRendering)
			{
				switch (previousRenderType)
				{
					case RenderType::CoarsePreview:
						nextRenderType = RenderType::Preview;
						sceneUpdatePending = true;
						break;

					case RenderType::Preview:
						nextRenderType = RenderType::Full;
						sceneUpdatePending = true;
						break;

					case RenderType::Full:
						break;
				}
			}

			infoTextUpdatePending = true;
		}

		if (lastRenderPercent != m_renderer.renderPercentage())
		{
			lastRenderPercent = m_renderer.renderPercentage();
			infoTextUpdatePending = true;
		}

		if (infoTextUpdatePending)
		{
			std::string infoMessage;
			infoMessage += std::string("(Enter) Save Image to File") + "\n";
			infoMessage += std::string("(Backspace) Restart Render") + "\n";
			infoMessage += std::string("(W/A/S/D, R/F) Move Camera\n");
			infoMessage += std::string("(I/J/K/L) Rotate Camera\n");

			infoMessage += "\n";
			infoMessage += "Camera Position:  " + scene.camera.position().string() + "\n";
			infoMessage += "Camera Direction: " + scene.camera.direction().string() + "\n";

			if (isRendering)
				infoMessage += std::string("Rendering In Progress (" + std::string(previousRenderType != RenderType::Full ? "Preview" : "Full") + " - " + std::to_string(m_renderer.renderPercentage()) + "%)");
			else
				infoMessage += std::string("Rendering Completed (") + std::to_string(m_renderer.renderTime().count()) + " ms)";

			m_infoText.setString(infoMessage);

			infoTextUpdatePending = false;
		}

		m_window.draw(m_sprite);
		m_window.draw(m_infoText);
		m_window.display();
	}
}
