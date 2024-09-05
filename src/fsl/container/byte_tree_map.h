#pragma once

#include <array>
#include "../rudi/macros/list.h"
#include "bytewise.h"


namespace fsl {


	template<
		std::size_t Depth,
		class Key_Type,
		class Value_Type,
		std::size_t Byte_Size = byte_size_t<Key_Type>::value,
		class Byte_Access = get_byte_t<Key_Type>
	>
	struct __byte_node {

		static constexpr std::size_t DEPTH{ Depth };
		using key_type = Key_Type;
		using value_type = Value_Type;
		static constexpr std::size_t BYTE_SIZE{ Byte_Size };
		using byte_access_type = Byte_Access;

		static_assert(DEPTH > 1);

		using sub_type = __byte_node<
			DEPTH - 1,
			key_type,
			value_type,
			BYTE_SIZE,
			Byte_Access
		>;

		using sub_ptr_type = sub_type*;
		using ptr_array_type = sub_ptr_type[256];

#if false
		using sub_iterator = sub_type::iterator;
		using sub_const_iterator = sub_type::const_iterator;

		class iterator {
			sub_ptr_type _ptr;

			const sub_ptr_type _end;

			std::optional<sub_iterator> _sub_iter;

			explicit iterator(const sub_ptr_type& ptr, const sub_iterator& sub_iter) : _ptr(ptr), _sub_iter(sub_iter) {}
			//friend class __byte_node;
		public:
			inline iterator& operator++() {
				if (!_sub_iter) {
					_sub_iter = _pter->begin();
				}
				if (_sub_iter != _ptr->end()) {
					++_sub_iter;
					return *this;
				}
				++_ptr;

				// must also jump over the nullptr elements:
				while (*_ptr == nullptr && _ptr != _end) {
					++_ptr;
				}
				_sub_iter.reset();
				return *this;
			}
			inline iterator operator++(int) {
				iterator c = *this;
				this->operator++();
				return c;
			}

			inline value_type& operator*() { return *_sub_iter; }
			inline const value_type& operator*() const { return *_sub_iter; }

			inline bool operator==(const iterator& other) const {
				return _ptr == other._ptr && _sub_iter == other._sub_iter;
			}
			inline bool operator!=(const iterator& other) const {
				return _ptr != other._ptr || _sub_iter != other._sub_iter;
			}
			inline bool operator<(const iterator& other) const {
				return _ptr < other._ptr || (_ptr == other._ptr && _sub_iter < other._sub_iter);
			}

		};
#endif


		ptr_array_type _arr;

		__byte_node(const value_type& default_value) {
			(void)default_value;
			for (uint16_t i{ 0 }; i < 256; ++i) {
				_arr[i] = nullptr;
			}
		}

		inline value_type& reference(const key_type& key, const value_type& default_value) {
			sub_ptr_type& sub_ptr{ _arr[Byte_Access()(key, BYTE_SIZE - DEPTH)] };
			if (sub_ptr == nullptr) {
				sub_ptr = new sub_type(default_value);
			}
			return sub_ptr->reference(key, default_value);
		}

		inline value_type get(const key_type& key, const value_type& default_value) const {
			sub_ptr_type sub_ptr{ _arr[Byte_Access()(key, BYTE_SIZE - DEPTH)] };
			if (sub_ptr == nullptr) return default_value;
			return sub_ptr->get(key, default_value);
		}

		~__byte_node() {
			for (uint16_t i{ 0 }; i < 256; ++i) {
				delete _arr[i];
			}
		}

	};


	template<
		class Key_Type,
		class Value_Type,
		std::size_t Byte_Size,
		class Byte_Access
	>
	struct __byte_node<1, Key_Type, Value_Type, Byte_Size, Byte_Access> {

		static constexpr std::size_t DEPTH{ 1 };
		using key_type = Key_Type;
		using value_type = Value_Type;
		static constexpr std::size_t BYTE_SIZE{ Byte_Size };
		using byte_access_type = Byte_Access;


		using value_type_ptr = value_type*;

		using value_array_type = value_type[256];
		value_array_type _arr;


		using iterator = value_type_ptr;
		using const_iterator = const value_type*;




		__byte_node(const value_type& default_value) : _arr{ FSL_COMMA_SEPARATED_STRINGS_256(default_value) }
		{
		}

		inline value_type& reference(const key_type& key, const value_type& default_value) {
			(void)default_value;
			return _arr[Byte_Access()(key, BYTE_SIZE - DEPTH)];
		}

		inline value_type get(const key_type& key, const value_type& default_value) const {
			(void)default_value;
			return _arr[Byte_Access()(key, BYTE_SIZE - DEPTH)];
		}

		~__byte_node() {
		}


		/*
		class iterator {
			value_type_ptr _ptr;

			explicit iterator(const value_type_ptr& ptr) : _ptr(ptr) {}
			//friend class __byte_node;
		public:
			inline iterator& operator++() { ++_ptr; return *this; }
			inline iterator operator++(int) { iterator c = *this; ++_ptr; return c; }

			inline value_type& operator*() { return *_ptr; }

			inline bool operator==(const iterator& other) const { return _ptr == other._ptr; }
			inline bool operator!=(const iterator& other) const { return _ptr != other._ptr; }
			inline bool operator<(const iterator& other) const { return _ptr < other._ptr; }
		};
		*/


		inline iterator begin() noexcept { return _arr; }
		inline const_iterator begin() const noexcept { return _arr; }
		inline const_iterator cbegin() const noexcept { return begin(); }

		inline iterator end() noexcept { return _arr + 256; }
		inline const_iterator end() const noexcept { return _arr + 256; }
		inline const_iterator cend() const noexcept { return end(); }

	};




	template<class Key_Type, class Value_Type, std::size_t Byte_Size = byte_size_t<Key_Type>::value, class Byte_Access = get_byte_t<Key_Type>>
	class byte_tree_map {
	public:
		static_assert(Byte_Size > 0, "Byte_Size must be positive.");

		using key_type = Key_Type;
		using value_type = Value_Type;
		static constexpr std::size_t BYTE_SIZE{ Byte_Size };
		using byte_access_type = Byte_Access;

		//template<class T>
		//using value_container_type = Value_Container_Type<T>;

		using value_type_ptr = value_type*;

		class iterator {

		};

	private:


		const value_type _default_value;

		__byte_node<BYTE_SIZE, key_type, value_type, BYTE_SIZE, byte_access_type> _root;

		//std::size_t _count_non_default_values;

	public:

		byte_tree_map(const value_type& default_value) : _default_value(default_value), _root(default_value) {
		}

		value_type& operator[](const key_type& key) {
			return _root.reference(key, _default_value);
		}

		value_type get(const key_type& key) const {
			return _root.get(key, _default_value);
		}

	};


}


