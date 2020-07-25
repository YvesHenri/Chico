#pragma once

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <Engine/Entities/System/System.hpp>

#include "../Component/Transform.h"
#include "../Component/Motion.h"

class DebugSystem : public ecs::System
{
public:
	explicit DebugSystem(const std::shared_ptr<sf::RenderWindow>& window) : window(window) {
		text.setFont(FontStore::get("Carlito.ttf"));
		text.setCharacterSize(16U);
		text.setColor(sf::Color::Black);
		x[0].color = sf::Color::Black;
		x[1].color = sf::Color::Black;
		x[0].position = sf::Vector2f(0.f, 300.f);
		x[1].position = sf::Vector2f(800.f, 300.f);
		y[0].color = sf::Color::Black;
		y[1].color = sf::Color::Black;
		y[0].position = sf::Vector2f(400.f, 0.f);
		y[1].position = sf::Vector2f(400.f, 600.f);
	}

	void draw(float dt) override {}

	void update(float dt) override {
		entities->each<Motion, Transform>([&](auto& entity, auto& motion, auto& transform) {
			auto fixed = window->getDefaultView();
			auto current = window->getView();

			// FPS
			text.setPosition(5.f, 5.f);
			text.setString("FPS: " + std::to_string(1.f / dt));
			window->setView(fixed);
			window->draw(text);
			
			// Player velocity
			if (entity.id() == 0U) {
				text.setString("Vx, Vy = " + std::to_string(motion.ax) + ", " + std::to_string(motion.ay));
				text.setPosition(5.f, 24.f);
				window->draw(text);
			}			

			// Player position
			if (entity.id() == 0U) {
				text.setString("X, Y = " + std::to_string(transform.x) + ", " + std::to_string(transform.y));
				text.setPosition(5.f, 44.f);
				window->draw(text);
			}

			// Cartesian plane
			window->draw(x, 2, sf::PrimitiveType::Lines);
			window->draw(y, 2, sf::PrimitiveType::Lines);

			window->setView(current);
		});
	}

private:
	sf::Text text;
	sf::Vertex x[2];
	sf::Vertex y[2];
	std::shared_ptr<sf::RenderWindow> window;
};