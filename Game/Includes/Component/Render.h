#pragma once

#include <SFML/Graphics/Color.hpp>

#include "../Resource/ResourceStore.hpp"

struct Render
{
	explicit Render(sf::Color color) : color(color) {}

	sf::Color color;
};