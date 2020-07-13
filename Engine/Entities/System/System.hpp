#ifndef ECS_SYSTEM_IMPL
#define ECS_SYSTEM_IMPL

#include "../Entity/EntityManager.hpp"
#include "../../Messages/MessageManager.hpp"

namespace ecs
{
	class System
	{
	public:
		virtual void update(float dt) = 0;
		virtual void configure(const std::weak_ptr<ecs::EntityManager>& entities, const std::weak_ptr<mqs::MessageManager>& messages) {
			this->messages = messages;
			this->entities = entities;
		}

	protected:
		std::weak_ptr<mqs::MessageManager> messages;
		std::weak_ptr<ecs::EntityManager> entities;
	};

	// Definition
	template <typename...>
	class SystemListener;

	// 1 argument specialization
	template <typename T>
	class SystemListener<T> : public System, public mqs::MessageListener<T>
	{
	public:
		virtual ~SystemListener() {
			connection.disconnect();
		}

		void configure(const std::weak_ptr<ecs::EntityManager>& entities, const std::weak_ptr<mqs::MessageManager>& messages) override {
			System::configure(entities, messages);

			if (const auto& pointer = messages.lock()) {
				connection = pointer->on<T>(this);
			}
		}

	private:
		mqs::SignalConnection connection;
	};

	// 1+ arguments specialization
	template <typename T, typename... Tn>
	class SystemListener<T, Tn...> : public System, public mqs::MessageListener<T, Tn...>
	{
	public:
		virtual ~SystemListener() {
			for (auto& connection : connections) {
				connection.disconnect();
			}
		}

		void configure(const std::weak_ptr<ecs::EntityManager>& entities, const std::weak_ptr<mqs::MessageManager>& messages) override {
			System::configure(entities, messages);

			if (const auto& pointer = messages.lock()) {
				connections.push_back(pointer->on<T>(this));
				connections.push_back(pointer->on<Tn...>(this));
			}
		}

	private:
		std::vector<mqs::SignalConnection> connections;
	};
}

#endif