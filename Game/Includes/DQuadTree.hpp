#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <Engine/Data/QuadTree/QuadTree.hpp>

class DQuadTree : public qdt::QuadTree
{
public:
	explicit DQuadTree(float x, float y, float width, float height) : DQuadTree(x, y, width, height, 64.f, 64.f) {}
	explicit DQuadTree(float x, float y, float width, float height, float minQuadWidth, float minQuadHeight) : QuadTree(x, y, width, height, minQuadWidth, minQuadHeight)
	{
		bounds.setOutlineThickness(1.f);
		bounds.setOutlineColor(sf::Color::Black);
		bounds.setFillColor(sf::Color::Transparent);
	}

	void draw(sf::RenderWindow& window) {
		traverse([&](const qdt::Rectangle& rectangle) {
			bounds.setPosition(sf::Vector2f(rectangle.x, rectangle.y));
			bounds.setSize(sf::Vector2f(rectangle.width, rectangle.height));
			window.draw(bounds);
		});
	}

private:
	sf::RectangleShape bounds;
};