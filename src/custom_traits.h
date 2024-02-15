#pragma once

#include <QSignalMapper>

namespace _Internal_Implementation_Tobor_Traits
{
	template <class T>
	struct member_function_user {
		using function_return_type = decltype(
			declval(T).mappedInt()
			);
	};

	template <class T, class Dummy>
	struct get_mapped_ptr {
		inline static const auto mappetPtr{ nullptr };
	};

	template <class T>
	struct get_mapped_ptr<T, void> {
		static const auto mappetPtr{ QOverload<int>::of(&T::mapped) };
	};

	template <class T>
	struct get_mapped_int_ptr {
		inline static const auto mappetIntPtr{ &T::mappedInt };
	};
	
	template <>
	struct get_mapped_int_ptr<QSignalMapper> {
		inline static const auto mappetIntPtr{ nullptr };
	};

	template <class T, template <typename TU> class Function_User, class Dummy>
	struct select_member_function : public std::false_type {
		//static const auto mappetIntPtr{ get_mapped_ptr<QSignalMapper, void>::mappetPtr };
	};

	template <class T, template <typename TU> class Function_User>
	struct select_member_function<T, Function_User, void> : public std::true_type {
		using user = Function_User<T>;
		//inline static const auto mappetIntPtr{ get_mapped_int_ptr<QSignalMapper>::mappetIntPtr };
	};

} // namespace detail

static constexpr bool QSignalMapper_HAS_mappedInt{
	   _Internal_Implementation_Tobor_Traits::select_member_function<
   QSignalMapper,
   _Internal_Implementation_Tobor_Traits::member_function_user,
   void>
   ::value
};

/*
static const auto QSignalMapper__mappetIntPtr{
	_Internal_Implementation_Tobor_Traits::select_member_function<
		QSignalMapper,
		_Internal_Implementation_Tobor_Traits::member_function_user,
		void
	>::mappetIntPtr
};
*/

//template<template<class...> class Op, class... Args>
//using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

static_assert(
	_Internal_Implementation_Tobor_Traits::select_member_function<
	QSignalMapper,
	_Internal_Implementation_Tobor_Traits::member_function_user,
	void>
	::value,
	"test member function prescence");
