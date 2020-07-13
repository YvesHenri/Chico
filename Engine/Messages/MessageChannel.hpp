#ifndef MESSAGES_MESSAGE_CHANNEL_IMPL
#define MESSAGES_MESSAGE_CHANNEL_IMPL

#include <queue>

#include "Message.hpp"
#include "MessageHook.hpp"
#include "../Signals/Signal.hpp"

namespace mqs
{
	class MessageChannel final
	{
	public:
		// Adds a message dispatcher
		template <typename Lambda>
		SignalConnection connect(Lambda&& lambda) {
			auto function = std::function<void(const mqs::Message&)>(std::move(lambda));
			return signal.connect(function);
		}

		// Constructs a message in-place and immediately publish it
		template <typename Message, typename = typename std::enable_if<std::is_base_of<mqs::Message, Message>::value>::type, typename... Args>
		void publish(const mqs::MessageHook& hook, Args&&... messageArgs) {
			auto message = Message(std::forward<Args>(messageArgs)...);
			publish(hook, message);
		}

		// Immediatelly publishes a message
		void publish(const mqs::MessageHook& hook, const mqs::Message& message) {
			hook.pre(message);
			signal(message); // TODO Execute only if preHook tells us to
			hook.post(message);
		}

		// Publishes every queued messages
		void flush(const mqs::MessageHook& hook) {
			while (!messages.empty()) {
				auto& message = *messages.front();
				publish(hook, message);
				messages.pop();
			}
		}

		// Constructs a message in-place and queue it
		template <typename Message, typename = typename std::enable_if<std::is_base_of<mqs::Message, Message>::value>::type, typename... Args>
		void push(Args&&... messageArgs) {
			auto message = std::make_shared<Message>(messageArgs...);
			messages.push(message);
		}

		// Checks whether there are pending messages to be published
		bool pending() const {
			return !messages.empty();
		}

	private:
		mqs::Signal<void(const mqs::Message&)> signal;
		std::queue<std::shared_ptr<mqs::Message>> messages;
	};
}

#endif