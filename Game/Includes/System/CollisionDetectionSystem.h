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
					if (collides(t1, t2, b1, b2)) {
						float distanc = distance(t1, t2);
						float displacement = 0.5f * (distanc - b1.radius - b2.radius); // 0.5 means half displacement for each circle

						// Displace balls away
						t1.x -= displacement * (t1.x - t2.x) / distanc; // Normalize direction by distance
						t1.y -= displacement * (t1.y - t2.y) / distanc;
						t2.x += displacement * (t1.x - t2.x) / distanc;
						t2.y += displacement * (t1.y - t2.y) / distanc;

						messages->publish<Collision>(b1, b2, m1, m2, t1, t2);
					}
				}
			});
		});
	}

private:
	bool collides(const Transform& t1, const Transform& t2, const Body& b1, const Body& b2) {
		return std::fabs((t1.x - t2.x) * (t1.x - t2.x) + (t1.y - t2.y) * (t1.y - t2.y)) <= (b1.radius + b2.radius) * (b1.radius + b2.radius);
	}

	float distance(const Transform& t1, const Transform& t2) {
		return std::sqrtf((t1.x - t2.x) * (t1.x - t2.x) + (t1.y - t2.y) * (t1.y - t2.y));
	}
};