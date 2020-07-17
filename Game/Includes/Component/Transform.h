#pragma once

struct Transform
{
	explicit Transform(float x, float y, float rotation = 0.f) : x(x), y(y), rotation(rotation) {}

	float x, y;
	float rotation = 0.f; // Degrees
};