#pragma once

#include "errors.h"
#include "wall.h"

namespace tobor {
	namespace v1_0 {

		/**
		*	@brief Represents the game's entire board, stating which cells are separated by walls.
		*
		*	@details Does NOT contain any information about where pieces are located.
		*/
		template <class Int_Type_T>
		class legacy_world {
		public:

			using int_type = Int_Type_T;
			using wall_vector_type = wall_vector;
			using type = legacy_world;

		private:

			int_type x_size;
			int_type y_size;

			/*
			* Contains all horizontal walls existing on the board.
			* order: start at (0,0) -> (0, y_size - 1), go line by line x=0 .. x_size - 1
			* At index i of this vector h_walls[i] you find the horizontal wall below the cell with transposed_id i.
			*/
			wall_vector_type h_walls;

			/*
			* Contains all vertical walls existing on the board.
			* order: start at (0,0) -> (x_size - 1, 0), go line by line y=0 .. y_size - 1
			* At index i of this vector v_walls[i] you find the vertical wall on the left of the cell with id i.
			*/
			wall_vector_type v_walls;

		public:

			/* ctors et. al. **************************************************************************************/

			legacy_world() : x_size(0), y_size(0) {}

			legacy_world(const int_type& x_size, const int_type& y_size) : legacy_world() {
				resize(x_size, y_size);
			}

			/**
			*	@brief Sets the size of the world, also creates an empty rectangle with walls only on the outer borders
			*/
			void resize(int_type _x_size, int_type _y_size) noexcept {
				this->x_size = _x_size;
				this->y_size = _y_size;
				{
					const wall_vector_type::size_type SIZE{ static_cast<std::size_t>(x_size) * y_size + 1 };

					h_walls = wall_vector_type(SIZE, false);
					v_walls = wall_vector_type(SIZE, false);
				}

				for (int_type i = 0; i <= x_size; ++i) { // set north and south walls
					h_walls[y_size * i] = true;
				}
				for (int_type i = 0; i <= y_size; ++i) { // set east and west walls
					v_walls[x_size * i] = true;
				}
			}

			/*
				@brief Fills center cells to make them unreachable.
			*/
			inline void block_center_cells(int_type x_blocked_size, int_type y_blocked_size) {
				// check for symmetry of blocked area:
				if ((x_size - x_blocked_size) % 2)
					throw division_by_2_error();
				if ((y_size - y_blocked_size) % 2)
					throw division_by_2_error();

				// check for non-blocked cells in every direction
				if (x_blocked_size >= x_size)
					throw blocked_center_error();
				if (y_blocked_size >= y_size)
					throw blocked_center_error();

				const int_type x_begin{ static_cast<int_type>((x_size - x_blocked_size) / 2) };
				const int_type x_end{ static_cast<int_type>(x_begin + x_blocked_size) };
				const int_type y_begin{ static_cast<int_type>((y_size - y_blocked_size) / 2) };
				const int_type y_end{ static_cast<int_type>(y_begin + y_blocked_size) };

				for (auto y = y_begin; y != y_end; ++y) {
					for (auto x = x_begin; x != x_end; ++x) {
						const int_type id = coordinates_to_cell_id(x, y);
						const int_type transposed_id = coordinates_to_transposed_cell_id(x, y);

						north_wall_by_transposed_id(transposed_id) = true;
						east_wall_by_id(id) = true;
						south_wall_by_transposed_id(transposed_id) = true;
						west_wall_by_id(id) = true;
						// remark: this is by the way inefficient, but readable
					}
				}
			}

			/* cell id conversion **************************************************************************************/

			inline constexpr int_type coordinates_to_cell_id(int_type x_coord, int_type y_coord) const noexcept {
				return x_size * y_coord + x_coord;
			}

			inline constexpr int_type coordinates_to_transposed_cell_id(int_type x_coord, int_type y_coord) const noexcept {
				return y_size * x_coord + y_coord;
			}

			inline constexpr std::pair<int_type, int_type> cell_id_to_coordinates(int_type id) const noexcept {
				return std::make_pair(id % x_size, id / x_size);
			}

			inline constexpr void cell_id_to_coordinates(int_type id, int_type& x_coord, int_type& y_coord) const noexcept {
				x_coord = id % x_size;
				y_coord = id / x_size;
			}

			inline constexpr std::pair<int_type, int_type> transposed_cell_id_to_coordinates(int_type transposed_id) const noexcept {
				return std::make_pair(transposed_id / y_size, transposed_id % y_size);
			}

			inline constexpr void transposed_cell_id_to_coordinates(int_type transposed_id, int_type& x_coord, int_type& y_coord) const noexcept {
				x_coord = transposed_id / y_size;
				y_coord = transposed_id % y_size;
			}

			inline constexpr int_type id_to_transposed_id(int_type id) const noexcept {
				return y_size * (id % x_size) + id / x_size;
			}

			inline constexpr int_type transposed_id_to_id(int_type transposed_id) const noexcept {
				return x_size * (transposed_id % y_size) + transposed_id / y_size;
			}

			/* wall accessors **************************************************************************************/

			inline wall& south_wall_by_transposed_id(int_type transposed_id) noexcept {
				return h_walls[transposed_id];
			}

			inline const wall& south_wall_by_transposed_id(int_type transposed_id) const noexcept {
				return h_walls[transposed_id];
			}

			inline wall& north_wall_by_transposed_id(int_type transposed_id) noexcept {
				return h_walls[transposed_id + 1];
			}

			inline const wall& north_wall_by_transposed_id(int_type transposed_id) const noexcept {
				return h_walls[transposed_id + 1];
			}

			inline wall& west_wall_by_id(int_type id) noexcept {
				return v_walls[id];
			}

			inline const wall& west_wall_by_id(int_type id) const noexcept {
				return v_walls[id];
			}

			inline wall& east_wall_by_id(int_type id) noexcept {
				return v_walls[id + 1];
			}

			inline const wall& east_wall_by_id(int_type id) const noexcept {
				return v_walls[id + 1];
			}

			/* getter **************************************************************************************/

			/**
			*	@brief Returns the board's number of cells, including totally blocked ones.
			*/
			inline int_type count_cells() const noexcept {
				return x_size * y_size;
			}

			/**
			*	@brief Return true if and only if given cell is blocked.
			*	@details cell_id must be a non-negative integer less than \a count_cells(). Otherwise behaviour is undefined.
			*/
			inline bool blocked(int_type cell_id) const {
				auto transposed_id = id_to_transposed_id(cell_id);
				return west_wall_by_id(cell_id) && east_wall_by_id(cell_id) && south_wall_by_transposed_id(transposed_id) && north_wall_by_transposed_id(transposed_id);
			}

			/**
			*	@brief Returns the board's number of blocked cells
			*/
			inline int_type blocked_cells() const noexcept {
				int_type counter{ 0 };
				for (int_type cell_id = 0; cell_id < count_cells(); ++cell_id) {
					counter += blocked(cell_id);
				}
				return counter;
			}

			/**
			*	@brief Returns the board's number of cells in a row.
			*/
			inline int_type get_horizontal_size() const noexcept {
				return x_size;
			}

			/**
			*	@brief Returns the board's number of cells in a column.
			*/
			inline int_type get_vertical_size() const noexcept {
				return y_size;
			}

			type turn_left_90() const { // only for quadratic
				if (x_size != y_size) {
					throw std::logic_error("Cannot turn for non-quadratic world.");
				}
				auto copy = type(x_size, y_size);

				for (int_type big_id = 0; big_id < x_size; ++big_id) {
					for (int_type little_id = 0; little_id < x_size; ++little_id) {
						copy.v_walls[big_id * x_size + little_id] = h_walls[big_id * x_size + (x_size - little_id)];
						copy.h_walls[big_id * x_size + little_id] = v_walls[((x_size - 1) - big_id) * x_size + little_id];
					}
				}
				return copy;
			}

		};

	}
}
