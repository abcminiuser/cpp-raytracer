#include "Engine/Matrix.hpp"
#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"

#include "ExampleScene.hpp"

#include <SFML/Graphics.hpp>

#include <chrono>
#include <numbers>
#include <stddef.h>
#include <stdexcept>
#include <stdint.h>
#include <time.h>

namespace
{
	constexpr size_t	kWidth		= 1920;
	constexpr size_t	kHeight		= 1080;

	constexpr uint32_t	kUpdateFps	= 60;

	std::string ToString(const Vector& v)
	{
		char buffer[64];
		snprintf(buffer, std::size(buffer), "(% f, % f, % f)", v.x(), v.y(), v.z());
		buffer[std::size(buffer) - 1] = '\0';

		return buffer;
	}
}

int main(int argc, char* argv[])
{
	sf::RenderWindow window(sf::VideoMode(kWidth, kHeight), "Ray Tracer", sf::Style::Titlebar | sf::Style::Close);
	window.setSize(sf::Vector2u(kWidth, kHeight));
	window.setFramerateLimit(kUpdateFps);

	sf::Texture texture;
	texture.create(kWidth, kHeight);

	sf::Sprite sprite;
	sprite.setTexture(texture);

	sf::Font font;
	font.loadFromFile("Assets/JupiteroidLight-R90XW.ttf");

	sf::Text infoText;
	infoText.setFont(font);
	infoText.setCharacterSize(16);
	infoText.setOutlineColor(sf::Color::Black);
	infoText.setOutlineThickness(1);
	infoText.setFillColor(sf::Color::White);
	infoText.move(10, 10);

	auto scene = ExampleScene::Build();

	Renderer renderer(kWidth, kHeight, std::thread::hardware_concurrency());

	bool isPreview = false;

	bool wasRendering = false;
	bool infoTextUpdatePending = true;
	bool sceneUpdatePending = true;

	while (window.isOpen())
	{
	    sf::Event event;
	    while (window.pollEvent(event))
	    {
			if (event.type == sf::Event::Closed)
			{
				window.close();
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
						if (texture.copyToImage().saveToFile(fileName))
							printf("Saved image to '%s'.\n", fileName.c_str());

						break;
					}

					case sf::Keyboard::Key::Backspace:
					{
						printf("Restarting render\n");
						renderer.stopRender();
						renderer.clear();

						isPreview = false;
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

						static std::map<sf::Keyboard::Key, Vector> cameraMoveAmount
							{
								{ sf::Keyboard::Key::W, Vector(0, 0, kMoveDelta) },
								{ sf::Keyboard::Key::A, Vector(-kMoveDelta, 0, 0) },
								{ sf::Keyboard::Key::S, Vector(0, 0,-kMoveDelta) },
								{ sf::Keyboard::Key::D, Vector(kMoveDelta, 0, 0) },
								{ sf::Keyboard::Key::R, Vector(0, kMoveDelta, 0) },
								{ sf::Keyboard::Key::F, Vector(0, -kMoveDelta, 0) },
							};

						scene.camera.setPosition(scene.camera.position().add(cameraMoveAmount.at(event.key.code)));

						isPreview = true;
						sceneUpdatePending = true;
						break;
					}

					case sf::Keyboard::Key::I:
					case sf::Keyboard::Key::J:
					case sf::Keyboard::Key::K:
					case sf::Keyboard::Key::L:
					{
						constexpr auto kRotateDelta = std::numbers::pi / 360.0;

						static std::map<sf::Keyboard::Key, Matrix<3, 3>> cameraRotateAmount
						{
							{ sf::Keyboard::Key::I, MatrixUtils::RotationMatrix(Vector(0, 0, kRotateDelta)) },
							{ sf::Keyboard::Key::J, MatrixUtils::RotationMatrix(Vector(0, -kRotateDelta, 0)) },
							{ sf::Keyboard::Key::K, MatrixUtils::RotationMatrix(Vector(0, 0, -kRotateDelta)) },
							{ sf::Keyboard::Key::L, MatrixUtils::RotationMatrix(Vector(0, kRotateDelta, 0)) },
						};

						scene.camera.setDirection(cameraRotateAmount.at(event.key.code).multiply(scene.camera.direction()).toVector().unit());

						isPreview = true;
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
			scene.maxRayDepth = isPreview ? 1 : 10;

			renderer.setScene(scene);
			renderer.startRender();

			wasRendering = false;

			sceneUpdatePending = false;
			infoTextUpdatePending = true;
		}

		const bool isRendering = renderer.isRendering();
		if (isRendering || wasRendering)
			texture.update(reinterpret_cast<const sf::Uint8*>(renderer.pixels()));

		if (isRendering != wasRendering)
		{
			wasRendering = isRendering;

			if (! isRendering && isPreview)
			{
				isPreview = false;
				sceneUpdatePending = true;
			}

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
			infoMessage += "Camera Position: " + ToString(scene.camera.position()) + "\n";
			infoMessage += "Camera Direction: " + ToString(scene.camera.direction()) + "\n";

			if (isRendering)
				infoMessage += std::string("Rendering In Progress (" + std::string(isPreview ? "Preview" : "Full") + ")");
			else
				infoMessage += std::string("Rendering Completed (") + std::to_string(renderer.renderTime().count()) + " ms)";

			infoText.setString(infoMessage);

			infoTextUpdatePending = false;
		}

		window.draw(sprite);
		window.draw(infoText);
		window.display();
	}
}
