#pragma once

#include <array>
#include "../rudi/macro/list.h"
#include "bytewise.h"


namespace fsl {

	/**
	* @brief Inner node for class byte_tree_map
	*/
	template<
		std::size_t Depth,
		class Key_Type,
		class Value_Type,
		std::size_t Byte_Size_C = byte_size_t<Key_Type>::value,
		class Byte_Access_Type = get_byte_t<Key_Type>
	>
	struct __byte_node {

		static constexpr std::size_t DEPTH{ Depth };
		using key_type = Key_Type;
		using value_type = Value_Type;
		static constexpr std::size_t BYTE_SIZE{ Byte_Size_C };
		using byte_access_type = Byte_Access_Type;

		static_assert(DEPTH > 1);

		using sub_type = __byte_node<
			DEPTH - 1,
			key_type,
			value_type,
			BYTE_SIZE,
			Byte_Access_Type
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

		/** The raw array of sub node pointers */
		ptr_array_type _arr;

		/**
		*	@brief Conctructs a __byte_node with all keys mapped to \p default_value
		*/
		__byte_node(const value_type& default_value) {
			(void)default_value;
			for (uint16_t i{ 0 }; i < 256; ++i) {
				_arr[i] = nullptr;
			}
		}

		/**
		*	@brief Returns a value_type reference to the value mapped to given \p key.
		*	@details If it does not have a dedicated instance of value_type, one is created inside the tree hirarchy.
		*/
		inline value_type& reference(const key_type& key, const value_type& default_value) {
			sub_ptr_type& sub_ptr{ _arr[Byte_Access_Type()(key, BYTE_SIZE - DEPTH)] };
			if (sub_ptr == nullptr) {
				sub_ptr = new sub_type(default_value);
			}
			return sub_ptr->reference(key, default_value);
		}

		/**
		*	@brief Returns the value_type which is mapped to given \p key.
		*	@details Read-only, does not modify the tree. In case there is no dedicated instance, a copy of default_value is returned.
		*/
		inline value_type get(const key_type& key, const value_type& default_value) const {
			sub_ptr_type sub_ptr{ _arr[Byte_Access_Type()(key, BYTE_SIZE - DEPTH)] };
			if (sub_ptr == nullptr) return default_value;
			return sub_ptr->get(key, default_value);
		}

		/**
		*	@brief Destroys the byte node and all its children recursively.
		*/
		~__byte_node() {
			for (uint16_t i{ 0 }; i < 256; ++i) {
				delete _arr[i];
			}
		}

	};


	template<
		class Key_Type,
		class Value_Type,
		std::size_t Byte_Size_C,
		class Byte_Access_Type
	>
	struct __byte_node<1, Key_Type, Value_Type, Byte_Size_C, Byte_Access_Type> {

		static constexpr std::size_t DEPTH{ 1 };
		using key_type = Key_Type;
		using value_type = Value_Type;
		static constexpr std::size_t BYTE_SIZE{ Byte_Size_C };
		using byte_access_type = Byte_Access_Type;


		using value_array_type = value_type[256];
		value_array_type _arr;

		/**
		*	@brief Constructs byte node at leaf level assigning default value to each key and potential key.
		*/
		__byte_node(const value_type& default_value) : _arr{ FSL_COMMA_SEPARATED_STRINGS_256(default_value) } {}

		/**
		*	@brief Returns a value_type reference to the value mapped to given \p key.
		*/
		inline value_type& reference(const key_type& key, const value_type& default_value) {
			(void)default_value;
			return _arr[Byte_Access_Type()(key, BYTE_SIZE - DEPTH)];
		}

		/**
		*	@brief Returns the value_type which is mapped to given \p key.
		*	@details Read-only, does not modify the tree.
		*/
		inline value_type get(const key_type& key, const value_type& default_value) const {
			(void)default_value;
			return _arr[Byte_Access_Type()(key, BYTE_SIZE - DEPTH)];
		}

		~__byte_node() {}


		using value_type_ptr = value_type*;
		using iterator = value_type_ptr;
		using const_iterator = const value_type*;


		inline iterator begin() noexcept { return _arr; }
		inline const_iterator begin() const noexcept { return _arr; }
		inline const_iterator cbegin() const noexcept { return begin(); }

		inline iterator end() noexcept { return _arr + 256; }
		inline const_iterator end() const noexcept { return _arr + 256; }
		inline const_iterator cend() const noexcept { return end(); }

	};



	/**
	*	@brief A complete mapping.
		Every key is mapped to some value. If no value was explicitly set for a key, it is always mapped to the default value.
		Key_Type must have a canonical representation as an array of bytes (uint8_t).
		Creates instances of Value_Type inplace in arrays at the bottom of byte tree.
	*/
	template<class Key_Type, class Value_Type, std::size_t Byte_Size_C = byte_size_t<Key_Type>::value, class Byte_Access_Type = get_byte_t<Key_Type>>
	class byte_tree_map {
	public:
		static_assert(Byte_Size_C > 0, "Byte_Size must be positive.");

		using key_type = Key_Type;
		using value_type = Value_Type;
		static constexpr std::size_t BYTE_SIZE{ Byte_Size_C };
		using byte_access_type = Byte_Access_Type;

	private:

		const value_type _default_value;

		__byte_node<BYTE_SIZE, key_type, value_type, BYTE_SIZE, byte_access_type> _root;

	public:

		/**
		*	@brief Constructs a byte_tree_map with initially all keys mapped to given default value.
		*/
		inline byte_tree_map(const value_type& default_value) : _default_value(default_value), _root(default_value) {}

		/**
		*	@brief Accesses the value for given \p key. If key is mapped to default without any memory allocated for key, it will allocate memory and tree nodes to provide dedicated value_type instance
		*/
		inline value_type& operator[](const key_type& key) {
			return _root.reference(key, _default_value);
		}

		/**
		*	@brief Returns the value mapped to given \p key (read-only)
		*/
		inline value_type get(const key_type& key) const {
			return _root.get(key, _default_value);
		}

	};


}


