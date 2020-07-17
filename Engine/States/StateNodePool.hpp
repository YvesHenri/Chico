#pragma once

#include <typeindex>
#include <unordered_map>

#include "StateNode.hpp"
#include "../Family.hpp"

namespace sts
{
	class StateNodePool final
	{
	public:
		explicit StateNodePool(const std::shared_ptr<mqs::MessageManager>& messages) : messages(messages) {}

		template <typename State, typename = typename std::enable_if<std::is_base_of<sts::State, State>::value>::type, typename... Args>
		std::shared_ptr<StateNode>& add(Args&&... stateArgs) {
			auto& stateType = typeid(State);

			// Create node only if state is unmanaged (which should be)
			if (nodes.find(stateType) == nodes.end()) {
				auto state = std::make_shared<State>(stateArgs...);
				auto node = std::make_shared<StateNode>(state);

				state->configure(messages);
				nodes[stateType] = node;
			}

			return nodes[stateType];
		}

		std::shared_ptr<StateNode>& get(const std::type_index& stateType) {
			return nodes[stateType];
		}

	private:
		std::shared_ptr<mqs::MessageManager> messages;
		std::unordered_map<std::type_index, std::shared_ptr<StateNode>> nodes;
	};
}