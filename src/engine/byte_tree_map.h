#pragma once

template<class T>
struct Default_Byte_Access {
public:

	inline uint8_t operator()(const T& value, const std::size_t& index) const {
		return value.get_byte(index);
	}
};

template<class T>
struct Default_Byte_Size {

	static constexpr std::size_t value{ T::byte_size() };

};

template<class Key_Type, class Value_Type, std::size_t Byte_Size = Default_Byte_Size<Key_Type>::value, class Byte_Access = Default_Byte_Access<Key_Type>>
class byte_tree_map {
public:
	static_assert(Byte_Size > 0, "Byte_Size must be positive.");

	using key_type = Key_Type;
	using value_type = Value_Type;
	static constexpr std::size_t byte_size{ Byte_Size };

	//template<class T>
	//using value_container_type = Value_Container_Type<T>;

	using value_type_ptr = value_type*;

	class iterator {

	};

private:

	template<std::size_t Depth>
	struct byte_node {

		static_assert(Depth > 1);

		static constexpr std::size_t depth{ Depth };

		using sub_type = byte_node<depth - 1>;
		using sub_ptr_type = sub_type*;
		using ptr_array_type = sub_ptr_type[256];

		ptr_array_type arr;

		byte_node(const value_type& default_value) {
			(void)default_value;
			for (uint16_t i{ 0 }; i < 256; ++i) {
				arr[i] = nullptr;
			}
		}

		inline value_type& reference(const key_type& key, const value_type& default_value) {
			sub_ptr_type& sub_ptr{ arr[Byte_Access()(key, byte_size - depth)] };
			if (sub_ptr == nullptr) {
				sub_ptr = new sub_type(default_value);
			}
			return sub_ptr->reference(key, default_value);

		}

		~byte_node() {
			for (uint16_t i{ 0 }; i < 256; ++i) {
				delete arr[i];
			}
		}

	};

	template<>
	struct byte_node<1> {

		static constexpr std::size_t depth{ 1 };

		using value_array_type = value_type[256];
		value_array_type arr;

		byte_node(const value_type& default_value) {
			for (uint16_t i{ 0 }; i < 256; ++i) {
				arr[i] = default_value;
			}
		}

		inline value_type& reference(const key_type& key, const value_type& default_value) {
			(void)default_value;
			return arr[Byte_Access()(key, byte_size - depth)];
		}

		~byte_node() {
		}

		class iterator {
			value_type_ptr _ptr;

			explicit iterator(const value_type_ptr& ptr) : _ptr(ptr) {}
			//friend class byte_node;
		public:
			inline iterator& operator++() { ++_ptr; return *this; }
			inline iterator operator++(int) { iterator c = *this; ++_ptr; return c; }

			inline value_type& operator*() { return *_ptr; }

			inline bool operator==(const iterator& other) const { return _ptr == other._ptr; }
			inline bool operator!=(const iterator& other) const { return _ptr != other._ptr; }
			inline bool operator<(const iterator& other) const { return _ptr < other._ptr; }
		};

		inline iterator begin() { return iterator(&arr[0]); }
		inline iterator end() { return iterator(&arr[0] + 256); }

	};

	const value_type _default_value;

	byte_node<byte_size> _root;

	//std::size_t _count_non_default_values;

public:

	byte_tree_map(const value_type& default_value) : _default_value(default_value), _root(default_value) {
	}

	value_type& operator[](const key_type& key) {
		return _root.reference(key, _default_value);
	}

};

