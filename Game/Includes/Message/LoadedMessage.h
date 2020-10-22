#pragma once

#include <Engine/Messages/Message.hpp>

struct LoadedMessage final : public mqs::ManagedMessage<LoadedMessage>
{
	LoadedMessage() : mqs::ManagedMessage<LoadedMessage>(0U) {}
};