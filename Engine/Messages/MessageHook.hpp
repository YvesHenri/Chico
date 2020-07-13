#ifndef MESSAGES_MESSAGE_HOOK_DEF
#define MESSAGES_MESSAGE_HOOK_DEF

#include "Message.hpp"
#include "../Signals/Signal.hpp"

namespace mqs
{
	struct MessageHook final
	{
		mqs::Signal<void(const mqs::Message&)> pre; // TODO Change to bool return type
		mqs::Signal<void(const mqs::Message&)> post;
	};
}

#endif