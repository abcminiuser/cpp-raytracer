#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Object/SphereObject.hpp"
#include "Engine/Texture/SolidTexture.hpp"
#include "Engine/Texture/CheckerboardTexture.hpp"
#include "Engine/Texture/ImageTexture.hpp"

#include <SFML/Graphics.hpp>

namespace
{
	constexpr size_t	kWidth		= 1920;
	constexpr size_t	kHeight		= 1080;

	constexpr uint32_t	kUpdateFps	= 10;

	std::unique_ptr<ImageTexture> MakeImageTexture(const std::string& path)
	{
		sf::Image imageTexture;
		imageTexture.loadFromFile(path);

		const auto dimensions = imageTexture.getSize();

		return std::make_unique<ImageTexture>(dimensions.x, dimensions.y,reinterpret_cast<const uint32_t*>(imageTexture.getPixelsPtr()));
	}

	Scene BuildScene()
	{
		Scene scene;

		scene.background = Palette::kWhite.scale(0.1);

		scene.camera = Camera(Vector(0, 2, -8), StandardVectors::kUnitZ, 4.0, 9.0 / 4.0);

		scene.lights.push_back(std::make_unique<Light>(Vector(5, 10, -5), Palette::kWhite));

		scene.objects.push_back(std::make_unique<SphereObject>(Vector(-4, 0, 4), Material{ .texture = std::make_unique<SolidTexture>(Palette::kWhite) }, 1.0));
		scene.objects.push_back(std::make_unique<SphereObject>(Vector(-2, 0, 2), Material{ .texture = std::make_unique<CheckerboardTexture>(Palette::kMagenta, Palette::kYellow, 8) }, 1.0));
		scene.objects.push_back(std::make_unique<SphereObject>(Vector( 0, 0, 0), Material{ .texture = std::make_unique<SolidTexture>(Palette::kRed) }, 1.0));
		scene.objects.push_back(std::make_unique<SphereObject>(Vector( 2, 0, 2), Material{ .texture = MakeImageTexture("Assets/Test.png")}, 1.0));
		scene.objects.push_back(std::make_unique<SphereObject>(Vector( 4, 0, 4), Material{ .texture = std::make_unique<SolidTexture>(Palette::kBlue) }, 1.0));

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

			if (! isRendering)
				texture.copyToImage().saveToFile("Output.png");
		}

		window.draw(sprite);
		window.display();
	}
}
