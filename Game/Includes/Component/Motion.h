#pragma once

#include <Engine/Mathematics.hpp>

struct Motion
{
	explicit Motion(float speed = 64.f) : Motion(speed, speed * 75.f) {}
	explicit Motion(float speed, float thrust) : speed(speed), thrust(thrust) {}

	math::Vector velocity;

	float speed; // Max
	float thrust;
};