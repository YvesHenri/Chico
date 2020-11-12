#pragma once

#include <Engine/Entities/System/System.hpp>

#include "../Message/Weather.h"
#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

class KinematicSystem : public ecs::SystemListener<Weather>
{
public:
	void update(float time) override {
		entities->each<Transform, Motion, Body>([&](auto& e, Transform& transform, Motion& motion, Body& body) {
			auto terrainFriction = 20.f; // 1 is no friction
			auto shoesTraction = 0.f; // 0 is no traction

			auto friction = std::max(1.f, terrainFriction) + shoesTraction;
			auto windAcceleration = windForce / body.mass; // Wind acceleration upon the entity
			
			// Apply wind and friction
			motion.velocity += windAcceleration * time;
			motion.velocity *= std::powf(1.f / friction, time);

			auto displacement = motion.velocity * time;

			transform.x += displacement.x;
			transform.y += displacement.y;
		});
	}

	void handle(const Weather& weather) override {
		TRACE("Weather changed! Force: %.2f, Angle: %.2f", weather.windStrenght, weather.windAngle.degrees());
		windForce = math::Vector::polar(weather.windStrenght, weather.windAngle);
	}

private:
	float windStrenght = 0.f;
	float windAngle = 0.f;
	math::Vector windForce;
};