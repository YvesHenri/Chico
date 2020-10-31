#pragma once

#include "../../../Messages/Message.hpp"

struct EntityRemoved final : public mqs::ManagedMessage<EntityRemoved>
{
	explicit EntityRemoved(unsigned entityId) : mqs::ManagedMessage<EntityRemoved>(0U), entityId(entityId) {}

	unsigned entityId;
};