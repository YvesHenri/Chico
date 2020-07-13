#ifndef UTILS_FAMILY_IMPL
#define UTILS_FAMILY_IMPL

#include <type_traits>

/**
* @brief Dynamic identifier generator.
*
* Utility class template that can be used to assign unique identifiers to types
* at runtime. Use different specializations to create separate sets of identifiers.
*/
template <typename...>
class Family final
{
public:
	/**
	* @brief Returns an unique identifier for the given type.
	* @return Statically generated unique identifier for the given type.
	*/
	template <typename... T>
	static unsigned uid() noexcept {
		return generate<std::decay_t<T>...>(); // Remove L/R values
	}

private:
	static unsigned entity() noexcept {
		static unsigned value = 0U;
		return value++;
	}

	template <typename...>
	static unsigned generate() noexcept {
		static unsigned value = entity();
		return value;
	}
};

using MessageFamily = Family<struct Messages>;
using ComponentFamily = Family<struct Components>;

#endif