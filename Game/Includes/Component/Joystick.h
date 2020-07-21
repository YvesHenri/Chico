#pragma once

struct Joystick
{
	enum class Type {
		Keyboard,
		Mouse
	};

	Type type = Type::Keyboard;
};