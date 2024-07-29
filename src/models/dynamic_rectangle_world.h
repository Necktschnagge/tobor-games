#pragma once

#include "wall.h"
#include "errors.h"

#include <stdexcept>
#include <algorithm>

namespace tobor {
	namespace v1_1 {

		/**
		*
		*	@brief Represents the game's entire board, stating which cells are separated by walls.
		*	@details Does NOT contain any information about where pieces are located.
		*
		*/
		template <class Int_Size_Type_T, class Int_Cell_Id_Type_T>
		class dynamic_rectangle_world {
		public:

			using int_size_type = Int_Size_Type_T;
			using int_cell_id_type = Int_Cell_Id_Type_T;

			using wall_vector_type = wall_vector;
			using type = dynamic_rectangle_world;

		private:

			int_size_type x_size;
			int_size_type y_size;

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

			static constexpr int_cell_id_type narrow(const int_size_type& x) {
				return static_cast<int_cell_id_type>(x);
			}

			static constexpr int_size_type wide(const int_cell_id_type& x) {
				return static_cast<int_size_type>(x);
			}

			/* ctors et. al. **************************************************************************************/

			dynamic_rectangle_world() : x_size(0), y_size(0) {}

			dynamic_rectangle_world(const int_size_type& x_size, const int_size_type& y_size) : dynamic_rectangle_world() {
				resize(x_size, y_size);
			}

			/**
			*	@brief Sets the size of the world, also creates an empty rectangle with walls only on the outer borders
			*/
			void resize(int_size_type _x_size, int_size_type _y_size) noexcept {
				this->x_size = _x_size;
				this->y_size = _y_size;
				{
					const wall_vector_type::size_type SIZE{ static_cast<std::size_t>(x_size) * y_size + 1 };

					h_walls = wall_vector_type(SIZE, false);
					v_walls = wall_vector_type(SIZE, false);
				}

				for (int_size_type i = 0; i <= x_size; ++i) { // set north and south walls
					h_walls[y_size * i] = true;
				}
				for (int_size_type i = 0; i <= y_size; ++i) { // set east and west walls
					v_walls[x_size * i] = true;
				}
			}

			/*
				@brief Fills center cells to make them unreachable.
			*/
			inline void block_center_cells(int_size_type x_blocked_size, int_size_type y_blocked_size) {
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

				const int_size_type x_begin{ static_cast<int_size_type>((x_size - x_blocked_size) / 2) };
				const int_size_type x_end{ static_cast<int_size_type>(x_begin + x_blocked_size) };
				const int_size_type y_begin{ static_cast<int_size_type>((y_size - y_blocked_size) / 2) };
				const int_size_type y_end{ static_cast<int_size_type>(y_begin + y_blocked_size) };

				for (auto y = y_begin; y != y_end; ++y) {
					for (auto x = x_begin; x != x_end; ++x) {
						const int_cell_id_type x_narrow{ narrow(x) };
						const int_cell_id_type y_narrow{ narrow(y) };

						const int_cell_id_type id = coordinates_to_cell_id(x_narrow, y_narrow);
						const int_cell_id_type transposed_id = coordinates_to_transposed_cell_id(x_narrow, y_narrow);

						north_wall_by_transposed_id(transposed_id) = true;
						east_wall_by_id(id) = true;
						south_wall_by_transposed_id(transposed_id) = true;
						west_wall_by_id(id) = true;
						// remark: this is by the way inefficient, but readable
					}
				}
			}

			/* cell id conversion **************************************************************************************/

			inline constexpr int_cell_id_type coordinates_to_cell_id(int_cell_id_type x_coord, int_cell_id_type y_coord) const noexcept {
				return narrow(x_size * y_coord + x_coord);
			}

			inline constexpr int_cell_id_type coordinates_to_transposed_cell_id(int_cell_id_type x_coord, int_cell_id_type y_coord) const noexcept {
				return narrow(y_size * x_coord + y_coord);
			}

			inline constexpr std::pair<int_cell_id_type, int_cell_id_type> cell_id_to_coordinates(int_cell_id_type id) const noexcept {
				return std::make_pair(id % x_size, id / x_size);
			}

			inline constexpr void cell_id_to_coordinates(int_cell_id_type id, int_cell_id_type& x_coord, int_cell_id_type& y_coord) const noexcept {
				x_coord = id % x_size;
				y_coord = id / x_size;
			}

			inline constexpr std::pair<int_cell_id_type, int_cell_id_type> transposed_cell_id_to_coordinates(int_cell_id_type transposed_id) const noexcept {
				return std::make_pair(transposed_id / y_size, transposed_id % y_size);
			}

			inline constexpr void transposed_cell_id_to_coordinates(int_cell_id_type transposed_id, int_cell_id_type& x_coord, int_cell_id_type& y_coord) const noexcept {
				x_coord = transposed_id / y_size;
				y_coord = transposed_id % y_size;
			}

			inline constexpr int_cell_id_type cell_id_to_transposed_cell_id(int_cell_id_type id) const noexcept {
				return narrow(y_size * (id % x_size) + id / x_size);
			}

			inline constexpr int_cell_id_type transposed_cell_id_to_cell_id(int_cell_id_type transposed_id) const noexcept {
				return narrow(x_size * (transposed_id % y_size) + transposed_id / y_size);
			}

			/* wall accessors **************************************************************************************/

			inline wall& south_wall_by_transposed_id(int_cell_id_type transposed_id) noexcept {
				return h_walls[transposed_id];
			}

			inline const wall& south_wall_by_transposed_id(int_cell_id_type transposed_id) const noexcept {
				return h_walls[transposed_id];
			}

			inline wall& north_wall_by_transposed_id(int_cell_id_type transposed_id) noexcept {
				return h_walls[wide(transposed_id) + 1];
			}

			inline const wall& north_wall_by_transposed_id(int_cell_id_type transposed_id) const noexcept {
				return h_walls[wide(transposed_id) + 1];
			}

			inline wall& west_wall_by_id(int_cell_id_type id) noexcept {
				return v_walls[id];
			}

			inline const wall& west_wall_by_id(int_cell_id_type id) const noexcept {
				return v_walls[id];
			}

			inline wall& east_wall_by_id(int_cell_id_type id) noexcept {
				return v_walls[wide(id) + 1];
			}

			inline const wall& east_wall_by_id(int_cell_id_type id) const noexcept {
				return v_walls[wide(id) + 1];
			}

			/* getter **************************************************************************************/

			/**
			*	@brief Returns the board's number of cells, including totally blocked ones.
			*/
			inline int_size_type count_cells() const noexcept {
				return x_size * y_size;
			}

			/**
			*	@brief Return true if and only if given cell is blocked.
			*	@details cell_id must be a non-negative integer less than \a count_cells(). Otherwise behaviour is undefined.
			*/
			inline bool blocked(int_cell_id_type cell_id) const {
				auto transposed_id = cell_id_to_transposed_cell_id(cell_id);
				return west_wall_by_id(cell_id) && east_wall_by_id(cell_id) && south_wall_by_transposed_id(transposed_id) && north_wall_by_transposed_id(transposed_id);
			}

			/**
			*	@brief Returns the board's number of blocked cells
			*/
			inline int_size_type blocked_cells() const noexcept {
				int_size_type counter{ 0 };
				for (int_size_type cell_id = 0; cell_id < count_cells(); ++cell_id) {
					counter += blocked(narrow(cell_id));
				}
				return counter;
			}

			/**
			*	@brief Returns the board's number of cells in a row.
			*/
			inline int_size_type get_horizontal_size() const noexcept {
				return x_size;
			}

			/**
			*	@brief Returns the board's number of cells in a column.
			*/
			inline int_size_type get_vertical_size() const noexcept {
				return y_size;
			}

			type turn_left_90() const { // only for quadratic
				if (x_size != y_size) {
					throw std::logic_error("Cannot turn for non-quadratic board.");
				}
				auto copy = type(x_size, y_size);

				for (int_size_type big_id = 0; big_id < x_size; ++big_id) {
					for (int_size_type little_id = 0; little_id < x_size; ++little_id) {
						copy.v_walls[big_id * x_size + little_id] = h_walls[big_id * x_size + (x_size - little_id)];
						copy.h_walls[big_id * x_size + little_id] = v_walls[((x_size - 1) - big_id) * x_size + little_id];
					}
				}
				return copy;
			}

		};

		using default_dynamic_rectangle_world = dynamic_rectangle_world<std::size_t, std::size_t>;
	}
}
