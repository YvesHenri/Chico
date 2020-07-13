#ifndef UTILS_EXTENSIONS_IMPL
#define UTILS_EXTENSIONS_IMPL

#include <type_traits>

namespace std
{
	template <typename Super, typename Base>
	using if_inherits = typename enable_if<is_base_of<Super, Base>::value>::type;

	template <typename Super, typename Base>
	using enable_if_extends = typename enable_if<is_base_of<Super, Base>::value>::type;
}

#endif