#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Sphere.hpp"

#include <SFML/Graphics.hpp>

namespace
{
	constexpr auto kWidth		= 1920;
	constexpr auto kHeight		= 1080;

	constexpr auto kUpdateFps	= 5;
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

	Scene scene;
	scene.camera = Camera(Vector(0, 2, -8), StandardVectors::kUnitZ, 4.0f, 9.0f/4.0f);
	scene.lights = {
		std::make_shared<Light>(Vector(5, 10, -5), Palette::kWhite),
	};
	scene.objects = {
		std::make_shared<Sphere>(Vector(-4, 0, 4), 1.0f, Palette::kYellow),
		std::make_shared<Sphere>(Vector(-2, 0, 2), 1.0f, Palette::kWhite),
		std::make_shared<Sphere>(Vector( 0, 0, 0), 1.0f, Palette::kRed),
		std::make_shared<Sphere>(Vector( 2, 0, 2), 1.0f, Palette::kGreen),
		std::make_shared<Sphere>(Vector( 4, 0, 4), 1.0f, Palette::kBlue),
	};

	Renderer renderer(kWidth, kHeight, std::thread::hardware_concurrency());
	renderer.setScene(std::move(scene));
	renderer.startRender();

	bool wasRendering = true;

	auto updateTitle = [&](bool rendering) { window.setTitle(std::string("Ray Tracer - Rendering ") + (rendering ? "In Progress" : "Done")); };

	updateTitle(false);

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

		if (isRendering != wasRendering)
		{
			updateTitle(isRendering);
			wasRendering = isRendering;
		}

		window.draw(sprite);
		window.display();
	}
}
