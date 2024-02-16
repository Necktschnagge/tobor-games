#pragma once

#include <QSignalMapper>

#include <utility>
#include <type_traits>

namespace _Internal_Implementation_Tobor_Traits
{

	template <class T, bool HAS_mappedInt>
	struct _Wrapper_mappedInt_member_function;

	template <class T>
	struct _Wrapper_mappedInt_member_function<T, false> {
		inline static const auto mappetPtr{ QOverload<int>::of(&T::mapped) };
	};

	template <class T>
	struct _Wrapper_mappedInt_member_function<T, true> {
		inline static const auto mappetPtr{ &T::mappedInt };
	};

	// primary template which should never be instantiated.
	template<typename, typename T>
	struct _Checker_has_member_function_mappedInt {
		static_assert(
			std::integral_constant<T, false>::value,
			"This struct should not be instantiated."
			); // we need to use an expression evaluating to false which is only evaluated when struct is instantiated
		static_assert(
			std::is_same<T, bool>::value,
			"This struct should not be instantiated."
			); // to ensure that bool(T(false)) is not true
	};

	// specialization for checking existance of C::mappedInt(Args...)
	template<typename T, typename ReturnType, typename... Args>
	struct _Checker_has_member_function_mappedInt<T, ReturnType(Args...)> {
	private:

		template<typename U>
		static constexpr auto check(U*) ->
			typename std::is_same<decltype(std::declval<U>().mappedInt(std::declval<Args>()...)), ReturnType>::type;
		// function declaration with return type std::true_type iff mappedInt exists as a member function.

		template<typename>
		static constexpr std::false_type check(...);

		typedef decltype(check<T>(0)) type;

	public:
		static constexpr bool value = type::value;
	};

}

static constexpr bool QSignalMapper_HAS_mappedInt{
	_Internal_Implementation_Tobor_Traits::_Checker_has_member_function_mappedInt<QSignalMapper, void(int)>::value
};

inline static const auto QSignalMapper__mappedInt__OR__mapped__PTR{
	_Internal_Implementation_Tobor_Traits::_Wrapper_mappedInt_member_function<QSignalMapper, QSignalMapper_HAS_mappedInt>::mappetPtr
};

namespace {
	struct example {

		static void test() {
			(void)QSignalMapper__mappedInt__OR__mapped__PTR; // get rid of the unused warning
		}
	};
}
