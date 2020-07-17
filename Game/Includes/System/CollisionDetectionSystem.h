#pragma once

#include <cmath>

#include "../Engine/Entities/System/System.hpp"

#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

class CollisionDetectionSystem final : public ecs::System
{
public:
	void draw(float dt) override {}

	void update(float dt) override {
		entities->each<Motion, Transform, Body>([&](auto& e1, auto& m1, auto& t1, auto& b1) {
			entities->each<Motion, Transform, Body>([&](auto& e2, auto& m2, auto& t2, auto& b2) {
				if (e1.id() != e2.id()) {
					if (collides(t1, b1, t2, b2)) {
						float dist = distance(t1, t2);
						float displacement = 0.5f * (dist - b1.radius - b2.radius);

						// Displace balls away
						t1.x -= displacement * (t1.x - t2.x) / dist;
						t1.y -= displacement * (t1.y - t2.y) / dist;
						t2.x += displacement * (t1.x - t2.x) / dist;
						t2.y += displacement * (t1.y - t2.y) / dist;

						messages->publish<EntityCollisionMessage>(dist, b1, b2, m1, m2, t1, t2);
					}
				}
			});
		});
	}

private:
	bool collides(const Transform& t1, const Body& b1, const Transform& t2, const Body& b2) {
		return std::fabs((t1.x - t2.x) * (t1.x - t2.x) + (t1.y - t2.y) * (t1.y - t2.y)) <= (b1.radius + b2.radius) * (b1.radius + b2.radius);
	}

	float distance(const Transform& t1, const Transform& t2) {
		return std::sqrtf((t1.x - t2.x) * (t1.x - t2.x) + (t1.y - t2.y) * (t1.y - t2.y));
	}
};