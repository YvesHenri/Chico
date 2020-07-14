#pragma once

#include "../Engine/Messages/Message.hpp"

struct LoadProgressMessage final : public mqs::ManagedMessage<LoadProgressMessage>
{
	LoadProgressMessage(int progress) : mqs::ManagedMessage<LoadProgressMessage>(0U), progress(progress) {
	}

	const int progress;
};