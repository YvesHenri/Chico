#pragma once

#include "../Engine/Entities/System/System.hpp"
#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/KeyboardController.h"

class KeyboardControllerSystem : public ecs::System
{
public:
	void draw(float dt) override {}

	void update(float dt) override {
		// Change the velocity vector according to the given direction
		entities->each<KeyboardController, Motion, Transform>([dt](auto& entity, auto& controller, auto& motion, auto& transform) {
			
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
		});
	}
};