#pragma once

#include <SFML/Graphics.hpp>

class Renderer;

struct Scene;

class Viewer
{
public:
						Viewer(Renderer& renderer, size_t width, size_t height);

	void				view(Scene scene);

private:
	Renderer&			m_renderer;

	sf::Image			m_icon;
	sf::RenderWindow	m_window;
	sf::Texture			m_texture;
	sf::Sprite			m_sprite;
	sf::Font			m_font;
	sf::Text			m_infoText;
};
