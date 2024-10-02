#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"

#include "ExampleScene.hpp"

#include <SFML/Graphics.hpp>

#include <stddef.h>
#include <stdexcept>
#include <stdint.h>

namespace
{
	constexpr size_t	kWidth		= 1920;
	constexpr size_t	kHeight		= 1080;

	constexpr uint32_t	kUpdateFps	= 60;
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

	Renderer renderer(kWidth, kHeight, std::thread::hardware_concurrency());
	renderer.setScene(ExampleScene::Build());
	renderer.startRender();

	window.setTitle("Ray Tracer - Rendering In Progress");

	bool wasRendering = true;

	while (window.isOpen())
	{
	    sf::Event event;
	    while (window.pollEvent(event))
	    {
	        if (event.type == sf::Event::Closed)
				window.close();
		}

		const bool isRendering = renderer.isRendering();
		if (isRendering || wasRendering)
			texture.update(reinterpret_cast<const sf::Uint8*>(renderer.pixels()));

		if (! isRendering && wasRendering)
		{
			window.setTitle(std::string("Ray Tracer - Rendering Completed (") + std::to_string(renderer.renderTime().count()) + " ms)");
			texture.copyToImage().saveToFile("Output.jpg");
		}

		wasRendering = isRendering;

		window.draw(sprite);
		window.display();
	}
}
