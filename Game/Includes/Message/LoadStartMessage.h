#pragma once

#include "../Engine/Messages/Message.hpp"

struct LoadStartMessage final : public mqs::ManagedMessage<LoadStartMessage>
{
	LoadStartMessage() : mqs::ManagedMessage<LoadStartMessage>(0U) {}
};