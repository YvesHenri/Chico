#pragma once

struct Body
{
	explicit Body(float mass) : Body(mass, mass * 32.f) {}
	explicit Body(float mass, float size) : mass(mass), maxMass(mass), radius(size / 2.f) {}

	float mass; // Current value
	float maxMass;
	float radius;

	bool flyable = false;
	bool pushable = false;
	bool collidable = true;
};