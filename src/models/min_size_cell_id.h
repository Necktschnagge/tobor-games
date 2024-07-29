#pragma once



namespace tobor {
	namespace v1_1 {

		/**
		*	@brief Stores just a cell id.
		*/
		template<class World_T>
		class min_size_cell_id {
		public:

			using world_type = World_T;

			using int_size_type = typename world_type::int_size_type; // narrow_int
			using int_cell_id_type = typename world_type::int_cell_id_type; // size_int

			using type = min_size_cell_id<world_type>;

			/* static factory member functions */

			inline static type create_by_coordinates(int_cell_id_type p_x_coord, int_cell_id_type p_y_coord, const world_type& world) noexcept {
				return type(world.coordinates_to_cell_id(p_x_coord, p_y_coord));
			}

			inline static type create_by_id(int_cell_id_type p_id) noexcept {
				return type(p_id);
			}

			inline static type create_by_id(int_cell_id_type p_id, const world_type&) noexcept {
				return create_by_id(p_id);
			}

			inline static type create_by_transposed_id(int_cell_id_type p_transposed_id, const world_type& world) noexcept {
				return type(world.transposed_cell_id_to_cell_id(p_transposed_id));
			}

			inline static type create_by_raw_id(const id_polarisation& p, int_cell_id_type p_raw_id, const world_type& world) noexcept {
				if (p) return create_by_transposed_id(p_raw_id, world);
				else return create_by_id(p_raw_id);
			}

		private:

			int_cell_id_type _id;

		public:

			/* ctors */

			min_size_cell_id() : _id(0) {}

			min_size_cell_id(int_cell_id_type id) : _id(id) {}

			min_size_cell_id(const min_size_cell_id&) = default;

			min_size_cell_id(min_size_cell_id&&) = default; // needed for static factory member function

			/* operator = */

			inline min_size_cell_id& operator = (const min_size_cell_id&) = default;

			inline min_size_cell_id& operator = (min_size_cell_id&&) = default;

			/* comparison operators */

			inline std::strong_ordering operator<=>(const min_size_cell_id& another) const = default;

			/* getter */

			inline int_cell_id_type get_id() const noexcept { return _id; }

			inline int_cell_id_type get_id(const world_type&) const noexcept { return _id; }

			inline int_cell_id_type get_transposed_id(const world_type& world) const noexcept { return world.cell_id_to_transposed_cell_id(_id); }

			inline int_cell_id_type get_x_coord(const world_type& world) const noexcept { return world.cell_id_to_coordinates(_id).first; }

			inline int_cell_id_type get_y_coord(const world_type& world) const noexcept { return world.cell_id_to_coordinates(_id).second; }

			inline int_cell_id_type get_raw_id(const id_polarisation& p, const world_type& world) const noexcept {
				if (p) return get_transposed_id(world);
				else return get_id();
			}

			/* modifiers */

			inline void set_id(int_cell_id_type p_id) noexcept {
				_id = p_id;
			}

			inline void set_id(int_cell_id_type p_id, const world_type&) noexcept {
				return set_id(p_id);
			}

			inline void set_transposed_id(int_cell_id_type p_transposed_id, const world_type& world) noexcept {
				_id = world.transposed_cell_id_to_cell_id(p_transposed_id);
			}

			inline void set_coord(int_cell_id_type p_x_coord, int_cell_id_type p_y_coord, const world_type& world) noexcept {
				_id = world.coordinates_to_cell_id(p_x_coord, p_y_coord);
			}

		};

	}
}