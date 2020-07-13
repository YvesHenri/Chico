#ifndef STATES_STATE_NODE_IMPL
#define STATES_STATE_NODE_IMPL

#include <memory>
#include <typeindex>
#include <unordered_map>

#include "StateResult.hpp"

namespace sts
{
	class State;

	class StateManager;

	struct StateNode final
	{
		explicit StateNode(const std::shared_ptr<State>& state) : state(state) {}

		std::shared_ptr<State> state;		
		std::unordered_map<std::type_index, std::shared_ptr<StateNode>> messages; // Message-based transitions
		std::unordered_map<sts::StateResult, std::shared_ptr<StateNode>> results; // Result-based transitions
	};
}

#endif