#pragma once

#include <random>
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

	void draw(float delta) {}

	void update(float delta) {
		entities->each<Camera, Transform>([&](auto& entity, auto& camera, auto& transform) {
			auto view = window->getView();
			
			// Lerp positions for smooth chasing
			elapsedChaseTime = std::min(elapsedChaseTime + smoothChaseFactor, 1.f);
			x = (1.f - elapsedChaseTime) * x + elapsedChaseTime * transform.x;
			y = (1.f - elapsedChaseTime) * y + elapsedChaseTime * transform.y;

			// Offset positions when shaking
			if (elapsedShakeTime < currentShakeDuration) {
				auto decay = std::max((currentShakeDuration - (elapsedShakeTime + delta)) / currentShakeDuration, 0.f);
				auto rotation = decay * random(randomEngine) * currentShakeStrenght * 0.35f; // 0.35 degrees per explosion ton

				TRACE("Elap.: %.3f of %.3f, Decay: %f, Mag.: %f, Rot.: %.2f", elapsedShakeTime, currentShakeDuration, decay, currentShakeStrenght, rotation);
				
				elapsedShakeTime += delta;
				x += decay * random(randomEngine) * currentShakeStrenght;
				y += decay * random(randomEngine) * currentShakeStrenght;

				view.setRotation(rotation); // Resets automatically (when decay reaches 0)
			}

			// Apply
			view.setCenter(x, y);
			window->setView(view);
		});
	}

	void handle(const Explosion& explosion) {
		float impact = explosion.impact(x, y);

		// Check whether the explosion causes any impact on where we are
		if (impact > 0.f) {
			ERR("Explosion! Impact: %.3f, Duration: %.3f. Elapsed: %.3f, Total: %.3f", impact, explosion.duration, elapsedShakeTime, currentShakeDuration);
			
			// Merge explosions if there's one active
			if (elapsedShakeTime < currentShakeDuration) {
				currentShakeStrenght = ((currentShakeDuration - elapsedShakeTime) / currentShakeDuration * currentShakeStrenght) + impact;
				currentShakeDuration += explosion.duration;
			}
			else {
				currentShakeStrenght = impact;
				currentShakeDuration = explosion.duration;
				elapsedShakeTime = 0.f;
			}

			ERR("Explosion! Final impact: %.3f, Final duration: %.3f", currentShakeStrenght, currentShakeDuration);
		}
	}

	void handle(const CameraChanged& message) {
		elapsedChaseTime = 0.f;
	}

private:
	float x = 0.f, y = 0.f;
	float smoothChaseFactor = 0.0095f;
	float elapsedChaseTime = 0.f;
	float elapsedShakeTime = 0.f; // If any
	float currentShakeStrenght = 0.f; // If any
	float currentShakeDuration = 0.f; // If any
	std::default_random_engine randomEngine;
	std::uniform_real_distribution<float> random;
	std::shared_ptr<sf::RenderWindow> window;
};