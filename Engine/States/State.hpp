#ifndef STATES_STATE_DEF
#define STATES_STATE_DEF

#include "StateResult.hpp"

namespace sts
{
	class State
	{
	public:
		virtual ~State() = default;

		virtual void onEnter() = 0;
		virtual void onLeave() = 0;

		virtual StateResult update(float dt) = 0;
	};
}

#endif