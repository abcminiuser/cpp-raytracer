#include "Engine/Renderer.hpp"

#include <SFML/Graphics.hpp>

namespace
{
	constexpr auto kWidth = 800;
	constexpr auto kHeight = 600;
}

int main(int argc, char* argv[])
{
	sf::RenderWindow window(sf::VideoMode(kWidth, kHeight), "RayTracer");
	window.setFramerateLimit(5);

	sf::Texture texture;
	texture.create(kWidth, kHeight);

	sf::Sprite sprite;
	sprite.setTexture(texture);

	Renderer renderer(kWidth, kHeight, 8);
	renderer.setRenderingEnabled(true);

	while (window.isOpen())
	{
	    sf::Event event;
	    while (window.pollEvent(event))
	    {
	        if (event.type == sf::Event::Closed)
				window.close();
		}

		texture.update(reinterpret_cast<const sf::Uint8*>(renderer.pixels()));

		window.draw(sprite);
		window.display();
	}
}
