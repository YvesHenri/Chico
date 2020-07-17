#pragma once

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "../Engine/Entities/System/System.hpp"
#include "../Component/Transform.h"
#include "../Component/Motion.h"

class DebugSystem : public ecs::System
{
public:
	explicit DebugSystem(const std::shared_ptr<sf::RenderWindow>& window) : window(window) {
		text.setFont(FontStore::get("Carlito.ttf"));
		text.setCharacterSize(16U);
		text.setColor(sf::Color::Black);
	}

	void draw(float dt) override {}

	void update(float dt) override {
		entities->each<Motion, Transform>([&](auto& entity, auto& motion, auto& transform) {
			if (entity.id() == 0U) {
				text.setString(std::to_string(motion.ax) + ", " + std::to_string(motion.ay));
				text.setPosition(5.f, 24.f);
				window->draw(text);
			}
		});
	}

private:
	sf::Text text;
	std::shared_ptr<sf::RenderWindow> window;
};