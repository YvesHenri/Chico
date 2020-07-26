#pragma once

#include "../../../Messages/Message.hpp"

template <typename Component>
struct ComponentAdded final : public mqs::ManagedMessage<ComponentAdded<Component>>
{
	explicit ComponentAdded(const Component& component, unsigned entity) : mqs::ManagedMessage<ComponentAdded<Component>>(0U), component(component), entity(entity){}

	const unsigned entity;
	const Component& component;
};