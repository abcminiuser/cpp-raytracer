#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"

#include "ExampleScene.hpp"

#include <SFML/Graphics.hpp>

#include <chrono>
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
	bool isCameraLocked = true;

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
					case sf::Keyboard::Key::P:
					{
						isPreview = !isPreview;

						printf("Preview mode %s\n", isPreview ? "Enabled" : "Disabled");
						sceneUpdatePending = true;
						break;
					}

					case sf::Keyboard::Key::S:
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

					case sf::Keyboard::Key::R:
					{
						printf("Restarting render\n");
						renderer.stopRender();
						renderer.clear();

						sceneUpdatePending = true;
						break;
					}

					case sf::Keyboard::Key::L:
					{
						isCameraLocked = ! isCameraLocked;
						printf("Camera %s\n", isCameraLocked ? "Locked" : "Unlocked");

						infoTextUpdatePending = true;
						break;
					}

					default:
					{
						break;
					}
				}
			}
			else if (event.type == sf::Event::MouseWheelScrolled)
			{
				if (! isCameraLocked)
				{
					scene.camera.setPosition(scene.camera.position().add(Vector(0, 0, double(event.mouseWheelScroll.delta) / 100.0)));

					printf("Moved camera to (%f, %f, %f)\n", scene.camera.position().x(), scene.camera.position().y(), scene.camera.position().z());
					sceneUpdatePending = true;
				}
			}
		}

		if (sceneUpdatePending)
		{
			scene.maxRayDepth = isPreview ? 1 : 10;
			scene.allowReflections = !isPreview;
			scene.allowRefractions = !isPreview;
			scene.allowLighting = !isPreview;

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

			infoTextUpdatePending = true;
		}

		if (infoTextUpdatePending)
		{
			std::string infoMessage;
			infoMessage += std::string("(P)review (") + (isPreview ? "Enabled" : "Disabled") + ")\n";
			infoMessage += std::string("(S)ave Image") + "\n";
			infoMessage += std::string("(R)estart Render") + "\n";
			infoMessage += std::string("(L)ock/Unlock Camera (") + (isCameraLocked ? "Locked" : "Unlocked") + ")\n";

			infoMessage += "\n";
			infoMessage += "Camera Position: " + ToString(scene.camera.position()) + "\n";
			infoMessage += "Camera Direction: " + ToString(scene.camera.direction()) + "\n";
			infoMessage += isRendering ? std::string("Rendering In Progress") : std::string("Rendering Completed (") + std::to_string(renderer.renderTime().count()) + " ms)";

			infoText.setString(infoMessage);

			infoTextUpdatePending = false;
		}

		window.draw(sprite);
		window.draw(infoText);
		window.display();
	}
}
