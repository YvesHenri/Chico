#ifndef STATES_STATE_NODE_IMPL
#define STATES_STATE_NODE_IMPL

#include <memory>
#include <typeindex>
#include <unordered_map>

#include "StateResult.hpp"
#include "../Signals/SignalConnection.hpp"

namespace sts
{
	class State;

	class StateManager;

	struct StateNode final
	{
		explicit StateNode(const std::shared_ptr<State>& state) : state(state) {}

		~StateNode() {
			for (auto& pair : connections) {
				pair.second.disconnect();
			}
		}

		std::shared_ptr<State> state;
		std::unordered_map<sts::StateResult, std::type_index> results;
		std::unordered_map<std::type_index, std::type_index> messages;
		std::unordered_map<std::type_index, mqs::SignalConnection> connections;
	};
}

#endif