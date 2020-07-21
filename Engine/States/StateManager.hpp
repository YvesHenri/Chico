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

			template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type, typename... Args>
			Mapping map(Args&&... stateArgs) {
				auto nextNode = manager->nodes->add<State>(stateArgs...);
				return Mapping(nextNode, manager);
			}

			template <typename Message, typename Enable = typename std::enable_if<std::is_base_of<mqs::Message, Message>::value>::type>
			MessageTransition<Message, Enable> onMessage() {
				return MessageTransition<Message>(node, manager);
			}

			ReturnTransition onReturn(StateResult result) {
				return ReturnTransition(node, manager, result);
			}

			ReturnTransition onDone() {
				return onReturn(StateResult::Done);
			}

			ReturnTransition onError() {
				return onReturn(StateResult::Error);
			}

			ReturnTransition onRunning() {
				return onReturn(StateResult::Running);
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
			Mapping go() {
				auto& state = typeid(State);
				node->results.emplace(result, state);
				return Mapping(node, manager);
			}

		private:
			sts::StateResult result;
		};

		template <typename Message, typename = typename std::enable_if<std::is_base_of<mqs::Message, Message>::value>::type>
		class MessageTransition final : public Transition
		{
		public:
			explicit MessageTransition(
				const std::shared_ptr<StateNode>& node,
				const std::shared_ptr<StateManager>& manager) : Transition(node, manager) {}

			template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
			Mapping go(const std::function<bool(const Message&)>& condition) {
				auto& state = typeid(State);
				auto& message = typeid(Message);

				//auto optionalCurrentConnetion = node->connections.find(message);

				// Remove current connection (if any), as we're going to create/overwrite it
				//if (optionalCurrentConnetion != node->connections.end())
					//optionalCurrentConnetion.second.disconnect();

				auto connection = manager->messages->on<Message>([=](const mqs::Message& message) {
					auto& concreteMessage = dynamic_cast<const Message&>(message);

					if (manager->active(node->state)) {
						if (condition(concreteMessage)) {
							manager->hooked(concreteMessage);
						}
					}
				});

				node->messages.emplace(message, state);
				node->connections.emplace(message, connection);

				return Mapping(node, manager);
			}

			template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
			Mapping go() {
				return go<State>([](const Message&) {
					return true;
				});
			}
		};

		~StateManager() {
			for (auto& pair : connections) {
				pair.second.disconnect();
			}
		}

		explicit StateManager(const std::shared_ptr<mqs::MessageManager>& messages) {
			this->messages = messages;
			this->nodes = std::make_shared<StateNodePool>(messages);
		}

		// Creates a State in-place and register it.
		template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type, typename... Args>
		Mapping map(Args&&... stateArgs) {
			node = nodes->add<State>(stateArgs...);
			return Mapping(node, shared_from_this());
		}

		template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
		bool active() {
			return std::dynamic_pointer_cast<State>(node->state).operator bool();
		}

		template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
		bool active(const std::shared_ptr<State>& unused) {
			return std::dynamic_pointer_cast<State>(node->state).operator bool();
		}

		void update(float delta) {
			auto result = node->state->update(delta);
			auto iterator = node->results.find(result);

			// Check whether there's a mapped transition
			if (iterator != node->results.end()) {
				transit(iterator->second);
			}
		}

	private:
		void hooked(const mqs::Message& message) {
			auto& type = typeid(message);
			auto iterator = node->messages.find(type);

			// Check whether there's a mapped transition
			if (iterator != node->messages.end()) {
				transit(iterator->second);
			}
		}

		void transit(const std::type_index& state) {
			auto transition = nodes->get(state);

			// Check whether there's a corresponding transition instance
			if (transition) {
				node->state->onLeave();
				node = transition;
				node->state->onEnter();
			}
		}

	private:
		std::shared_ptr<sts::StateNode> node;
		std::shared_ptr<sts::StateNodePool> nodes;
		std::shared_ptr<mqs::MessageManager> messages;
		//std::vector<mqs::SignalConnection> connections;
		std::unordered_map<std::type_index, mqs::SignalConnection> connections;
	};
}

#endif