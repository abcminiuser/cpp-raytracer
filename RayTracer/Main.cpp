#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Sphere.hpp"

#include <SFML/Graphics.hpp>

namespace
{
	constexpr size_t	kWidth		= 1920;
	constexpr size_t	kHeight		= 1080;

	constexpr uint32_t	kUpdateFps	= 5;

	Scene BuildScene()
	{
		Scene scene;

		scene.background = Color(64, 64, 64);

		scene.camera = Camera(Vector(0, 2, -8), StandardVectors::kUnitZ, 4.0f, 9.0f / 4.0f);

		scene.lights.push_back(std::make_unique<Light>(Vector(5, 10, -5), Palette::kWhite));

		scene.objects.push_back(std::make_unique<Sphere>(Vector(-4, 0, 4), 1.0f, Material{.color = Palette::kYellow }));
		scene.objects.push_back(std::make_unique<Sphere>(Vector(-2, 0, 2), 1.0f, Material{.color = Palette::kWhite }));
		scene.objects.push_back(std::make_unique<Sphere>(Vector(0, 0, 0), 1.0f, Material{.color = Palette::kRed }));
		scene.objects.push_back(std::make_unique<Sphere>(Vector(2, 0, 2), 1.0f, Material{.color = Palette::kGreen }));
		scene.objects.push_back(std::make_unique<Sphere>(Vector(4, 0, 4), 1.0f, Material{.color = Palette::kBlue }));

		return scene;
	}
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
	renderer.setScene(BuildScene());
	renderer.startRender();

	const auto updateTitle = [&](bool rendering) { window.setTitle(std::string("Ray Tracer - Rendering ") + (rendering ? "In Progress" : "Done")); };

	updateTitle(false);

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

		if (isRendering != wasRendering)
		{
			updateTitle(isRendering);
			wasRendering = isRendering;
		}

		window.draw(sprite);
		window.display();
	}
}
