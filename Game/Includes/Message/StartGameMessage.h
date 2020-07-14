#pragma once

#include "../Engine/Messages/Message.hpp"

struct StartGameMessage final : public mqs::ManagedMessage<StartGameMessage>
{
	StartGameMessage() : mqs::ManagedMessage<StartGameMessage>(0U) {}
};