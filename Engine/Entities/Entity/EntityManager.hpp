#ifndef ECS_ENTITY_MANAGER_IMPL
#define ECS_ENTITY_MANAGER_IMPL

#include <cassert>
#include <stdexcept>

#include "../../Family.hpp"
#include "../Entity/EntityManager.h"
#include "../Component/ComponentView.hpp"

namespace ecs
{
	Entity EntityManager::create() {
		Entity::Id id = 0U;

		if (available) {
			auto entity = next;
			auto version = entities[entity] & ~Entity::ID_MASK;

			id = entity | version;
			next = entities[entity] & Entity::ID_MASK;
			entities[entity] = id;
			available--;
		}
		else {
			id = entities.size();
			assert(id < Entity::ID_MASK);
			entities.push_back(id);
		}

		return Entity(id, this);
	}

	template <typename Component, typename... Args>
	Entity EntityManager::create(Args&&... componentArgs) {
		auto entity = create();
		entity.assign<Component>(componentArgs...);
		return entity;
	}

	template <typename Component, typename... Components>
	Entity EntityManager::create(const Component& component, const Components&... components) {
		auto entity = create();
		entity.assign(component, components...);
		return entity;
	}

	template <typename Component, typename... Args>
	Component EntityManager::assign(Entity::Id entityId, Args&&... componentArgs) {
		validate(entityId);
		auto component = Component(std::forward<Args>(componentArgs)...);
		safeCollection<Component>().add(entityId, component);
		return component;
	}

	template <typename Component, typename... Args>
	Component EntityManager::replace(Entity::Id entityId, Args&&... componentArgs) {
		validate(entityId);
		auto component = Component(std::forward<Args>(componentArgs)...);
		unsafeCollection<Component>().replace(entityId, component);
		return component;
	}

	template <typename Component, typename... Args>
	Component EntityManager::save(Entity::Id entityId, Args&&... componentArgs) {
		validate(entityId);
		auto component = Component(std::forward<Args>(componentArgs)...);
		safeCollection<Component>().save(entityId, component);
		return component;
	}

	template <typename Component>
	Component& EntityManager::component(Entity::Id entityId) {
		validate(entityId);
		return unsafeCollection<Component>().get(entityId);
	}

	template <typename Component, typename... Components>
	void EntityManager::assign(Entity::Id entityId, const Component& component, const Components&... components) {
		validate(entityId);
		safeCollection<Component>().add(entityId, component);
		assign(entityId, components...);
	}

	template <typename Component, typename... Components>
	void EntityManager::replace(Entity::Id entityId, const Component& component, const Components&... components) {
		validate(entityId);
		unsafeCollection<Component>().replace(entityId, component);
		replace(entityId, components...);
	}

	template <typename Component, typename... Components>
	void EntityManager::save(Entity::Id entityId, const Component& component, const Components&... components) {
		validate(entityId);
		safeCollection<Component>().save(entityId, component);
		save(entityId, components...);
	}

	template <typename Component, typename... Components>
	void EntityManager::reset(Entity::Id entityId) {
		validate(entityId);

		if (managed<Component>()) {
			unsafeCollection<Component>().reset(entityId);
		}

		reset<Components...>(entityId);
	}

	template <typename Component, typename... Components>
	void EntityManager::reset(Entity::Id entityId, const Component&, const Components&...) {
		reset<Component, Components...>(entityId);
	}

	template <typename Component, typename... Components>
	void EntityManager::reset() {
		if (managed<Component>()) {
			auto& collection = unsafeCollection<Component>();

			each([&collection](Entity& entity) {
				collection.reset(entity.id());
			});
		}

		reset<Components...>();
	}

	template <typename Component, typename... Components>
	void EntityManager::remove(Entity::Id entityId) {
		validate(entityId);
		unsafeCollection<Component>().remove(entityId);
		remove<Components...>(entityId);
	}

	template <typename Component, typename... Components>
	void EntityManager::remove(Entity::Id entityId, const Component&, const Components&...) {
		remove<Component, Components...>(entityId);
	}

	template <typename Component, typename... Components>
	bool EntityManager::has(Entity::Id entityId) {
		validate(entityId);
		return managed<Component>() && unsafeCollection<Component>().contains(entityId) && has<Components...>(entityId);
	}

	template <typename Component, typename... Components, typename Lambda>
	void EntityManager::each(Lambda&& lambda) {
		auto function = std::function<void(Entity&, Component&, Components&...)>(std::move(lambda));
		ComponentView<Component, Components...>(this, safeCollection<Component>(), safeCollection<Components>()...).each(function);
	}

	template <typename Lambda>
	void EntityManager::each(Lambda&& lambda) {
		auto function = std::function<void(Entity&)>(std::move(lambda));

		if (available) {
			for (auto entityId : entities) {
				auto clone = entityId; // Needed?
				auto masked = clone & Entity::ID_MASK;

				if (entityId == masked) {
					auto entity = Entity(entityId, this);
					function(entity);
				}
			}
		}
		else {
			for (auto entityId : entities) {
				auto entity = Entity(entityId, this);
				function(entity);
			}
		}
	}

	template <typename Component>
	unsigned EntityManager::count() {
		return managed<Component>() ? unsafeCollection<Component>().size() : 0U;
	}

	unsigned EntityManager::size() const {
		return entities.size() - available;
	}

	Entity::Version EntityManager::version(Entity::Id entityId) const {
		return Entity::Version((entityId >> Entity::VERSION_SHIFT) & Entity::VERSION_MASK);
	}

	Entity::Version EntityManager::current(Entity::Id entityId) const {
		auto index = entityId & Entity::ID_MASK;
		assert(index < entities.size());
		return Entity::Version((entities[index] >> Entity::VERSION_SHIFT) & Entity::VERSION_MASK);
	}

	bool EntityManager::valid(Entity::Id entityId) const {
		auto index = entityId & Entity::ID_MASK;
		return index < entities.size() && entities[index] == entityId;
	}

	void EntityManager::destroy(Entity::Id entityId) {
		validate(entityId);

		auto entity = entityId & Entity::ID_MASK;
		auto version = (entityId & (~Entity::ID_MASK)) + (1U << Entity::VERSION_SHIFT);
		auto node = (available ? next : ((entity + 1U) & Entity::ID_MASK)) | version;

		entities[entity] = node;
		next = entity;
		available++;

		for (auto& collection : collections) {
			if (collection && collection->contains(entityId)) {
				collection->remove(entityId);
			}
		}
	}

	template <typename Component>
	ComponentCollection<Component>& EntityManager::safeCollection() {
		auto uid = ComponentFamily::uid<Component>();

		if (uid >= collections.size()) {
			collections.resize(uid + 1U);
		}

		if (!collections[uid]) {
			collections[uid] = std::make_unique<ComponentCollection<Component>>();
		}

		return unsafeCollection<Component>();
	}

	template <typename Component>
	ComponentCollection<Component>& EntityManager::unsafeCollection() {
		auto& collection = collections[ComponentFamily::uid<Component>()];
		return static_cast<ComponentCollection<Component>&>(*collection);
	}

	template <typename Component>
	bool EntityManager::managed() const {
		auto uid = ComponentFamily::uid<Component>();
		return uid < collections.size() && collections[uid];
	}

	template <bool>
	bool EntityManager::has(Entity::Id entityId) {
		return true; // Fallback function for recursion
	}

	template <bool>
	bool EntityManager::empty() const {
		return false; // Fallback function for recursion
	}

	template <bool>
	void EntityManager::reset() {
		// Fallback blank function for recursion
	}

	template <bool>
	void EntityManager::remove(Entity::Id entityId) {
		// Fallback blank function for recursion
	}

	template <bool>
	void EntityManager::reset(Entity::Id entityId) {
		// Fallback blank function for recursion
	}

	void EntityManager::assign(Entity::Id entityId) {
		// Fallback blank function for recursion
	}

	void EntityManager::replace(Entity::Id entityId) {
		// Fallback blank function for recursion
	}

	void EntityManager::save(Entity::Id entityId) {
		// Fallback blank function for recursion
	}

	void EntityManager::validate(Entity::Id entityId) {
		if (!valid(entityId)) throw new std::runtime_error("Invalid entity identifier");
	}
}

#endif