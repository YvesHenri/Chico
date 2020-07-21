#pragma once

#include <cmath>

#include "../Engine/Entities/System/System.hpp"
#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

class KinematicSystem : public ecs::System
{
public:
	void draw(float delta) override {}
	void update(float delta) override {
		entities->each<Transform, Motion, Body>([&](auto& e, auto& transform, auto& motion, auto& body) {
			// Update acceleration
			motion.ax = -motion.vx * body.weight * gravity;
			motion.ay = -motion.vy * body.weight * gravity;

			// Update velocity
			motion.vx += motion.ax * delta;
			motion.vy += motion.ay * delta;

			// Update position
			transform.x -= motion.vx * delta;
			transform.y -= motion.vy * delta;

			if (std::fabs(motion.vx * motion.vx + motion.vy * motion.vy) < 0.1f) {
				motion.vx = 0.f;
				motion.vy = 0.f;
			}
		});
	}

private:
	float gravity = 0.0095f; // The greater, the slower
};