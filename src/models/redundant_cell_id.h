#pragma once


namespace tobor {
	namespace v1_0 {

		/**
		*	@brief Kind of iterator to a cell of a board game. Does only store a cell id.
		*
		*	@details This version calculates all three cell id types when set and stores all of them.
					It is the least memory efficient way but may reduce computation time (not yet tested!).
					Reading different kind of ids does not require a game board object.
		*/
		template<class World_Type_T>
		class redundant_cell_id {
		public:

			using world_type = World_Type_T;

			using int_type = typename world_type::int_type;

			using int_cell_id_type = typename world_type::int_type; // pseudo narrow_int
			using int_size_type = typename world_type::int_type; // size_int

			using type = redundant_cell_id;

			/* static factory member functions */

			inline static redundant_cell_id create_by_coordinates(int_type p_x_coord, int_type p_y_coord, const world_type& world) noexcept {
				redundant_cell_id result;
				result.set_coord(p_x_coord, p_y_coord, world);
				return result;
			}

			inline static redundant_cell_id create_by_id(int_type p_id, const world_type& world) noexcept {
				redundant_cell_id result;
				result.set_id(p_id, world);
				return result;
			}

			inline static redundant_cell_id create_by_transposed_id(int_type p_transposed_id, const world_type& world) noexcept {
				redundant_cell_id result;
																					result.set_transposed_id(p_transposed_id, world);
				return result;
			}

		private:

			int_type id;
			int_type transposed_id;
			int_type x_coord;
			int_type y_coord;

		public:

			/* ctors */

			redundant_cell_id() : id(0), transposed_id(0), x_coord(0), y_coord(0) {}

			redundant_cell_id(const redundant_cell_id&) = default;

			redundant_cell_id(redundant_cell_id&&) = default; // needed for static factory member function

			/* operator = */

			inline redundant_cell_id& operator = (const redundant_cell_id&) = default;

			inline redundant_cell_id& operator = (redundant_cell_id&&) = default;

			/* comparison operators */

			inline bool operator < (const redundant_cell_id& other) const noexcept {
				return this->id < other.id;
			}

			inline bool operator == (const redundant_cell_id& other) const noexcept {
				return this->id == other.id;
			}

			/* getter */

			inline int_type get_id() const noexcept { return id; }

			inline int_type get_transposed_id() const noexcept { return transposed_id; }

			inline int_type get_x_coord() const noexcept { return x_coord; }

			inline int_type get_y_coord() const noexcept { return y_coord; }

			/* modifiers */

			inline void set_id(int_type p_id, const world_type& world) noexcept {
				id = p_id;
				world.cell_id_to_coordinates(id, x_coord, y_coord);
				transposed_id = world.coordinates_to_transposed_cell_id(x_coord, y_coord);
			}

			inline void set_transposed_id(int_type p_transposed_id, const world_type& world) noexcept {
				transposed_id = p_transposed_id;
				world.transposed_cell_id_to_coordinates(transposed_id, x_coord, y_coord);
				id = world.coordinates_to_cell_id(x_coord, y_coord);
			}

			inline void set_coord(int_type p_x_coord, int_type p_y_coord, const world_type& world) noexcept {
				x_coord = p_x_coord;
				y_coord = p_y_coord;
				id = world.coordinates_to_cell_id(x_coord, y_coord);
				transposed_id = world.coordinates_to_transposed_cell_id(x_coord, y_coord);
			}

		};

	}
}
