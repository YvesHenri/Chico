#pragma once

#include <Engine/Messages/Message.hpp>

#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

struct Collision final : public mqs::ManagedMessage<Collision>
{
	explicit Collision(Body& b1, Body& b2, Motion& m1, Motion& m2, Transform& t1, Transform& t2) 
		: ManagedMessage(1U)
		, b1(b1), b2(b2)
		, m1(m1), m2(m2)
		, t1(t1), t2(t2) {}

	Body& b1;
	Body& b2;
	Motion& m1;
	Motion& m2;
	Transform& t1;
	Transform& t2;
};