#pragma once

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <Engine/Entities/System/System.hpp>

#include "../Component/Body.h"
#include "../Component/Render.h"
#include "../Component/Transform.h"

class RenderSystem : public ecs::System
{
public:
	explicit RenderSystem(const std::shared_ptr<sf::RenderWindow>& window) : window(window) {
		text.setFont(FontStore::get("Carlito.ttf"));
		text.setCharacterSize(24U);
		text.setFillColor(sf::Color::Black);
	}

	void update(float dt) override {
		sf::CircleShape shape;
		sf::RectangleShape line;

		shape.setOutlineThickness(1.f);
		shape.setOutlineColor(sf::Color::Black);
		line.setOutlineThickness(0.75f);
		line.setOutlineColor(sf::Color::Black);

		entities->each<Body, Render, Transform, Motion>([&](auto& entity, auto& body, auto& render, auto& transform, auto& motion) {
			// Entity shape
			shape.setRadius(body.radius);
			shape.setOrigin(shape.getRadius(), shape.getRadius());
			shape.setPosition(transform.x, transform.y);
			shape.setFillColor(render.color);

			// Entity identifier
			text.setString(std::to_string(entity.id()));
			auto rect = text.getLocalBounds();
			text.setOrigin(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f); // width / 2, height / 2
			text.setPosition(transform.x, transform.y);

			// Entity rotation cue (velocity)
			line.setPosition(transform.x, transform.y);
			line.setSize(sf::Vector2f(body.radius + 16.f, 0.f));
			line.setRotation(motion.velocity.angle().degrees());

			window->draw(shape);
			window->draw(text);
			window->draw(line);
		});
	}

private:
	sf::Text text;
	std::shared_ptr<sf::RenderWindow> window;
};