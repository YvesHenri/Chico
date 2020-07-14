#ifndef ECS_SYSTEM_MANAGER_IMPL
#define ECS_SYSTEM_MANAGER_IMPL

#include "System.hpp"

namespace ecs
{
	class SystemManager final
	{
	public:
		SystemManager(const std::shared_ptr<ecs::EntityManager>& entities, const std::shared_ptr<mqs::MessageManager>& messages)
			: entities(entities)
			, messages(messages)
		{}

		template <typename S, typename = typename std::enable_if<std::is_base_of<ecs::System, S>::value>::type, typename... Args>
		void add(Args&&... systemArgs) {
			auto system = new S(std::forward<Args>(systemArgs)...);
			system->configure(entities, messages);
			systems.emplace_back(system);
		}

		void update(float delta) {
			for (auto& system : systems) {
				system->update(delta);
			}
		}

	private:
		std::shared_ptr<mqs::MessageManager> messages;
		std::shared_ptr<ecs::EntityManager> entities;
		std::vector<std::shared_ptr<System>> systems;
	};
}

#endif