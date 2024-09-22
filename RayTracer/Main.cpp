#include <SFML/Graphics.hpp>

#include <vector>

namespace
{
	constexpr auto kWidth = 800;
	constexpr auto kHeight = 600;
}

int main(int argc, char* argv[])
{
	sf::RenderWindow window(sf::VideoMode(kWidth, kHeight), "RayTracer");

	std::vector<sf::Uint8> pixels(kWidth * kHeight * 4);
	memset(pixels.data(), 0xFF, pixels.size());

	sf::Texture texture;
	texture.create(kWidth, kHeight);

	sf::Sprite sprite;
	sprite.setTexture(texture);

	while (window.isOpen())
	{
	    sf::Event event;
	    while (window.pollEvent(event))
	    {
	        switch (event.type)
	        {
	            case sf::Event::Closed:
	            {
	                window.close();
	                break;
	            }

	            default:
	                break;
			}
		}

		texture.update(pixels.data());

		window.draw(sprite);

		window.display();
	}
}
