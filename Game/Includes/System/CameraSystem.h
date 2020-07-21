#pragma once

#include <random>
#include <queue>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../Engine/Entities/System/System.hpp"
#include "../Message/Explosion.h"
#include "../Message/CameraChanged.h"
#include "../Component/Camera.h"
#include "../Component/Transform.h"

class CameraSystem final : public ecs::SystemListener<CameraChanged, Explosion>
{
public:
	explicit CameraSystem(const std::shared_ptr<sf::RenderWindow>& window) : window(window), random(-1.f, 1.f) {}

	void draw(float dt) {}

	void update(float dt) {
		entities->each<Camera, Transform>([&](auto& entity, auto& camera, auto& transform) {
			// Udate target location
			tx = transform.x;
			ty = transform.y;

			// Lerp positions over time for smooth chasing
			elapsedChaseTime = std::min(elapsedChaseTime + chaseSmoothness, 1.f); // "Clamp"
			cx = (1.f - elapsedChaseTime) * cx + elapsedChaseTime * tx;
			cy = (1.f - elapsedChaseTime) * cy + elapsedChaseTime * ty;

			auto view = window->getView();

			// Add shake if there's one
			if (elapsedShakeTime < activeShakeDuration) {
				elapsedShakeTime += dt;

				auto decay = std::max((activeShakeDuration - elapsedShakeTime) / activeShakeDuration, 0.f);
				auto rotation = 5.f * decay * random(randomEngine); // TODO Find max rotation based on magnitude (current = 5 degrees)

				INFO("Elap.: %.3f of %.3f, Decay: %f, Mag.: %f, Rot.: %.2f", elapsedShakeTime, activeShakeDuration, decay, activeShakeMagnitude, rotation);
				
				cx += decay * random(randomEngine) * activeShakeMagnitude;
				cy += decay * random(randomEngine) * activeShakeMagnitude;

				view.setRotation(rotation); // Will automatically reset when decay reaches 0
			}

			// Apply
			view.setCenter(cx, cy);
			window->setView(view);
		});
	}

	void handle(const Explosion& explosion) {
		float magnitude = explosion.impact(cx, cy);

		// Check whether the explosion causes any impact on where we are
		if (magnitude > 0.f) {
			ERR("Explosion! Mag: %.3f, Dur.: %.3f", magnitude, explosion.duration);
			activeShakeMagnitude = magnitude; // Perhaps LERP previous + current?

			// Merge shakes (TODO merge it better)
			if (elapsedShakeTime < activeShakeDuration) {
				activeShakeDuration += explosion.duration;
			}
			else {
				activeShakeDuration = explosion.duration;
				elapsedShakeTime = 0.f;
			}
		}
	}

	void handle(const CameraChanged& message) {
		elapsedChaseTime = 0.f;
	}

	/*
	void handle(const ChangeCamera& message) {
		std::vector<ecs::Entity::Id> ids;
		
		// Remember current position
		auto cx = 0.f;
		auto cy = 0.f;

		elapsedChaseTime = 0.f;

		// Copy entities with a camera (should result in a vector with 1 element)
		entities->each<Camera>([&](auto& entity, auto& camera) {
			ids.push_back(entity.id());
			cx = camera.cx;
			cy = camera.cy;
		});

		// Removes the camera from whoever have it
		for (auto& id : ids) {
			entities->remove<Camera>(id);
		}

		// Assign a brand new camera to the target
		entities->assign<Camera>(message.target, cx, cy);
	}
	

	void handle(const ZoomCamera& zoom) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal)) {
			modifiers.emplace(zoom.factor);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash)) {
			modifiers.emplace(zoom.factor);
		}
	}
	*/

private:
	float cx = 0.f, cy = 0.f;
	float tx = 0.f, ty = 0.f;
	float activeShakeMagnitude = 0.f; // If any
	float activeShakeDuration = 0.f; // If any
	float elapsedShakeTime = 0.f;
	float elapsedChaseTime = 0.f;
	float chaseSmoothness = 0.0095f;
	std::default_random_engine randomEngine;
	std::uniform_real_distribution<float> random;
	std::shared_ptr<sf::RenderWindow> window;
};