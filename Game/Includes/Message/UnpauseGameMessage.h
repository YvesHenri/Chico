#pragma once

#include "../Engine/Messages/Message.hpp"

struct UnpauseGameMessage final : public mqs::ManagedMessage<UnpauseGameMessage>
{
	UnpauseGameMessage() : mqs::ManagedMessage<UnpauseGameMessage>(0U) {}
};