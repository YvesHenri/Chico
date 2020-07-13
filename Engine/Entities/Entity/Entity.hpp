#ifndef ECS_ENTITY_IMPL
#define ECS_ENTITY_IMPL

#include "EntityManager.hpp"

namespace ecs
{
	Entity::Entity(Entity::Id id, EntityManager* manager) {
		this->identifier = id;
		this->manager = manager;
	}

	Entity::Id Entity::id() const {
		return identifier;
	}

	Entity::Version Entity::version() const {
		return manager->current(identifier);
	}

	template <typename Component>
	Component& Entity::component() const {
		return manager->component<Component>(identifier);
	}

	template <typename Component, typename... Args>
	Component Entity::assign(Args&&... args) const {
		return manager->assign<Component>(identifier, args...);
	}

	template <typename Component, typename... Args>
	Component Entity::replace(Args&&... args) const {
		return manager->replace<Component>(identifier, args...);
	}

	template <typename Component, typename... Args>
	Component Entity::save(Args&&... args) const {
		return manager->save<Component>(identifier, args...);
	}

	template <typename Component, typename... Components>
	void Entity::assign(const Component& component, const Components&... components) const {
		manager->assign(identifier, component, components...);
	}

	template <typename Component, typename... Components>
	void Entity::replace(const Component& component, const Components&... components) const {
		manager->replace(identifier, component, components...);
	}

	template <typename Component, typename... Components>
	void Entity::save(const Component& component, const Components&... components) const {
		manager->save(identifier, component, components...);
	}

	template <typename Component, typename... Components>
	void Entity::reset(const Component* unused, const Components*... unuseds) {
		manager->reset<Component, Components...>(identifier);
	}

	template <typename Component, typename... Components>
	void Entity::reset() {
		manager->reset<Component, Components...>(identifier);
	}

	template <typename Component, typename... Components>
	void Entity::remove(const Component* unused, const Components*... unuseds) {
		manager->remove<Component, Components...>(identifier);
	}

	template <typename Component, typename... Components>
	void Entity::remove() {
		manager->remove<Component, Components...>(identifier);
	}

	template <typename Component, typename... Components>
	bool Entity::has() const {
		return manager->has<Component, Components...>(identifier);
	}

	template <typename Component, typename... Components>
	bool Entity::has(const Component* unused, const Components*... unuseds) const {
		return manager->has<Component, Components...>(identifier);
	}

	void Entity::destroy() {
		manager->destroy(identifier);
	}

	bool Entity::valid() const {
		return manager->valid(identifier);
	}

	bool Entity::operator==(const Entity& other) const {
		return identifier == other.identifier;
	}

	bool Entity::operator!=(const Entity& other) const {
		return identifier != other.identifier;
	}

	bool Entity::operator<(const Entity& other) const {
		return identifier < other.identifier;
	}

	Entity::operator bool() const {
		return manager->valid(identifier);
	}
}

#endif