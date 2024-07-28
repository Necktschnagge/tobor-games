#pragma once

namespace tobor {

	template<uint64_t X>
	inline constexpr uint64_t FACULTY{ X * FACULTY<X - 1> };

	template<>
	inline constexpr uint64_t FACULTY<0>{ 1 };
}
#pragma once
