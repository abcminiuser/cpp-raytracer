﻿#pragma once

#include "Engine/Matrix.hpp"
#include "Engine/Renderer.hpp"
#include "Engine/Vector.hpp"

#include <string>

#include <SFML/Graphics.hpp>

class Viewer
{
public:
						Viewer(size_t width, size_t height);

	void				view(const std::string& path);

private:
	Renderer			m_renderer;

	sf::Image			m_icon;
	sf::RenderWindow	m_window;
	sf::Texture			m_texture;
	sf::Sprite			m_sprite;
	sf::Font			m_font;
	sf::Text			m_instructionsText;
	sf::Text			m_infoText;
};
