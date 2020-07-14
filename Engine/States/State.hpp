#ifndef STATES_STATE_DEF
#define STATES_STATE_DEF

#include "StateResult.hpp"
#include "../Messages/MessageManager.hpp"

namespace sts
{
	class State
	{
	public:
		virtual StateResult update(float delta) = 0;

		virtual void onEnter() = 0;
		virtual void onLeave() = 0;

		virtual void configure(const std::shared_ptr<mqs::MessageManager>& messages) {
			this->messages = messages;
		}

	protected:
		std::shared_ptr<mqs::MessageManager> messages;
	};
}

#endif