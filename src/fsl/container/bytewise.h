#pragma once

namespace fsl {
	/**
	*	@brief Default access to T::get_byte(const U&)
	*/
	template<class T>
	struct get_byte_t {
		template<class U>
		inline uint8_t operator()(const T& obj, const U& index) const {
			return obj.get_byte(index);
		}
	};

	/**
	*	@brief Default access to T::get_byte(const U&)
	*/
	template<class T, class U>
	inline uint8_t get_byte(const T& obj, const U& index) { return get_byte_t<T>()(obj, index); }

	/**
	*	@brief Default access to T::byte_size()
	*/
	template<class T>
	struct byte_size_t {
		using size_type = decltype(T::byte_size());
		static constexpr size_type value{ T::byte_size() };
	};

	/**
	*	@brief Default access to T::byte_size()
	*/
	template<class T>
	constexpr auto byte_size() -> typename byte_size_t<T>::size_type { return byte_size_t<T>::value; }
}
