#pragma once

#include "../Engine/Messages/Message.hpp"

struct GravityChangedMessage final : public mqs::ManagedMessage<GravityChangedMessage>
{
	explicit GravityChangedMessage(float gravity) : ManagedMessage(1U), gravity(gravity) {}

	const float gravity;
};