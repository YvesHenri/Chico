#pragma once

#include "Messages/MessageManager.hpp"
#include "Entities/Entity/Entity.hpp"
#include "Entities/Entity/EntityManager.hpp"
#include "Entities/System/SystemManager.hpp"
#include "States/StateManager.hpp"

namespace cge
{
	// Kinda behaves like a service locator
	class Engine final
	{
	public:
		static mqs::MessageManager& messages() {
			return *messageManager.get();
		}

		static ecs::EntityManager& entities() {
			return *entityManager.get();
		}

		static ecs::SystemManager& systems() {
			return *systemManager.get();
		}

		static sts::StateManager& states() {
			return *stateManager.get();
		}

	private:
		static std::shared_ptr<mqs::MessageManager> messageManager;
		static std::shared_ptr<ecs::EntityManager> entityManager;
		static std::shared_ptr<ecs::SystemManager> systemManager;
		static std::shared_ptr<sts::StateManager> stateManager;
	};

	std::shared_ptr<mqs::MessageManager> Engine::messageManager = std::make_shared<mqs::MessageManager>();
	std::shared_ptr<ecs::EntityManager> Engine::entityManager = std::make_shared<ecs::EntityManager>();
	std::shared_ptr<ecs::SystemManager> Engine::systemManager = std::make_shared<ecs::SystemManager>(entityManager, messageManager);
	std::shared_ptr<sts::StateManager> Engine::stateManager = std::make_shared<sts::StateManager>(messageManager);
}