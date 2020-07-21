#pragma once

#include "../Engine/Messages/Message.hpp"

struct Explosion : public mqs::ManagedMessage<Explosion>
{
	explicit Explosion(float magnitude, float ex, float ey) : ManagedMessage(1U), magnitude(magnitude), ex(ex), ey(ey) {
		// Set values proportionally
		duration = magnitude * 0.0215f;
	}

	float ex, ey; // Epicenter	
	float duration;
	float magnitude;

	float impact() const {
		return impact(ex, ey);
	}

	float impact(float tx, float ty) const {
		float amplifier = 1.072f;
		float distance = std::sqrtf((tx - ex) * (tx - ex) + (ty - ey) * (ty - ey));
		return distance >= magnitude ? 0.f : (1.f - distance / magnitude) * magnitude * amplifier;
	}
};