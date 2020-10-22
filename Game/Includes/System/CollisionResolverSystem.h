#pragma once

#include <Engine/Entities/System/System.hpp>

#include "../Message/Collision.h"
#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

class CollisionResolverSystem final : public ecs::SystemListener<Collision>
{
public:
	void update(float dt) override {}

	void handle(const Collision& message) {

		float dist = distance(message.t1, message.t2);

		// Normal
		float nx = (message.t2.x - message.t1.x) / dist;
		float ny = (message.t2.y - message.t1.y) / dist;

		// Tangent (cross?)
		float tx = -ny;
		float ty = nx;

		// Dot Product Tangent
		float dpt1 = message.m1.velocity.x * tx + message.m1.velocity.y * ty;
		float dpt2 = message.m2.velocity.x * tx + message.m2.velocity.y * ty;

		// Dot Product Normal
		float dpn1 = message.m1.velocity.x * nx + message.m1.velocity.y * ny;
		float dpn2 = message.m2.velocity.x * nx + message.m2.velocity.y * ny;

		// Conservation of momentum in 1D
		float m1 = (dpn1 * (message.b1.mass - message.b2.mass) + 2.0f * message.b2.mass * dpn2) / (message.b1.mass + message.b2.mass);
		float m2 = (dpn2 * (message.b2.mass - message.b1.mass) + 2.0f * message.b1.mass * dpn1) / (message.b1.mass + message.b2.mass);

		// Update ball velocities
		message.m1.velocity.x = tx * dpt1 + nx * m1;
		message.m1.velocity.y = ty * dpt1 + ny * m1;
		message.m2.velocity.x = tx * dpt2 + nx * m2;
		message.m2.velocity.y = ty * dpt2 + ny * m2;

		// Update rotations
		//message.t1.rotation = std::atan2f(message.m1.velocity.y, message.m1.velocity.x) * 57.2958f;
		//message.t2.rotation = std::atan2f(message.m2.velocity.y, message.m2.velocity.x) * 57.2958f;

		//messages->publish<Explosion>(20.f, message.t1.x, message.t1.y);
	}

private:
	float distance(const Transform& t1, const Transform& t2) {
		return std::sqrtf((t1.x - t2.x) * (t1.x - t2.x) + (t1.y - t2.y) * (t1.y - t2.y));
	}
};