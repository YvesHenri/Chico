#ifndef STATES_STATE_STATUS_IMPL
#define STATES_STATE_STATUS_IMPL

namespace sts
{
	enum class StateResult : unsigned
	{
		Done,
		Error,
		Running
	};
}

#endif