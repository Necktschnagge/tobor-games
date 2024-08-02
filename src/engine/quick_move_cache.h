#pragma once

#include <vector>

namespace tobor {

	namespace v1_1 {

		/**
		* @brief Class for keeping the information about quick jumps
		*
		* @details For a board, it stores for each cell the information which are the cells to move to in one step [west, east, south, north] until wall, assuming that there are no pieces on the way.
		*/
		template<class World_T>
		class quick_move_cache {
		public:

			using world_type = World_T;

			using int_cell_id_type = typename world_type::int_cell_id_type;
			using int_size_type = typename world_type::int_size_type;

		private:
			const world_type& _board;

			std::vector<int_cell_id_type> go_west; // using cell ids
			std::vector<int_cell_id_type> go_east; // using cell ids
			std::vector<int_cell_id_type> go_south; // using transposed cell ids
			std::vector<int_cell_id_type> go_north; // using transposed cell ids

		public:

			/**
			*	@brief Calculates quick moves for all cells of board. Make sure that reference to \p board stays valid until this is destroyed. Otherwise behavior is undefined.
			*
			*	@details Make sure that for this cache to be correct, update() needs to be called whenever board is changed.
			*/
			quick_move_cache(const world_type& board) : _board(board) {
				update();
			}

			inline const world_type& board() const noexcept { return _board; }

			/**
			* @brief Updates the cache stored by this object. Needs to be called after any change of the board for the cache to be valid.
			*/
			void update() {
				const int_size_type VECTOR_SIZE{ _board.count_cells() };

				if (!(VECTOR_SIZE > 0)) {
					return;
				}

				go_west = std::vector<int_cell_id_type>(VECTOR_SIZE, 0);
				go_east = std::vector<int_cell_id_type>(VECTOR_SIZE, 0);
				go_south = std::vector<int_cell_id_type>(VECTOR_SIZE, 0);
				go_north = std::vector<int_cell_id_type>(VECTOR_SIZE, 0);

				{
					go_west[0] = 0;
					go_south[0] = 0;

					int_cell_id_type id = 0;
					while (static_cast<int_size_type>(id) + 1 < VECTOR_SIZE) {
						++id;
						if (_board.west_wall_by_id(id)) {
							go_west[id] = id;
						}
						else {
							go_west[id] = go_west[id - 1];
						}
						if (_board.south_wall_by_transposed_id(id)) {
							go_south[id] = id;
						}
						else {
							go_south[id] = go_south[id - 1];
						}
					}
				}
				{
					go_east[VECTOR_SIZE - 1] = static_cast<int_cell_id_type>(VECTOR_SIZE - 1);
					go_north[VECTOR_SIZE - 1] = static_cast<int_cell_id_type>(VECTOR_SIZE - 1);

					int_cell_id_type id{ static_cast<int_cell_id_type>(VECTOR_SIZE - 1) };
					do {
						--id;
						if (_board.east_wall_by_id(id)) {
							go_east[id] = id;
						}
						else {
							go_east[id] = go_east[id + 1];
						}
						if (_board.north_wall_by_transposed_id(id)) {
							go_north[id] = id;
						}
						else {
							go_north[id] = go_north[id + 1];
						}
					} while (id != 0);
				}
			}

			/**
			* @brief Returns the id of the cell you reach from cell \p id when moving west with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, _board.count_cells() - 1 ]
			*/
			inline int_cell_id_type get_west(int_cell_id_type id) const { return go_west[id]; }

			/**
			* @brief Returns the id of the cell you reach from cell \p id when moving east with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, _board.count_cells() - 1 ]
			*/
			inline int_cell_id_type get_east(int_cell_id_type id) const { return go_east[id]; }

			/**
			* @brief Returns the transposed id of the cell you reach from cell \p transposed_id when moving south with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, _board.count_cells() - 1 ]
			*/
			inline int_cell_id_type get_south(int_cell_id_type transposed_id) const { return go_south[transposed_id]; }

			/**
			* @brief Returns the transposed id of the cell you reach from cell \p transposed_id when moving north with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, _board.count_cells() - 1 ]
			*/
			inline int_cell_id_type get_north(int_cell_id_type transposed_id) const { return go_north[transposed_id]; }

			/**
			*	@brief Returns the raw id of the cell you reach from cell \p id when moving in direction \p d, assuming there are no pieces on the way.
			*
			*	@details You must pass a tranposed id as raw id for north/south and an id for east/west.
			*		The value returned is also a transposed id or an id respectively.
			*		Has undefined behavior if \p id is out of range. Valid range is [ 0, _board.count_cells() - 1 ].
			*/
			inline int_cell_id_type get(const direction& d, int_cell_id_type raw_id) const {
				switch (d.get())
				{
				case direction::encoding::NORTH:
					return get_north(raw_id);
				case direction::encoding::EAST:
					return get_east(raw_id);
				case direction::encoding::SOUTH:
					return get_south(raw_id);
				case direction::encoding::WEST:
					return get_west(raw_id);
				default:
					return raw_id;
				}
			}
		};

	}
}
