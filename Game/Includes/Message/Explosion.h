#pragma once

#include <Engine/Messages/Message.hpp>

struct Explosion : public mqs::ManagedMessage<Explosion>
{
	explicit Explosion(float tons, float x, float y) : ManagedMessage(1U), tons(tons), x(x), y(y) {
		radius = tons * 16.794f; // ~17 pixels per ton
		duration = tons * 0.01785f;
	}

	float x, y;
	float tons;
	float radius;
	float duration;

	float impact(float tx, float ty) const {
		float distance = std::sqrtf((tx - x) * (tx - x) + (ty - y) * (ty - y));
		return (radius - distance) * tons / radius; // Ranges from 0% to 100% explosion radius
	}
};