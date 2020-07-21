#pragma once

#include "../Engine/Entities/System/System.hpp"

#include "../Message/Collision.h"
#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

class CollisionResolverSystem final : public ecs::SystemListener<Collision>
{
public:
	void draw(float dt) override {}

	void update(float dt) override {}

	void handle(const Collision& message) {

		float dist = distance(message.t1, message.t2);

		// Normal
		float nx = (message.t2.x - message.t1.x) / dist;
		float ny = (message.t2.y - message.t1.y) / dist;

		// Tangent
		float tx = -ny;
		float ty = nx;

		// Dot Product Tangent
		float dpt1 = message.m1.vx * tx + message.m1.vy * ty;
		float dpt2 = message.m2.vx * tx + message.m2.vy * ty;

		// Dot Product Normal
		float dpn1 = message.m1.vx * nx + message.m1.vy * ny;
		float dpn2 = message.m2.vx * nx + message.m2.vy * ny;

		// Conservation of momentum in 1D
		float m1 = (dpn1 * (message.b1.weight - message.b2.weight) + 2.0f * message.b2.weight * dpn2) / (message.b1.weight + message.b2.weight);
		float m2 = (dpn2 * (message.b2.weight - message.b1.weight) + 2.0f * message.b1.weight * dpn1) / (message.b1.weight + message.b2.weight);

		// Update ball velocities
		message.m1.vx = tx * dpt1 + nx * m1;
		message.m1.vy = ty * dpt1 + ny * m1;
		message.m2.vx = tx * dpt2 + nx * m2;
		message.m2.vy = ty * dpt2 + ny * m2;

		// Update rotations
		message.t1.rotation = std::atan2f(message.m1.vy, message.m1.vx) * 57.2958f;
		message.t2.rotation = std::atan2f(message.m2.vy, message.m2.vx) * 57.2958f;

		messages->publish<Explosion>(20.f, message.t1.x, message.t1.y);
	}

private:
	float distance(const Transform& t1, const Transform& t2) {
		return std::sqrtf((t1.x - t2.x) * (t1.x - t2.x) + (t1.y - t2.y) * (t1.y - t2.y));
	}
};