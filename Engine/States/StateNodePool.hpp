#pragma once

#include <typeindex>
#include <unordered_map>

#include "StateNode.hpp"

namespace sts
{
	class StateNodePool final
	{
	public:
		explicit StateNodePool(const std::shared_ptr<mqs::MessageManager>& messages)
		{
			this->messages = messages;
		}

		template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
		std::shared_ptr<StateNode>& get(const std::shared_ptr<State>& state) {
			auto& type = typeid(State);		

			// Re-configure as the node will either be created or state will be updated
			state->configure(messages);

			// Create node if state is unmanaged, otherwise overwrites the mapped state
			if (nodes.find(type) == nodes.end()) {
				nodes[type] = std::make_shared<StateNode>(state);
			}
			else {
				nodes[type]->state = state;
			}

			return nodes[type];
		}

		template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type>
		std::shared_ptr<StateNode>& get() {
			auto& type = typeid(State);

			// Create node if state is unmanaged
			if (nodes.find(type) == nodes.end()) {
				auto state = std::make_shared<State>();
				auto node = std::make_shared<StateNode>(state);

				state->configure(messages);
				nodes.emplace(type, node);
			}

			return nodes[type];
		}

	private:
		std::shared_ptr<mqs::MessageManager> messages;
		std::unordered_map<std::type_index, std::shared_ptr<StateNode>> nodes;
	};
}