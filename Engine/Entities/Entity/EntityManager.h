#ifndef ECS_ENTITY_MANAGER_DEF
#define ECS_ENTITY_MANAGER_DEF

#include <memory>

#include "../Component/ComponentView.hpp"
#include "../Component/Message/EntityAdded.hpp"
#include "../Component/Message/EntityRemoved.hpp"

namespace ecs
{
	class Entity;

	class EntityManager final
	{
	public:
		EntityManager(const std::shared_ptr<mqs::MessageManager>& messages);
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
		Component assign(unsigned entityId, Args&&... componentArgs);

		template <typename Component, typename... Args>
		Component replace(unsigned entityId, Args&&... componentArgs);

		template <typename Component, typename... Args>
		Component save(unsigned entityId, Args&&... componentArgs);

		template <typename Component>
		Component& component(unsigned entityId);

		template <typename Component, typename... Components>
		void assign(unsigned entityId, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void replace(unsigned entityId, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void save(unsigned entityId, const Component& component, const Components&... components);

		template <typename Component, typename... Components>
		void reset(unsigned entityId);

		template <typename Component, typename... Components>
		void reset(unsigned entityId, const Component&, const Components&...);

		template <typename Component, typename... Components>
		void reset();

		template <typename Component, typename... Components>
		void remove(unsigned entityId);

		template <typename Component, typename... Components>
		void remove(unsigned entityId, const Component&, const Components&...);

		template <typename Component, typename... Components>
		bool has(unsigned entityId);

		template <typename Component, typename... Components, typename Lambda>
		void each(Lambda&& lambda);

		template <typename Lambda>
		void each(Lambda&& lambda);

		template <typename Component>
		unsigned count();

		unsigned size() const;

		unsigned version(unsigned entityId) const;
		unsigned current(unsigned entityId) const;

		bool valid(unsigned entityId) const;

		void destroy(unsigned entityId);

	private:
		template <typename Component>
		ComponentCollection<Component>& safeCollection();

		template <typename Component>
		ComponentCollection<Component>& unsafeCollection();

		template <typename Component>
		bool managed() const;

		template <bool = true>
		bool has(unsigned entityId);

		template <bool = true>
		bool empty() const;

		template <bool = true>
		void reset();

		template <bool = true>
		void remove(unsigned entityId);

		template <bool = true>
		void reset(unsigned entityId);

		void assign(unsigned entityId);

		void replace(unsigned entityId);

		void save(unsigned entityId);

		void validate(unsigned entityId);

	private:
		unsigned next = 0U;
		unsigned available = 0U;
		std::vector<unsigned> entities;
		std::vector<std::unique_ptr<Collection>> collections;
		std::shared_ptr<mqs::MessageManager> messages;
	};
}

#endif