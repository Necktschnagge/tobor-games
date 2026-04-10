#pragma once

#include <cstdint>

namespace tobor {

	template<std::uint64_t X>
	inline constexpr std::uint64_t FACULTY{ X * FACULTY<X - 1> };

	template<>
	inline constexpr std::uint64_t FACULTY<0>{ 1 };
}
#pragma once
