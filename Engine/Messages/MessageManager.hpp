#ifndef MESSAGES_MESSAGE_MANAGER_IMPL
#define MESSAGES_MESSAGE_MANAGER_IMPL

#include <typeindex>
#include <unordered_map>

#include "MessageChannel.hpp"
#include "MessageListener.hpp"

namespace mqs
{
	class MessageManager final
	{
	public:
		// Installs a message hook which will be triggered before a message is about to be published
		SignalConnection hook(const std::function<void(const mqs::Message&)>& function) { // TODO Change to bool return type
			return hooks.pre.connect(function);
		}

		// Installs a message hook which will be triggered after a message is published
		SignalConnection hooked(const std::function<void(const mqs::Message&)>& function) {
			return hooks.post.connect(function);
		}

		// Registers an even handler of the given message type
		template <typename M>
		SignalConnection on(const std::function<void(const mqs::Message&)>& function) {
			return channels[typeid(M)].connect(function);
		}

		// Registers an even handler of the given message type
		template <typename M, typename = typename std::enable_if<std::is_base_of<mqs::Message, M>::value>::type>
		SignalConnection on(const mqs::MessageListener<M>* listener) {
			auto constlessListener = const_cast<mqs::MessageListener<M>*>(listener);

			return channels[typeid(M)].connect([constlessListener](const mqs::Message& message) {
				constlessListener->handle(dynamic_cast<const M&>(message));
			});
		}

		// Constructs a message in-place and immediatelly publish it
		template <typename M, typename = typename std::enable_if<std::is_base_of<mqs::Message, M>::value>::type, typename... Args>
		void publish(Args&&... messageArgs) {
			channels[typeid(M)].publish<M>(hooks, messageArgs...);
		}

		// Immediatelly publishes a message
		void publish(const mqs::Message& message) {
			channels[typeid(message)].publish(hooks, message);
		}

		// Publishes every queued messages of a given type
		template <typename M, typename = typename std::enable_if<std::is_base_of<mqs::Message, M>::value>::type>
		void flush() {
			channels[typeid(M)].flush(hooks);
		}

		// Publishes every queued messages of all types
		void flush() {
			for (auto& pair : channels) {
				pair.second.flush(hooks);
			}
		}

		// Constructs a message in-place and queue it
		template <typename M, typename = typename std::enable_if<std::is_base_of<mqs::Message, M>::value>::type, typename... Args>
		void push(Args&&... messageArgs) {
			channels[typeid(M)].push<M>(messageArgs...);
		}

		// Checks whether there are pending messages of a given type to be published
		template <typename M, typename = typename std::enable_if<std::is_base_of<mqs::Message, M>::value>::type>
		bool pending() {
			return channels[typeid(M)].pending();
		}

		// Checks whether there are any pending messages to be published
		bool pending() {
			bool pending = false;

			for (auto& pair : channels) {
				pending |= pair.second.pending();
			}

			return pending;
		}

	private:
		mqs::MessageHook hooks;
		std::unordered_map<std::type_index, mqs::MessageChannel> channels;
	};
}

#endif