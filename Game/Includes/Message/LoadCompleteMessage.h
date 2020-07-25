#pragma once

#include <Engine/Messages/Message.hpp>

struct LoadCompleteMessage final : public mqs::ManagedMessage<LoadCompleteMessage>
{
	LoadCompleteMessage() : mqs::ManagedMessage<LoadCompleteMessage>(0U) {}
};