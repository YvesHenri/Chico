#pragma once

#include <Engine/Messages/Message.hpp>
#include <Engine/Mathematics.hpp>

struct Weather : public mqs::ManagedMessage<Weather>
{
	explicit Weather(float windStrenght, math::Angle windAngle)
		: ManagedMessage(1U)
		, windStrenght(windStrenght)
		, windAngle(windAngle) {};

	float windStrenght;
	math::Angle windAngle;
};