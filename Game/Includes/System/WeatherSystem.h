#pragma once

#include <random>

#include <SFML/Graphics/RenderWindow.hpp>
#include <Engine/Entities/System/System.hpp>

class WeatherSystem final : public ecs::System
{
public:
	WeatherSystem() : randomStrenght(0.f, 100.f), randomRadians(0.f, 6.28f) {}

	void update(float delta) override {
		accumulator += delta;

		if (accumulator >= timeToChange) {
			auto strenght = randomStrenght(randomEngine);
			auto angle = randomRadians(randomEngine);
			messages->publish<Weather>(strenght, math::Angle::radians(angle));
			accumulator = 0.f;
		}
	}

private:
	float accumulator = 0.f;
	float timeToChange = 5.f;
	std::default_random_engine randomEngine;
	std::uniform_real_distribution<float> randomStrenght;
	std::uniform_real_distribution<float> randomRadians;
};