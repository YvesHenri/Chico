#pragma once

#include <SFML/Window/Mouse.hpp>
#include <Engine/Entities/System/System.hpp>

#include "../Component/Motion.h"
#include "../Component/Joystick.h"
#include "../Component/Body.h"
#include "../Component/Transform.h"

class JoystickSystem : public ecs::System
{
public:
	explicit JoystickSystem(sf::RenderWindow& window) : window(window) {}

	void update(float time) override {
		entities->each<Joystick, Motion, Body, Transform>([&](auto& entity, auto& joystick, auto& motion, auto& body, auto& transform) {
			auto x = 0.f;
			auto y = 0.f;
			auto input = false;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				x = -1.f;
				input = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				x = 1.f;
				input = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				y = -1.f;
				input = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				y = 1.f;
				input = true;
			}
			
			// Runs 20% faster
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
				x *= 1.2f;
				y *= 1.2f;
			}

			// TODO: Cap max speed
			/*
			if (input) {
				math::Vector direction(x, y);
				math::Vector acceleration = (direction * motion.thrust) / body.mass;
				math::Vector deltaVelocity = (acceleration * time) - motion.velocity;

				motion.velocity += deltaVelocity;
			}
			*/

			auto mouse = sf::Mouse::getPosition(window);
			auto world = window.mapPixelToCoords(mouse);
			auto seek = 0.1f;

			auto worldVector = math::Vector(world.x, world.y);
			auto transformVector = math::Vector(transform.x, transform.y);

			auto desiredVelocity = (worldVector - transformVector).normalize() * 10.f;
			auto steer = desiredVelocity - motion.velocity;

			if (steer.magnitude() > seek) {
				steer.clamp(seek);
			}

			motion.velocity += desiredVelocity;
		});
	}

private:
	sf::RenderWindow& window;
};