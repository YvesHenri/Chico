#pragma once

#include <SFML/Window/Mouse.hpp>

#include "../Engine/Entities/System/System.hpp"
#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/MouseController.h"

class MouseControllerSystem : public ecs::System
{
public:
	explicit MouseControllerSystem(sf::RenderWindow& window) : window(window) {}

	void draw(float delta) override {}

	void update(float delta) override {
		entities->each<MouseController, Transform>([&](auto& e, auto& controller, auto& transform) {
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
				transform.x = float(sf::Mouse::getPosition(window).x);
				transform.y = float(sf::Mouse::getPosition(window).y);
			}
		});
	}

private:
	sf::RenderWindow& window;
};