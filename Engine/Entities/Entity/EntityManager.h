#ifndef ECS_ENTITY_MANAGER_DEF
#define ECS_ENTITY_MANAGER_DEF

#include <memory>

#include "Entity.h"

namespace ecs
{
	class EntityManager final
	{
	public:
		EntityManager() = default;
		EntityManager(const EntityManager&) = delete;
		EntityManager(EntityManager&&) = default;

		EntityManager& operator=(const EntityManager&) = delete;
		EntityManager& operator=(EntityManager&&) = default;

		Entity create();

		template <typename Component, typename... Args>
		Entity create(Args&&... componentArgs);

		template <typename Component, typename... Components>
		Entity create(const Component& component, const Components&... components);

		template <typename Component, typename... Args>
		Component assign(Entity::Id entityId, Args&&... componentArgs);

		template <typename Component, typename... Args>
		Component replace(Entity::Id entityId, Args&&... componentArgs);

		template <typename Component, typename... Args>
		Component save(Entity::Id entityId, Args&&... componentArgs);

		template <typename Component>
		Component& component(Entity::Id entityId);

		template <typename Component, typename... Components>
		void assign(Entity::Id entityId, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void replace(Entity::Id entityId, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void save(Entity::Id entityId, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void reset(Entity::Id entityId);

		template <typename Component, typename... Components>
		void reset(Entity::Id entityId, const Component&, const Components&...);

		template <typename Component, typename... Components>
		void reset();

		template <typename Component, typename... Components>
		void remove(Entity::Id entityId);

		template <typename Component, typename... Components>
		void remove(Entity::Id entityId, const Component&, const Components&...);

		template <typename Component, typename... Components>
		bool has(Entity::Id entityId);

		template <typename Component, typename... Components, typename Lambda>
		void each(Lambda&& lambda);

		template <typename Lambda>
		void each(Lambda&& lambda);

		template <typename Component>
		unsigned count();

		unsigned size() const;

		Entity::Version version(Entity::Id entityId) const;

		Entity::Version current(Entity::Id entityId) const;

		bool valid(Entity::Id entityId) const;

		void destroy(Entity::Id entityId);

	private:
		template <typename Component>
		ComponentCollection<Component>& safeCollection();

		template <typename Component>
		ComponentCollection<Component>& unsafeCollection();

		template <typename Component>
		bool managed() const;

		template <bool = true>
		bool has(Entity::Id entityId);

		template <bool = true>
		bool empty() const;

		template <bool = true>
		void reset();

		template <bool = true>
		void remove(Entity::Id entityId);

		template <bool = true>
		void reset(Entity::Id entityId);

		void assign(Entity::Id entityId);

		void replace(Entity::Id entityId);

		void save(Entity::Id entityId);

		void validate(Entity::Id entityId);

	private:
		unsigned next = 0U;
		unsigned available = 0U;
		std::vector<unsigned> entities;
		std::vector<std::unique_ptr<Collection>> collections;
	};
}

#endif