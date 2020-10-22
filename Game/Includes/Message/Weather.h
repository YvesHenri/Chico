#pragma once

#include <Engine/Messages/Message.hpp>
#include <Engine/Mathematics.hpp>

struct Weather : public mqs::ManagedMessage<Weather>
{
	explicit Weather(float windForce, math::Angle windAngle) 
		: ManagedMessage(1U)
		, windForce(windForce)
		, windAngle(windAngle) {};

	float windForce;
	math::Angle windAngle;
};