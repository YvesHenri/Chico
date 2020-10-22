#pragma once

#include <Engine/Messages/Message.hpp>

struct LoadMessage final : public mqs::ManagedMessage<LoadMessage>
{
	LoadMessage() : mqs::ManagedMessage<LoadMessage>(0U) {}
};