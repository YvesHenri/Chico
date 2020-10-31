#pragma once

#include <cmath>
#include <Engine/Entities/System/System.hpp>
#include <Engine/Entities/Component/Message/EntityAdded.hpp>
#include <Engine/Entities/Component/Message/EntityRemoved.hpp>
//#include <Engine/Structures/QuadTree.hpp>

#include "../QuadTree.hpp"
#include "../Message/Collision.h"
#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

class CollisionSystem final : public ecs::SystemListener<EntityAdded, EntityRemoved>
{
public:
	CollisionSystem(const std::shared_ptr<sf::RenderWindow>& window) : quadTree(0.f, 0.f, 400.f, 300.f), window(window) {}

	void update(float dt) override {
		quadTree.clear();
		entities->each<Motion, Transform, Body>([&](auto& entity, auto& motion, auto& transform, auto& body) {
			//quadTree.addCircle(transform.x, transform.y, body.radius);
		});

		quadTree.draw(*window);
	}

	void handle(const EntityAdded& message) override {
		DEBUG("Entity added %d", message.entityId);

		// Validate whether the entity has the needed components
		if (entities->has<Transform, Body>(message.entityId)) {
			auto body = entities->component<Body>(message.entityId);
			auto transform = entities->component<Transform>(message.entityId);
			//quadTree.addCircle(transform.x, transform.y, body.radius);
		}
	}

	void handle(const EntityRemoved& message) override {
		DEBUG("Entity destroyed %d", message.entityId);
	}

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
	}

private:
	bool collides(const Transform& t1, const Transform& t2, const Body& b1, const Body& b2) {
		return std::fabs((t1.x - t2.x) * (t1.x - t2.x) + (t1.y - t2.y) * (t1.y - t2.y)) <= (b1.radius + b2.radius) * (b1.radius + b2.radius);
	}

	float distance(const Transform& t1, const Transform& t2) {
		return std::sqrtf((t1.x - t2.x) * (t1.x - t2.x) + (t1.y - t2.y) * (t1.y - t2.y));
	}

private:
	QuadTree quadTree;
	std::shared_ptr<sf::RenderWindow> window;
};