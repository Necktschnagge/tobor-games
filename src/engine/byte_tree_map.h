#pragma once


template<std::size_t Byte_Size, class Key_Type, class Value_Type, class Byte_Access>
class byte_tree_map {
public:
	static_assert(Byte_Size > 0, "Byte_Size must be positive.");

	using key_type = Key_Type;
	using value_type = Value_Type;
	static constexpr std::size_t byte_size{ Byte_Size };

	//template<class T>
	//using value_container_type = Value_Container_Type<T>;

	using value_type_ptr = Value_Type*;

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
	};

	const value_type default_value;

	byte_node<byte_size> root;

public:

	byte_tree_map(const value_type& default_value) : default_value(default_value), root(default_value) {
	}

	value_type& operator[](const key_type& key) {
		return root.reference(key, default_value);
	}

};

