#pragma once

struct Body
{
	explicit Body(float weight) : weight(weight), radius(weight * 0.7f) {}

	float weight;
	float radius;

	bool flyable = true;
	bool pushable = false;
	bool collidable = true;
};