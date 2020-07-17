#pragma once

struct Motion
{
	explicit Motion(float speed = 300.f) : speed(speed) {}

	float speed;
	float vx = 0.f, vy = 0.f; // Velocity
	float ax = 0.f, ay = 0.f; // Acceleration

	
	float currentSpeed = 0.f;
	float maximumSpeed = 200.f;
	float acceleration = 200.f; // How fast will object reach a maximum speed
	float deceleration = 100.f; // How fast will object reach a speed of 0
};