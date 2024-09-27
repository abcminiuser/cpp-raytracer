#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Sphere.hpp"

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

	Scene scene;
	scene.camera = Camera(Vector(0, 2, -8), StandardVectors::kUnitZ, 4.0f, 9.0f/4.0f);
	scene.objects = {
		std::make_shared<Sphere>(Vector(-4, 0, 4), 1.0f, Palette::kYellow),
		std::make_shared<Sphere>(Vector(-2, 0, 2), 1.0f, Palette::kWhite),
		std::make_shared<Sphere>(Vector( 0, 0, 0), 1.0f, Palette::kRed),
		std::make_shared<Sphere>(Vector( 2, 0, 2), 1.0f, Palette::kGreen),
		std::make_shared<Sphere>(Vector( 4, 0, 4), 1.0f, Palette::kBlue),
	};

	Renderer renderer(kWidth, kHeight, 8);
	renderer.setRenderingEnabled(true);
	renderer.setScene(std::move(scene));

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
