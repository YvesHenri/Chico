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
		text.setFillColor(sf::Color::Blue);
		xaxis[0].color = sf::Color::Black;
		xaxis[1].color = sf::Color::Black;
		xaxis[0].position = sf::Vector2f(0.f, 300.f);
		xaxis[1].position = sf::Vector2f(800.f, 300.f);
		yaxis[0].color = sf::Color::Black;
		yaxis[1].color = sf::Color::Black;
		yaxis[0].position = sf::Vector2f(400.f, 0.f);
		yaxis[1].position = sf::Vector2f(400.f, 600.f);
	}

	void update(float dt) override {
		entities->each<Motion, Transform>([&](auto& entity, auto& motion, auto& transform) {
			auto fixed = window->getDefaultView();
			auto current = window->getView();

			// FPS
			text.setPosition(5.f, 5.f);
			text.setString("FPS: " + std::to_string(1.f / dt));

			window->setView(fixed);
			window->draw(text);
			
			// Player
			if (entity.has<Joystick>()) {
				// Velocity
				text.setString("Velocity = " + std::to_string(motion.velocity.x) + ", " + std::to_string(motion.velocity.y) + " = " + std::to_string(motion.velocity.magnitude()));
				text.setPosition(5.f, 24.f);
				window->draw(text);

				// Acceleration
				//text.setString("Acceleration = " + std::to_string(motion.acceleration.x) + ", " + std::to_string(motion.acceleration.y));
				//text.setPosition(5.f, 44.f);
				//window->draw(text);

				// Player position
				text.setString("Player = " + std::to_string(transform.x) + ", " + std::to_string(transform.y));
				text.setPosition(5.f, 64.f);
				window->draw(text);
			}

			// Cartesian plane
			//window->draw(xaxis, 2, sf::PrimitiveType::Lines);
			//window->draw(yaxis, 2, sf::PrimitiveType::Lines);

			window->setView(current);
		});
	}

private:
	sf::Text text;
	sf::Vertex xaxis[2];
	sf::Vertex yaxis[2];
	std::shared_ptr<sf::RenderWindow> window;
};