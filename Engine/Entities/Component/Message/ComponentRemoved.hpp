#pragma once

#include "../../../Messages/Message.hpp"

template <typename Component>
struct ComponentRemoved final : public mqs::ManagedMessage<ComponentAdded<Component>>
{
	explicit ComponentRemoved(const Component& component, unsigned entity) : mqs::ManagedMessage<ComponentAdded<Component>>(0U), component(component), entity(entity){}

	const unsigned entity;
	const Component& component;
};