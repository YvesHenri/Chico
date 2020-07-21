#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Mouse.hpp>

#include "../Engine/Entities/System/System.hpp"
#include "../Component/Transform.h"
#include "../Component/Joystick.h"

class JoystickSystem : public ecs::System
{
public:
	explicit JoystickSystem(const std::shared_ptr<sf::RenderWindow>& window) : window(window) {}

	void draw(float dt) override {}

	void update(float dt) override {
		entities->each<Joystick, Transform, Motion>([&](auto& entity, auto& joystick, auto& transform, auto& motion) {
			switch (joystick.type) {
				case Joystick::Type::Keyboard:
					dispatchKeyboard(dt, motion, transform);
					break;
				case Joystick::Type::Mouse:
					dispatchMouse(transform);
					break;
			}
		});
	}

private:
	void dispatchKeyboard(float dt, Motion& motion, Transform& transform) {
		bool input = false;

		// Horizontal movement
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			input = true;
			motion.vx += motion.speed * dt;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			input = true;
			motion.vx -= motion.speed * dt;
		}

		// Vertical movement
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			input = true;
			motion.vy += motion.speed * dt;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			input = true;
			motion.vy -= motion.speed * dt;
		}

		// Rotation update (if we received any input, to avoid atan2 calculations)
		if (input) {
			transform.rotation = std::atan2f(motion.vy, motion.vx) * 57.2958f;
		}
	}

	void dispatchMouse(Transform& transform) {
		// Update position based on the mouse position
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
			transform.x = float(sf::Mouse::getPosition(*window.get()).x);
			transform.y = float(sf::Mouse::getPosition(*window.get()).y);
		}
	}

private:
	std::shared_ptr<sf::RenderWindow> window;
};