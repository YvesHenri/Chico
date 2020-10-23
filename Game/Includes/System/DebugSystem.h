#pragma once

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <Engine/Entities/System/System.hpp>

#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

class DebugSystem : public ecs::System
{
public:
	explicit DebugSystem(const std::shared_ptr<sf::RenderWindow>& window) : window(window) {
		auto cartesianColor = sf::Color::Black;
		text.setFont(FontStore::get("Carlito.ttf"));
		text.setCharacterSize(16U);
		xaxis[0].color = cartesianColor;
		xaxis[1].color = cartesianColor;
		xaxis[0].position = sf::Vector2f(0.f, 300.f);
		xaxis[1].position = sf::Vector2f(800.f, 300.f);
		yaxis[0].color = cartesianColor;
		yaxis[1].color = cartesianColor;
		yaxis[0].position = sf::Vector2f(400.f, 0.f);
		yaxis[1].position = sf::Vector2f(400.f, 600.f);
	}

	void update(float dt) override {
		entities->each<Motion, Transform, Body>([&](auto& entity, auto& motion, auto& transform, auto& body) {
			auto fixed = window->getDefaultView();
			auto current = window->getView();
			
			// FPS
			window->setTitle("FPS: " + std::to_string(1.f / dt));
			window->setView(fixed);			

			// Player stats
			if (entity.has<Joystick>()) {
				drawText(5.f, 4.f, sf::Color::Black, "Velocity = " + std::to_string(motion.velocity.x) + ", " + std::to_string(motion.velocity.y) + " = " + std::to_string(motion.velocity.magnitude()));
				drawText(5.f, 24.f, sf::Color::Black, "Position = " + std::to_string(transform.x) + ", " + std::to_string(transform.y));
				drawText(5.f, 44.f, sf::Color::Black, "Mass = " + std::to_string(body.mass));
				drawText(5.f, 64.f, sf::Color::Black, "Max speed = " + std::to_string(motion.speed));
				drawText(5.f, 84.f, sf::Color::Black, "Thrust = " + std::to_string(motion.thrust));
				drawText(5.f, 104.f, sf::Color::Black, "View = " + std::to_string(current.getCenter().x) + ", " + std::to_string(current.getCenter().y));
			}

			// Cartesian plane
			window->draw(xaxis, 2, sf::PrimitiveType::Lines);
			window->draw(yaxis, 2, sf::PrimitiveType::Lines);

			window->setView(current);
		});
	}

private:
	void drawText(float x, float y, sf::Color color, const sf::String& string) {
		text.setString(string);
		text.setFillColor(color);
		text.setPosition(x, y);
		window->draw(text);
	}

private:
	sf::Text text;
	sf::Vertex xaxis[2];
	sf::Vertex yaxis[2];
	std::shared_ptr<sf::RenderWindow> window;
};