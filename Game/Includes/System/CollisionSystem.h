#pragma once

#include <Engine/Data/QuadTree/QuadTree.hpp>
#include <Engine/Entities/System/System.hpp>
#include <Engine/Entities/Component/Message/EntityAdded.hpp>
#include <Engine/Entities/Component/Message/EntityRemoved.hpp>

#include "../Message/Collision.h"
#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

class CollisionSystem final : public ecs::SystemListener<EntityAdded, EntityRemoved>
{
public:
	explicit CollisionSystem(sf::RenderWindow& window) : quadTree(0.f, 0.f, window.getView().getSize().x, window.getView().getSize().y), window(window) {}

	void update(float dt) override {
		std::vector<std::shared_ptr<qdt::CircleObject>> candidates;

		// Viewport
		auto vl = window.getView().getCenter().x - window.getView().getSize().x / 2.f;
		auto vt = window.getView().getCenter().y - window.getView().getSize().y / 2.f;
		auto vr = vl + window.getView().getSize().x;
		auto vb = vt + window.getView().getSize().y;

		// Reset tree
		quadTree.clear();

		// Rebuild tree
		entities->each<Motion, Transform, Body>([&](auto& entity, auto& motion, auto& transform, auto& body) {
			quadTree.addCircle(entity.id(), transform.x, transform.y, body.radius);
		});

		// Fetch objects
		quadTree.queryCircles(vl, vt, vr, vb, candidates);

		// Dispatch collisions
		for (auto& left : candidates) {
			for (auto& right : candidates) {
				if (left->id != right->id) {
					if (left->intersects(right->x, right->y, right->radius)) {
						
						auto& t1 = entities->component<Transform>(left->id);
						auto& t2 = entities->component<Transform>(right->id);

						float distance = std::sqrtf((left->x - right->x) * (left->x - right->x) + (left->y - right->y) * (left->y - right->y));						
						float displacement = 0.5f * (distance - left->radius - right->radius); // 0.5 means half displacement for each circle

						// Displace balls away (normalize direction by distance)
						t1.x -= displacement * (t1.x - t2.x) / distance;
						t1.y -= displacement * (t1.y - t2.y) / distance;
						t2.x += displacement * (t1.x - t2.x) / distance;
						t2.y += displacement * (t1.y - t2.y) / distance;
					}
				}
			}
		}
	}

	void handle(const EntityAdded& message) override {
		DEBUG("Entity added %d", message.entityId);

		// Validate whether the entity has the needed components
		if (entities->has<Transform, Body>(message.entityId)) {
			auto body = entities->component<Body>(message.entityId);
			auto transform = entities->component<Transform>(message.entityId);
			quadTree.addCircle(message.entityId, transform.x, transform.y, body.radius);
		}
	}

	void handle(const EntityRemoved& message) override {
		DEBUG("Entity destroyed %d", message.entityId);
	}

	void handle(const Collision& message) {
		float dist = std::sqrtf((message.t1.x - message.t2.x) * (message.t1.x - message.t2.x) + (message.t1.y - message.t2.y) * (message.t1.y - message.t2.y));

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
	}

private:
	qdt::QuadTree quadTree;
	sf::RenderWindow& window;
};