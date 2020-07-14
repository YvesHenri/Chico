#ifndef STATES_STATE_MANAGER_IMPL
#define STATES_STATE_MANAGER_IMPL

#include <memory>

#include "State.hpp"
#include "StateNodePool.hpp"
#include "../Messages/MessageManager.hpp"

namespace sts
{
	class StateManager final : public std::enable_shared_from_this<StateManager>
	{
	public:
		template <typename Message, typename>
		class MessageTransition;
		class ReturnTransition;

		class Mapping final
		{
		public:
			explicit Mapping(const std::shared_ptr<StateNode>& node, const std::shared_ptr<StateManager>& manager)
			{
				this->node = node;
				this->manager = manager;
			}
			
			void done() {
				manager->node->state->onEnter(); // TODO Check indeterminate state (?)
			}

			template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
			Mapping map(const std::shared_ptr<State>& state) {
				return map(manager->nodes->get<State>(state), false);
			}

			template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
			Mapping map() {
				return map(manager->nodes->get<State>(), false);
			}

			template <typename Message, typename Enable = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
			MessageTransition<Message, Enable> onMessage() {
				return MessageTransition<Message>(node, manager);
			}

			ReturnTransition onReturn(StateResult result) {
				return ReturnTransition(node, manager, result);
			}

			ReturnTransition onFinished() {
				return onReturn(StateResult::Finished);
			}

			ReturnTransition onFailed() {
				return onReturn(StateResult::Failed);
			}

			ReturnTransition onRunning() {
				return onReturn(StateResult::Running);
			}

		private:
			Mapping map(const std::shared_ptr<StateNode>& nextNode, bool addFinishedTransition) {
				// Automatically create a Finished transition if onFinished() wasn't called previously
				if (addFinishedTransition) {
					auto result = StateResult::Finished;
					auto iterator = node->results.find(result);

					if (iterator == node->results.end()) {
						node->results[result] = nextNode;
					}
				}

				return Mapping(nextNode, manager);
			}

		private:
			std::shared_ptr<sts::StateNode> node;
			std::shared_ptr<sts::StateManager> manager;
		};

		class Transition
		{
		public:
			explicit Transition(const std::shared_ptr<StateNode>& node, const std::shared_ptr<StateManager>& manager)
			{
				this->node = node;
				this->manager = manager;
			}

		protected:
			std::shared_ptr<sts::StateNode> node;
			std::shared_ptr<sts::StateManager> manager;
		};

		class ReturnTransition final : public Transition
		{
		public:
			explicit ReturnTransition(
				const std::shared_ptr<StateNode>& node, 
				const std::shared_ptr<StateManager>& manager,
				const sts::StateResult& result) : Transition(node, manager), result(result) {}

			template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
			Mapping go(const std::shared_ptr<State>& state) {
				return go<State>(manager->nodes->get<State>(state));
			}

			template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
			Mapping go() {
				return go<State>(manager->nodes->get<State>());
			}

		private:
			template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
			Mapping go(const std::shared_ptr<StateNode>& nextNode) {
				node->results[result] = nextNode;
				return Mapping(node, manager);
			}

		private:
			sts::StateResult result;
		};

		template <typename Message, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
		class MessageTransition final : public Transition
		{
		public:
			explicit MessageTransition(
				const std::shared_ptr<StateNode>& node,
				const std::shared_ptr<StateManager>& manager) : Transition(node, manager) {}

			template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
			Mapping go(const std::shared_ptr<State>& state) {
				return go<State>(manager->nodes->get<State>(state));
			}

			template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
			Mapping go() {
				return go<State>(manager->nodes->get<State>());
			}

		private:
			template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
			Mapping go(const std::shared_ptr<StateNode>& nextNode) {
				// Register listener and create transition
				if (const auto& messageManager = manager->messages.lock()) {
					auto connection = messageManager->on<Message>([this](const mqs::Message& message) {
						manager->hooked(dynamic_cast<const Message&>(message));
					});

					manager->connections.push_back(connection);
					node->messages[typeid(Message)] = nextNode;
				}				

				return Mapping(node, manager);
			}
		};

		~StateManager() {
			for (auto& connection : connections) {
				connection.disconnect();
			}
		}

		explicit StateManager(const std::shared_ptr<mqs::MessageManager>& messages) {
			this->messages = messages;
			this->nodes = std::make_shared<StateNodePool>(messages);
		}

		template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
		Mapping map(const std::shared_ptr<State>& state) {
			node = nodes->get<State>(state);
			return Mapping(node, shared_from_this());
		}

		template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
		Mapping map() {
			node = nodes->get<State>();
			return Mapping(node, shared_from_this());
		}

		template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
		bool current() {
			return std::dynamic_pointer_cast<State>(node->state).operator bool();
		}

		void update(float dt) {
			auto result = node->state->update(dt);
			auto iterator = node->results.find(result);

			if (iterator != node->results.end()) {
				transit(iterator->second);
			}
		}

	private:
		void hooked(const mqs::Message& message) {
			auto& type = typeid(message);
			auto iterator = node->messages.find(type);

			if (iterator != node->messages.end()) {
				transit(iterator->second);
			}
		}

		void transit(const std::shared_ptr<StateNode>& nextNode) {
			node->state->onLeave();
			node = nextNode;
			node->state->onEnter();
		}

	private:
		std::shared_ptr<sts::StateNode> node;
		std::shared_ptr<sts::StateNodePool> nodes;
		std::weak_ptr<mqs::MessageManager> messages;
		std::vector<mqs::SignalConnection> connections;
	};
}

#endif