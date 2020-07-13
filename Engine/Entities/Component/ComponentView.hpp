#ifndef ENTITIES_COMPONENT_VIEW_IMPL
#define ENTITIES_COMPONENT_VIEW_IMPL

#include <functional>

#include "../Entity/Entity.h"
#include "../Component/ComponentCollectionIntersection.hpp"

namespace ecs
{
	/**
	* @brief 
	*/
	template <typename... Components>
	class ComponentView
	{
	public:
		ComponentView(EntityManager* manager, ComponentCollection<Components>&... components)
			: manager(manager)
			, intersection(components...)
		{}

		/**
		* @brief 
		*/
		void each(std::function<void(Entity&, Components&...)>& callback) {
			for (auto entityId : intersection) {
				auto entity = Entity(entityId, manager);
				callback(entity, intersection.get<Components>().get(entityId)...);
			}
		}

	private:
		EntityManager* manager;
		ComponentCollectionIntersection<Components...> intersection;
	};

	/**
	* @brief Specialization for single component views.
	*/
	template <typename Component>
	class ComponentView<Component> final
	{
	public:
		ComponentView(EntityManager* manager, ComponentCollection<Component>& components)
			: manager(manager)
			, components(components)
		{}

		/**
		* @brief
		*/
		void each(std::function<void(Entity&, Component&)>& callback) {
			for (auto entityId : components) {
				auto entity = Entity(entityId, manager);
				callback(entity, components.get(entityId));
			}
		}

	private:
		EntityManager* manager;
		ComponentCollection<Component>& components;
	};
}

#endif