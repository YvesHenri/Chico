#pragma once

#include <Engine/Messages/Message.hpp>

struct LoadingMessage final : public mqs::ManagedMessage<LoadingMessage>
{
	LoadingMessage(int progress) : mqs::ManagedMessage<LoadingMessage>(0U), progress(progress) {}

	const int progress;
};