#pragma once

#include "../Engine/Entities/System/System.hpp"

#include "../Message/EntityCollisionMessage.h"
#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

class CollisionResolverSystem final : public ecs::SystemListener<EntityCollisionMessage>
{
public:
	void draw(float dt) override {}

	void update(float dt) override {}

	void handle(const EntityCollisionMessage& message) {
		// Normal
		float nx = (message.t2.x - message.t1.x) / message.distance;
		float ny = (message.t2.y - message.t1.y) / message.distance;

		// Tangent
		float tx = -ny;
		float ty = nx;

		// Dot Product Tangent
		float dpTan1 = message.m1.vx * tx + message.m1.vy * ty;
		float dpTan2 = message.m2.vx * tx + message.m2.vy * ty;

		// Dot Product Normal
		float dpNorm1 = message.m1.vx * nx + message.m1.vy * ny;
		float dpNorm2 = message.m2.vx * nx + message.m2.vy * ny;

		// Conservation of momentum in 1D
		float m1 = (dpNorm1 * (message.b1.weight - message.b2.weight) + 2.0f * message.b2.weight * dpNorm2) / (message.b1.weight + message.b2.weight);
		float m2 = (dpNorm2 * (message.b2.weight - message.b1.weight) + 2.0f * message.b1.weight * dpNorm1) / (message.b1.weight + message.b2.weight);

		// Update ball velocities
		message.m1.vx = tx * dpTan1 + nx * m1;
		message.m1.vy = ty * dpTan1 + ny * m1;
		message.m2.vx = tx * dpTan2 + nx * m2;
		message.m2.vy = ty * dpTan2 + ny * m2;
	}
};