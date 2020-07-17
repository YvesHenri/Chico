#pragma once

#include "../Engine/Messages/Message.hpp"

#include "../Component/Transform.h"
#include "../Component/Motion.h"
#include "../Component/Body.h"

struct EntityCollisionMessage final : public mqs::ManagedMessage<EntityCollisionMessage>
{
	explicit EntityCollisionMessage(float distance, Body& b1, Body& b2, Motion& m1, Motion& m2, Transform& t1, Transform& t2) 
		: ManagedMessage(1U)
		, distance(distance)
		, b1(b1), b2(b2)
		, m1(m1), m2(m2)
		, t1(t1), t2(t2) {}

	float distance;
	Body& b1;
	Body& b2;
	Motion& m1;
	Motion& m2;
	Transform& t1;
	Transform& t2;
};