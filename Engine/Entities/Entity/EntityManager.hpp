#ifndef ECS_ENTITY_MANAGER_IMPL
#define ECS_ENTITY_MANAGER_IMPL

#include <cassert>

#include "EntityManager.h"
#include "Entity.h"
#include "../../Family.hpp"

namespace ecs
{
	inline EntityManager::EntityManager(const std::shared_ptr<mqs::MessageManager>& messages) : messages(messages) {
		next = 0U;
		available = 0U;
	}

	inline Entity EntityManager::create() {
		unsigned id = 0U;

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

		messages->publish<EntityAdded>(id);

		return Entity(id, this);
	}

	template <typename Component, typename... Args>
	inline Entity EntityManager::create(Args&&... componentArgs) {
		auto entity = create();
		entity.assign<Component>(componentArgs...);
		return entity;
	}

	template <typename Component, typename... Components>
	inline Entity EntityManager::create(const Component& component, const Components&... components) {
		auto entity = create();
		entity.assign(component, components...);
		return entity;
	}

	template <typename Component, typename... Args>
	inline Component EntityManager::assign(unsigned entityId, Args&&... componentArgs) {
		validate(entityId);
		auto component = Component(std::forward<Args>(componentArgs)...);
		safeCollection<Component>().add(entityId, component);
		return component;
	}

	template <typename Component, typename... Args>
	inline Component EntityManager::replace(unsigned entityId, Args&&... componentArgs) {
		validate(entityId);
		auto component = Component(std::forward<Args>(componentArgs)...);
		unsafeCollection<Component>().replace(entityId, component);
		return component;
	}

	template <typename Component, typename... Args>
	inline Component EntityManager::save(unsigned entityId, Args&&... componentArgs) {
		validate(entityId);
		auto component = Component(std::forward<Args>(componentArgs)...);
		safeCollection<Component>().save(entityId, component);
		return component;
	}

	template <typename Component>
	inline Component& EntityManager::component(unsigned entityId) {
		validate(entityId);
		return unsafeCollection<Component>().get(entityId);
	}

	template <typename Component, typename... Components>
	inline void EntityManager::assign(unsigned entityId, const Component& component, const Components&... components) {
		validate(entityId);
		safeCollection<Component>().add(entityId, component);
		assign(entityId, components...);
	}

	template <typename Component, typename... Components>
	inline void EntityManager::replace(unsigned entityId, const Component& component, const Components&... components) {
		validate(entityId);
		unsafeCollection<Component>().replace(entityId, component);
		replace(entityId, components...);
	}

	template <typename Component, typename... Components>
	inline void EntityManager::save(unsigned entityId, const Component& component, const Components&... components) {
		validate(entityId);
		safeCollection<Component>().save(entityId, component);
		save(entityId, components...);
	}

	template <typename Component, typename... Components>
	inline void EntityManager::reset(unsigned entityId) {
		validate(entityId);

		if (managed<Component>()) {
			unsafeCollection<Component>().reset(entityId);
		}

		reset<Components...>(entityId);
	}

	template <typename Component, typename... Components>
	inline void EntityManager::reset(unsigned entityId, const Component&, const Components&...) {
		reset<Component, Components...>(entityId);
	}

	template <typename Component, typename... Components>
	inline void EntityManager::reset() {
		if (managed<Component>()) {
			auto& collection = unsafeCollection<Component>();

			each([&collection](Entity& entity) {
				collection.reset(entity.id());
			});
		}

		reset<Components...>();
	}

	template <typename Component, typename... Components>
	inline void EntityManager::remove(unsigned entityId) {
		validate(entityId);
		unsafeCollection<Component>().remove(entityId);
		remove<Components...>(entityId);
	}

	template <typename Component, typename... Components>
	inline void EntityManager::remove(unsigned entityId, const Component&, const Components&...) {
		remove<Component, Components...>(entityId);
	}

	template <typename Component, typename... Components>
	inline bool EntityManager::has(unsigned entityId) {
		validate(entityId);
		return managed<Component>() && unsafeCollection<Component>().contains(entityId) && has<Components...>(entityId);
	}

	template <typename Component, typename... Components, typename Lambda>
	inline void EntityManager::each(Lambda&& lambda) {
		auto function = std::function<void(Entity&, Component&, Components&...)>(std::move(lambda));
		ComponentView<Component, Components...>(this, safeCollection<Component>(), safeCollection<Components>()...).each(function);
	}

	template <typename Lambda>
	inline void EntityManager::each(Lambda&& lambda) {
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
	inline unsigned EntityManager::count() {
		return managed<Component>() ? unsafeCollection<Component>().size() : 0U;
	}

	inline unsigned EntityManager::size() const {
		return entities.size() - available;
	}

	inline unsigned EntityManager::version(unsigned entityId) const {
		return unsigned((entityId >> Entity::VERSION_SHIFT) & Entity::VERSION_MASK);
	}

	inline unsigned EntityManager::current(unsigned entityId) const {
		auto index = entityId & Entity::ID_MASK;
		assert(index < entities.size());
		return unsigned((entities[index] >> Entity::VERSION_SHIFT) & Entity::VERSION_MASK);
	}

	inline bool EntityManager::valid(unsigned entityId) const {
		auto index = entityId & Entity::ID_MASK;
		return index < entities.size() && entities[index] == entityId;
	}

	inline void EntityManager::destroy(unsigned entityId) {
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

		messages->publish<EntityRemoved>(entityId);
	}

	template <typename Component>
	inline ComponentCollection<Component>& EntityManager::safeCollection() {
		auto uid = ComponentFamily::uid<Component>();

		if (uid >= collections.size()) {
			collections.resize(uid + 1U);
		}

		if (!collections[uid]) {
			collections[uid] = std::make_unique<ComponentCollection<Component>>(messages);
		}

		return unsafeCollection<Component>();
	}

	template <typename Component>
	inline ComponentCollection<Component>& EntityManager::unsafeCollection() {
		auto& collection = collections[ComponentFamily::uid<Component>()];
		return static_cast<ComponentCollection<Component>&>(*collection);
	}

	template <typename Component>
	inline bool EntityManager::managed() const {
		auto uid = ComponentFamily::uid<Component>();
		return uid < collections.size() && collections[uid];
	}

	template <bool>
	inline bool EntityManager::has(unsigned entityId) {
		return true; // Fallback function for recursion
	}

	template <bool>
	inline bool EntityManager::empty() const {
		return false; // Fallback function for recursion
	}

	template <bool>
	inline void EntityManager::reset() {
		// Fallback blank function for recursion
	}

	template <bool>
	inline void EntityManager::remove(unsigned entityId) {
		// Fallback blank function for recursion
	}

	template <bool>
	inline void EntityManager::reset(unsigned entityId) {
		// Fallback blank function for recursion
	}

	inline void EntityManager::assign(unsigned entityId) {
		// Fallback blank function for recursion
	}

	inline void EntityManager::replace(unsigned entityId) {
		// Fallback blank function for recursion
	}

	inline void EntityManager::save(unsigned entityId) {
		// Fallback blank function for recursion
	}

	inline void EntityManager::validate(unsigned entityId) {
		if (!valid(entityId)) throw "Invalid entity identifier";
	}
}

#endif