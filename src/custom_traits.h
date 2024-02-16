#pragma once

#include <QSignalMapper>

#include <utility>
#include <type_traits>

namespace _Internal_Implementation_Tobor_Traits
{
	template <class T>
	struct member_function_user {
		static void test(T& t) {
			t.mappedInt(5);

		}
		
	};

	template <class T, bool HAS_mappedInt>
	struct get_mapped_ptr;
	
	template <class T>
	struct get_mapped_ptr<T, false> {
		inline static const auto mappetPtr{ QOverload<int>::of(&T::mapped) };
	};
	
	template <class T>
	struct get_mapped_ptr<T, true> {
		inline static const auto mappetPtr{ &T::mappedInt };
	};

	/*
	template <class T>
	struct get_mapped_int_ptr {
		inline static const auto mappetIntPtr{ &T::mappedInt };
	};
	
	template <>
	struct get_mapped_int_ptr<QSignalMapper> {
		inline static const auto mappetIntPtr{ nullptr };
	};
	*/

	template <class T, template <typename TU> class Function_User, class Dummy>
	struct select_member_function {

		static constexpr bool value{ false };

		//static const auto mappetIntPtr{ get_mapped_ptr<QSignalMapper, void>::mappetPtr };
	};

	template <class T, template <typename TU> class Function_User>
	struct select_member_function<T, Function_User, void> {

		static constexpr bool value{ true };

		using user = Function_User<T>;
		//inline static const auto mappetIntPtr{ get_mapped_int_ptr<QSignalMapper>::mappetIntPtr };
	};

// Primary template with a static assertion
// for a meaningful error message
// if it ever gets instantiated.
// We could leave it undefined if we didn't care.

	template<typename, typename T>
	struct has_m {
		static_assert(
			std::integral_constant<T, false>::value,
			"Second template parameter needs to be of function type.");
	};

	// specialization that does the checking

	template<typename C, typename Ret, typename... Args>
	struct has_m<C, Ret(Args...)> {
	private:
		template<typename T>
		static constexpr auto check(T*)
			-> typename
			std::is_same<
			decltype(std::declval<T>().mappedInt(std::declval<Args>()...)),
			Ret    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			>::type;  // attempt to call it and see if the return type is correct

		template<typename>
		static constexpr std::false_type check(...);

		typedef decltype(check<C>(0)) type;

	public:
		static constexpr bool value = type::value;
	};

	
} // namespace detail

//static constexpr bool QSignalMapper_HAS_mappedInt{
//	   _Internal_Implementation_Tobor_Traits::select_member_function<
//   QSignalMapper,
//   _Internal_Implementation_Tobor_Traits::member_function_user,
//   void>
//   ::value
//};
static constexpr bool QSignalMapper_HAS_mappedInt{
	_Internal_Implementation_Tobor_Traits::has_m<QSignalMapper, void(int)>::value
};

inline static auto mapped_ptr{
	_Internal_Implementation_Tobor_Traits::get_mapped_ptr<QSignalMapper, QSignalMapper_HAS_mappedInt>::mappetPtr
};

static_assert(
	_Internal_Implementation_Tobor_Traits::select_member_function<
	QSignalMapper,
	_Internal_Implementation_Tobor_Traits::member_function_user,
	void>
	::value,
	"test member function prescence"); // does not work

static_assert(
	_Internal_Implementation_Tobor_Traits::has_m<QSignalMapper, void(int)>::value,
	"2nd try to check"); // does not work

namespace {
	struct example {

		static void test() {
			(void)mapped_ptr; // get rid of the unused warning
		}
	};
}
