#ifndef SIGNALS_SIGNAL_IMPL
#define SIGNALS_SIGNAL_IMPL

#include <vector>

#include "SignalConnection.hpp"

namespace mqs
{
	template <typename...>
	class Signal;

	template <typename R, typename... A>
	class Signal<R(A...)> final
	{
	public:
		Signal() {
			disconnector = std::make_shared<mqs::SignalDisconnector>([this](std::uint32_t index) {
				slots.erase(slots.begin() + index);
			});
		}

		template <typename Lambda>
		mqs::SignalConnection connect(Lambda&& lambda) {
			slots.push_back(std::forward<Lambda>(lambda));
			return mqs::SignalConnection(disconnector, slots.size() - 1U);
		}

		void operator()(A&&... args) const {
			for (auto& slot : slots) {
				slot(std::forward<A>(args)...);
			}
		}

		unsigned connections() const {
			return slots.size();
		}

	private:
		std::vector<std::function<R(A...)>> slots;
		std::shared_ptr<mqs::SignalDisconnector> disconnector;
	};
}

#endif