#pragma once

struct Transform
{
	explicit Transform(float x, float y, float rotation = 0.f) : x(x), y(y), z(0.f) {}

	float x, y, z = 0.f;

	float dx, dy; // Destination (target)
	float tx, ty; // Tile coordinates
};