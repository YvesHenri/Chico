#ifndef ECS_ENTITY_IMPL
#define ECS_ENTITY_IMPL

#include "Entity.h"
#include "EntityManager.h"

namespace ecs
{
	inline Entity::Entity(unsigned id, EntityManager* manager) {
		this->identifier = id;
		this->manager = manager;
	}

	inline unsigned Entity::id() const {
		return identifier;
	}

	inline unsigned Entity::version() const {
		return manager->current(identifier);
	}

	template <typename Component>
	inline Component& Entity::component() const {
		return manager->component<Component>(identifier);
	}

	template <typename Component, typename... Args>
	inline Component Entity::assign(Args&&... args) const {
		return manager->assign<Component>(identifier, args...);
	}

	template <typename Component, typename... Args>
	inline Component Entity::replace(Args&&... args) const {
		return manager->replace<Component>(identifier, args...);
	}

	template <typename Component, typename... Args>
	inline Component Entity::save(Args&&... args) const {
		return manager->save<Component>(identifier, args...);
	}

	template <typename Component, typename... Components>
	inline void Entity::assign(const Component& component, const Components&... components) const {
		manager->assign(identifier, component, components...);
	}

	template <typename Component, typename... Components>
	inline void Entity::replace(const Component& component, const Components&... components) const {
		manager->replace(identifier, component, components...);
	}

	template <typename Component, typename... Components>
	inline void Entity::save(const Component& component, const Components&... components) const {
		manager->save(identifier, component, components...);
	}

	template <typename Component, typename... Components>
	inline void Entity::reset(const Component* unused, const Components*... unuseds) {
		manager->reset<Component, Components...>(identifier);
	}

	template <typename Component, typename... Components>
	inline void Entity::reset() {
		manager->reset<Component, Components...>(identifier);
	}

	template <typename Component, typename... Components>
	inline void Entity::remove(const Component* unused, const Components*... unuseds) {
		manager->remove<Component, Components...>(identifier);
	}

	template <typename Component, typename... Components>
	inline void Entity::remove() {
		manager->remove<Component, Components...>(identifier);
	}

	template <typename Component, typename... Components>
	inline bool Entity::has() const {
		return manager->has<Component, Components...>(identifier);
	}

	template <typename Component, typename... Components>
	inline bool Entity::has(const Component* unused, const Components*... unuseds) const {
		return manager->has<Component, Components...>(identifier);
	}

	inline void Entity::destroy() {
		manager->destroy(identifier);
	}

	inline bool Entity::valid() const {
		return manager->valid(identifier);
	}

	inline bool Entity::operator==(const Entity& other) const {
		return identifier == other.identifier;
	}

	inline bool Entity::operator!=(const Entity& other) const {
		return identifier != other.identifier;
	}

	inline Entity::operator bool() const {
		return manager->valid(identifier);
	}
}

#endif