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

	constexpr uint32_t	kUpdateFps	= 10;
}

int main(int argc, char* argv[])
{
	sf::RenderWindow window(sf::VideoMode(kWidth, kHeight), "RayTracer", sf::Style::Titlebar | sf::Style::Close);
	window.setSize(sf::Vector2u(kWidth, kHeight));
	window.setFramerateLimit(kUpdateFps);

	sf::Texture texture;
	texture.create(kWidth, kHeight);

	sf::Sprite sprite;
	sprite.setTexture(texture);

	Renderer renderer(kWidth, kHeight, std::thread::hardware_concurrency());
	renderer.setScene(ExampleScene::Build());
	renderer.startRender();

	const auto updateTitle = [&](bool rendering) { window.setTitle(std::string("Ray Tracer - Rendering ") + (rendering ? "In Progress" : "Done")); };
	const auto updateTexture = [&]() { texture.update(reinterpret_cast<const sf::Uint8*>(renderer.pixels())); };

	updateTitle(true);

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
			updateTexture();

		if (isRendering != wasRendering)
		{
			updateTitle(isRendering);
			wasRendering = isRendering;

			if (! isRendering)
				texture.copyToImage().saveToFile("Output.png");
		}

		window.draw(sprite);
		window.display();
	}
}
