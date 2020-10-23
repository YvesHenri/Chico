#pragma once

#include <cmath>
#include <Engine/Entities/System/System.hpp>

#include "../Message/Weather.h"
#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

class KinematicSystem : public ecs::SystemListener<Weather>
{
public:
	explicit KinematicSystem(const std::shared_ptr<sf::RenderWindow>& window) : window(window) {}

	void update(float time) override {
		entities->each<Transform, Motion, Body>([&](auto& e, Transform& transform, Motion& motion, Body& body) {
			auto terrainFriction = 5.123f; // 1 is no friction
			auto shoesTraction = 0.f; // 0 is no traction (no shoes)

			auto friction = std::max(1.f, terrainFriction) + shoesTraction;
			auto windForce = math::Vector::polar(windStrenght, math::Angle::radians(windAngle));
			auto windAcceleration = windForce / body.mass; // Wind acceleration upon the entity
			
			// Apply wind and friction
			motion.velocity += windAcceleration * time;
			motion.velocity *= std::powf(1.f / friction, time);

			auto displacement = motion.velocity * time;

			transform.x += displacement.x;
			transform.y -= displacement.y; // SFML uses reverse Y (-)
		});
	}

	void handle(const Weather& weather) override {
		TRACE("Weather changed! Force: %.2f, Angle: %.2f", weather.windForce, weather.windAngle.degrees());
		windStrenght = weather.windForce;
		windAngle = weather.windAngle.radians();
	}

private:
	float windStrenght = 0.f;
	float windAngle = 0.f;
	std::shared_ptr<sf::RenderWindow> window;
};