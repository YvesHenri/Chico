#pragma once

#include "../../../Messages/Message.hpp"

struct EntityAdded final : public mqs::ManagedMessage<EntityAdded>
{
	explicit EntityAdded(unsigned id) : mqs::ManagedMessage<EntityAdded>(0U), entityId(id) {}

	unsigned entityId;
};