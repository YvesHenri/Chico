#pragma once

#include <random>

#include <SFML/Graphics/RenderWindow.hpp>
#include <Engine/Entities/System/System.hpp>

class WeatherSystem final : public ecs::System
{
public:
	WeatherSystem() : forceRandom(0.f, 100.f), angleRandom(0.f, 360.f) {}

	void update(float delta) override {
		accumulator += delta;

		if (accumulator >= timeToChange) {
			messages->publish<Weather>(forceRandom(randomEngine), math::Angle::degrees(angleRandom(randomEngine)));
			accumulator = 0.f;
		}		
	}

private:
	float accumulator = 0.f;
	float timeToChange = 5.f;
	std::default_random_engine randomEngine;
	std::uniform_real_distribution<float> forceRandom;
	std::uniform_real_distribution<float> angleRandom;
};