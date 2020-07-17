#ifndef ECS_SYSTEM_IMPL
#define ECS_SYSTEM_IMPL

#include "../Entity/EntityManager.hpp"
#include "../../Messages/MessageManager.hpp"

namespace ecs
{
	class System
	{
	public:
		virtual void draw(float delta) = 0;
		virtual void update(float delta) = 0;
		virtual void configure(const std::shared_ptr<ecs::EntityManager>& entities, const std::shared_ptr<mqs::MessageManager>& messages) {
			this->messages = messages;
			this->entities = entities;
		}

	protected:
		std::shared_ptr<mqs::MessageManager> messages;
		std::shared_ptr<ecs::EntityManager> entities;
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
			for (auto& connection : connections) {
				connection.disconnect();
			}
		}

		void configure(const std::shared_ptr<ecs::EntityManager>& entities, const std::shared_ptr<mqs::MessageManager>& messages) override {
			System::configure(entities, messages);
			connections.push_back(messages->on<T>(this));
		}

	private:
		std::vector<mqs::SignalConnection> connections; // Avoid copy problems
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

		void configure(const std::shared_ptr<ecs::EntityManager>& entities, const std::shared_ptr<mqs::MessageManager>& messages) override {
			System::configure(entities, messages);
			connections.push_back(messages->on<T>(this));
			connections.push_back(messages->on<Tn...>(this));
		}

	private:
		std::vector<mqs::SignalConnection> connections;
	};
}

#endif