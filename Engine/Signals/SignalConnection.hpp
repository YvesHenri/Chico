#ifndef SIGNALS_CONNECTION_IMPL
#define SIGNALS_CONNECTION_IMPL

#include <memory>
#include <functional>

namespace mqs
{
	using SignalDisconnector = std::function<void(std::uint32_t)>;

	class SignalConnection final
	{
	public:
		explicit SignalConnection(std::shared_ptr<mqs::SignalDisconnector> disconnector, std::uint32_t index)
		{
			this->index = index;
			this->disconnector = disconnector;
		}

		bool connected() const {
			return !disconnector.expired();
		}

		void disconnect() const {
			if (const auto& lock = disconnector.lock()) {
				lock->operator()(index);
			}
		}

	private:
		std::uint32_t index = 0U;
		std::weak_ptr<mqs::SignalDisconnector> disconnector;
	};
}

#endif