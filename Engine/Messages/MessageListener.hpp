#ifndef MESSAGES_MESSAGE_HANDLER_IMPL
#define MESSAGES_MESSAGE_HANDLER_IMPL

namespace mqs
{
	template <typename...>
	class MessageListener;

	template <typename M>
	class MessageListener<M>
	{
	public:
		virtual void handle(const M&) = 0;
	};

	template <typename M, typename... Mn>
	class MessageListener<M, Mn...> : public MessageListener<M>, public MessageListener<Mn>...
	{
	};
}

#endif