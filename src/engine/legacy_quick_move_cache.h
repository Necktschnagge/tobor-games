#pragma once




namespace tobor {

	namespace v1_0 {

		/**
		* @brief Class for keeping the information about quick jumps
		*
		* @details For a board, it stores for each cell the information which are the cells to move to on one step [west, east, south, north] until wall, assuming that there are no pieces on the way.
		*/
		template<class World_Type_T>
		class legacy_quick_move_cache {
		public:

			using world_type = World_Type_T;

			using cell_id_int_type = typename world_type::int_type;
			//### does the world type itself check for overflows?

		private:
			const world_type& board;

			std::vector<cell_id_int_type> go_west; // using cell ids
			std::vector<cell_id_int_type> go_east; // using cell ids
			std::vector<cell_id_int_type> go_south; // using cell transposed ids
			std::vector<cell_id_int_type> go_north; // using cell transposed ids

		public:

			/***
				@brief Calculates quick moves for all cells of board. Make sure that reference to \p board stays valid until this is destroyed. Otherwise behavior is undefined.

				@details Make sure that for this cache to be correct, update() needs to be called whenever board is changed.
			*/
			legacy_quick_move_cache(const world_type& board) : board(board) {
				update();
			}

			/**
			* @brief Updates the cache stored by this object. Needs to be called after any change of the board for the cache to be valid.
			*/
			void update() {
				const cell_id_int_type VECTOR_SIZE{ board.count_cells() };

				go_west = std::vector<cell_id_int_type>(VECTOR_SIZE, static_cast<cell_id_int_type>(-1));
				go_east = std::vector<cell_id_int_type>(VECTOR_SIZE, static_cast<cell_id_int_type>(-1));
				go_south = std::vector<cell_id_int_type>(VECTOR_SIZE, static_cast<cell_id_int_type>(-1));
				go_north = std::vector<cell_id_int_type>(VECTOR_SIZE, static_cast<cell_id_int_type>(-1));

				go_west[0] = 0;
				go_south[0] = 0;
				for (cell_id_int_type id = 1; id < VECTOR_SIZE; ++id) {
					if (board.west_wall_by_id(id)) {
						go_west[id] = id;
					}
					else {
						go_west[id] = go_west[id - 1];
					}
					if (board.south_wall_by_transposed_id(id)) {
						go_south[id] = id;
					}
					else {
						go_south[id] = go_south[id - 1];
					}
				}

				go_east[VECTOR_SIZE - 1] = VECTOR_SIZE - 1;
				go_north[VECTOR_SIZE - 1] = VECTOR_SIZE - 1;
				//static_assert(std::is_unsigned<cell_id_int_type>::value, "int type must be unsigned for the following loop to be correct:");
				cell_id_int_type id = VECTOR_SIZE - 1;
				do {
					--id;
					if (board.east_wall_by_id(id)) {
						go_east[id] = id;
					}
					else {
						go_east[id] = go_east[id + 1];
					}
					if (board.north_wall_by_transposed_id(id)) {
						go_north[id] = id;
					}
					else {
						go_north[id] = go_north[id + 1];
					}
				} while (id != 0);
			}

			/**
			* @brief Returns the id of the cell you reach from cell \p id when moving west with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			cell_id_int_type get_west(cell_id_int_type id) const { return go_west[id]; }

			/**
			* @brief Returns the id of the cell you reach from cell \p id when moving east with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			cell_id_int_type get_east(cell_id_int_type id) const { return go_east[id]; }

			/**
			* @brief Returns the transposed id of the cell you reach from cell \p transposed_id when moving south with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			cell_id_int_type get_south(cell_id_int_type transposed_id) const { return go_south[transposed_id]; }

			/**
			* @brief Returns the transposed id of the cell you reach from cell \p transposed_id when moving north with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			cell_id_int_type get_north(cell_id_int_type transposed_id) const { return go_north[transposed_id]; }
		};

	}
}
