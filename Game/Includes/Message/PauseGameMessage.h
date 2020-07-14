#pragma once

#include "../Engine/Messages/Message.hpp"

struct PauseGameMessage final : public mqs::ManagedMessage<PauseGameMessage>
{
	PauseGameMessage() : mqs::ManagedMessage<PauseGameMessage>(0U) {}
};