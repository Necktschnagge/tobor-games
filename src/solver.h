#pragma once

#include "models.h"

#include <map>
#include <array>
#include <algorithm>


namespace tobor {

	namespace v1_0 {

#if false
		/**
		*	@brief Stores the cell ids which can be reached by moving in one of the four directions,
					assuming moving as far as possible until hitting a wall,
					also assuming there are no other obstacles than walls on the way.
					Should be used as one object per cell.
		*/
		class [[deprecated]] quick_moves_of_cell {

			using cell_id = universal_cell_id;

			using game_board = tobor_world;

		public:
			cell_id next_north; // ### cannot be const, because of ctor, should be only const-accessable!
			cell_id next_east;
			cell_id next_south;
			cell_id next_west;

			quick_moves_of_cell(const cell_id& start_field, const game_board& world) {
				// ### inefficient: try to use some recursive call on next east of left neighbour cell, if next_east is not equal to start cell
				// east, west - vwalls - id
				std::size_t next_west_id = start_field.get_id();
				while (!world.west_wall_by_id(next_west_id)) {
					--next_west_id;
				}
				next_west.set_id(next_west_id, world);

				std::size_t next_east_id = start_field.get_id();
				while (!world.east_wall_by_id(next_east_id)) {
					++next_east_id;
				}
				next_east.set_id(next_east_id, world);

				// north south - hwalls - transposed_id
				std::size_t next_south_transposed_id = start_field.get_transposed_id();
				while (!world.south_wall_by_transposed_id(next_south_transposed_id)) {
					--next_south_transposed_id;
				}
				next_south.set_transposed_id(next_south_transposed_id, world);

				std::size_t next_north_transposed_id = start_field.get_transposed_id();
				while (!world.north_wall_by_transposed_id(next_north_transposed_id)) {
					++next_north_transposed_id;
				}
				next_north.set_transposed_id(next_north_transposed_id, world);

			}
		};


		/**
		*	@brief Stores the quick move information for all cells of the game's board.
		*/
		class [[deprecated]] quick_move_board {
		public:
			using cell_id = universal_cell_id;
			using game_board = tobor_world;

			// maps:   id |-> quick_moves_of_cell of cell with given id
			std::vector<quick_moves_of_cell> cells;

			quick_move_board() {}

			quick_move_board(const game_board& world) {
				cells.reserve(world.count_cells());
				for (std::size_t id = 0; id < world.count_cells(); ++id) {
					cell_id cell;
					cell.set_id(id, world);

					cells.emplace_back(quick_moves_of_cell(cell, world));
				}
			}
		};
#endif

		// ### alternative board representation is just two sorted vectors of indices where there are walls, additionally you can uniformly encode pieces like walls, in this case double walls left and right, or top and bottom.


		/**
		* @brief Class for keeping the information about quick jumps
		*/
		class quick_move_cache {

			using game_board = tobor_world;

		private:
			const game_board& board;

			std::vector<std::size_t> go_west; // using cell ids
			std::vector<std::size_t> go_east; // using cell ids
			std::vector<std::size_t> go_south; // using cell transposed ids
			std::vector<std::size_t> go_north; // using cell transposed ids

		public:

			/***
				@brief Calculates quick moves for all cells of board. Make sure that reference to \p board stays valid until this is destroyed. Otherwise behavior is undefined.

				@details Make sure that for this cache to be correct, update() needs to be called whenever board is changed.
			*/
			quick_move_cache(const game_board& board) : board(board) {
				update();
			}

			/**
			* @brief Updates the cache stored by this object. Needs to be called after any change of the board for the cache to be valid.
			*/
			void update() {
				const std::size_t VECTOR_SIZE{ board.count_cells() };

				go_west = std::vector<std::size_t>(VECTOR_SIZE, static_cast<std::size_t>(-1));
				go_east = std::vector<std::size_t>(VECTOR_SIZE, static_cast<std::size_t>(-1));
				go_south = std::vector<std::size_t>(VECTOR_SIZE, static_cast<std::size_t>(-1));
				go_north = std::vector<std::size_t>(VECTOR_SIZE, static_cast<std::size_t>(-1));

				go_west[0] = 0;
				go_south[0] = 0;
				for (std::size_t id = 1; id < VECTOR_SIZE; ++id) {
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
				for (std::size_t id = VECTOR_SIZE - 2; id != static_cast<std::size_t>(-1); --id) {
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
				}
			}

			std::size_t get_west(std::size_t id) const { return go_west[id]; }
			std::size_t get_east(std::size_t id) const { return go_east[id]; }
			std::size_t get_south(std::size_t transposed_id) const { return go_south[transposed_id]; }
			std::size_t get_north(std::size_t transposed_id) const { return go_north[transposed_id]; }
		};

		/**
		*	@brief Calculates successor cells of one pice to move at a time.
		*/
		template<std::size_t COUNT_NON_TARGET_PIECES>
		class move_one_piece_calculator {
		public:
			using world_type = tobor_world;

		private:
			const world_type& my_world;

			quick_move_cache cache;

		public:

			move_one_piece_calculator(const world_type& my_world) : my_world(my_world), cache(my_world) {
			}

			// put the following four functions together using some if constexpr. Use the direction as template parameter. ####
			// it may be better to not return a bool, just return the start cell itself if no move happens. (?)

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving west until obstacle.
			*/
			inline std::pair<universal_cell_id, bool> get_next_field_on_west_move(const universal_cell_id& start_cell, const positions_of_pieces<COUNT_NON_TARGET_PIECES>& state) {
				std::size_t next_cell{ cache.get_west(start_cell.get_id()) };

				if (start_cell.get_id() == next_cell) { // no move possible by walls
					return std::make_pair(universal_cell_id::create_by_id(next_cell, my_world), false);
				}

				// iterate over all pieces since they may appear as obstacle

				if (next_cell <= state.target_piece.get_id() && state.target_piece.get_id() < start_cell.get_id()) { // target piece
					next_cell = state.target_piece.get_id() + 1;
				}

				for (std::size_t i = 0; i < COUNT_NON_TARGET_PIECES; ++i) { // non target pieces
						if (next_cell <= state.non_target_pieces[i].get_id() && state.non_target_pieces[i].get_id() < start_cell.get_id()) { // target piece
							next_cell = state.non_target_pieces[i].get_id() + 1;
						}
				}

				if (start_cell.get_id() == next_cell) { // no move possible by walls
					return std::make_pair(universal_cell_id::create_by_id(next_cell, my_world), false);
				}

				return std::make_pair(universal_cell_id::create_by_id(next_cell, my_world), true);
			}

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving east until obstacle.
			*/
			inline std::pair<universal_cell_id, bool> get_next_field_on_east_move(const universal_cell_id& start_cell, const positions_of_pieces< COUNT_NON_TARGET_PIECES>& state) {
				
				//decltype(universal_cell_id::get_id) universal_cell_id::* get_id_type = &universal_cell_id::get_id;
				//decltype(quick_move_cache::get_west) quick_move_cache::* next_cache_direction = &quick_move_cache::get_east;

				std::size_t next_cell{ cache.get_east(start_cell.get_id()) };

				if (start_cell.get_id() == next_cell) { // no move possible by walls
					return std::make_pair(universal_cell_id::create_by_id(next_cell, my_world), false);
				}

				// iterate over all pieces since they may appear as obstacle

				if (start_cell.get_id() < state.target_piece.get_id() && state.target_piece.get_id() <= next_cell) { // target piece
					next_cell = state.target_piece.get_id() - 1;
				}

				for (std::size_t i = 0; i < COUNT_NON_TARGET_PIECES; ++i) { // non target pieces
					if (start_cell.get_id() < state.non_target_pieces[i].get_id() && state.non_target_pieces[i].get_id() <= next_cell) { // target piece
						next_cell = state.non_target_pieces[i].get_id() - 1;
					}
				}

				if (start_cell.get_id() == next_cell) { // no move possible by walls
					return std::make_pair(universal_cell_id::create_by_id(next_cell, my_world), false);
				}

				return std::make_pair(universal_cell_id::create_by_id(next_cell, my_world), true);
			}

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving south until obstacle.
			*/
			inline std::pair<universal_cell_id, bool> get_next_field_on_south_move(const universal_cell_id& start_cell, const positions_of_pieces< COUNT_NON_TARGET_PIECES>& state) {
				std::size_t next_cell{ cache.get_south(start_cell.get_transposed_id()) };

				if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
					return std::make_pair(universal_cell_id::create_by_transposed_id(next_cell, my_world), false);
				}

				// iterate over all pieces since they may appear as obstacle

				if (next_cell <= state.target_piece.get_transposed_id() && state.target_piece.get_transposed_id() < start_cell.get_transposed_id()) { // target piece
					next_cell = state.target_piece.get_transposed_id() + 1;
				}

				for (std::size_t i = 0; i < COUNT_NON_TARGET_PIECES; ++i) { // non target pieces
					if (next_cell <= state.non_target_pieces[i].get_transposed_id() && state.non_target_pieces[i].get_transposed_id() < start_cell.get_transposed_id()) { // target piece
						next_cell = state.non_target_pieces[i].get_transposed_id() + 1;
					}
				}

				if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
					return std::make_pair(universal_cell_id::create_by_transposed_id(next_cell, my_world), false);
				}

				return std::make_pair(universal_cell_id::create_by_transposed_id(next_cell, my_world), true);
			}

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving north until obstacle.
			*/
			inline std::pair<universal_cell_id, bool> get_next_field_on_north_move(const universal_cell_id& start_cell, const positions_of_pieces<  COUNT_NON_TARGET_PIECES>& state) {
				std::size_t next_cell{ cache.get_north(start_cell.get_transposed_id()) };

				if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
					return std::make_pair(universal_cell_id::create_by_transposed_id(next_cell, my_world), false);
				}

				// iterate over all pieces since they may appear as obstacle

				if (start_cell.get_transposed_id() < state.target_piece.get_transposed_id() && state.target_piece.get_transposed_id() <= next_cell) { // target piece
					next_cell = state.target_piece.get_transposed_id() - 1;
				}

				for (std::size_t i = 0; i < COUNT_NON_TARGET_PIECES; ++i) { // non target pieces
					if (start_cell.get_transposed_id() < state.non_target_pieces[i].get_transposed_id() && state.non_target_pieces[i].get_transposed_id() <= next_cell) { // target piece
						next_cell = state.non_target_pieces[i].get_transposed_id() - 1;
					}
				}

				if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
					return std::make_pair(universal_cell_id::create_by_transposed_id(next_cell, my_world), false);
				}

				return std::make_pair(universal_cell_id::create_by_transposed_id(next_cell, my_world), true);
			}

		};


		/*
		*	@brief Equivalent to a pair of a piece_id and a direction where to move it.
		*/
		class piece_move {
		public:
			using piece_id_type = uint8_t;
			using move_direction_type = uint8_t;

			static constexpr move_direction_type NORTH{ 1 << 0 };
			static constexpr move_direction_type EAST{ 1 << 1 };
			static constexpr move_direction_type SOUTH{ 1 << 2 };
			static constexpr move_direction_type WEST{ 1 << 3 };

			piece_id_type piece_id;
			move_direction_type direction;

			piece_move(const piece_id_type& _piece_id, const move_direction_type& _direction) : piece_id(_piece_id), direction(_direction) {}
		};



		template <std::size_t COUNT_NON_TARGET_PIECES>
		class partial_solution_connections {
		public:
			using cell_id = universal_cell_id;
			using State_Type = positions_of_pieces<COUNT_NON_TARGET_PIECES>;


			using partial_solutions_map_type = std::map<positions_of_pieces<COUNT_NON_TARGET_PIECES>, partial_solution_connections>;
			using map_iterator_type = typename partial_solutions_map_type::iterator;

			static constexpr std::size_t MAX{ std::numeric_limits<std::size_t>::max() };

			std::vector<std::tuple<map_iterator_type, piece_move>> predecessors;
			std::size_t steps{ MAX };
			std::size_t count_successors{ 0 }; // is leaf iff == 0
		};

		class move_candidate {
		public:
			piece_move move;
			std::pair<universal_cell_id, bool> next_field_paired_enable;

			move_candidate(const piece_move& m, const std::pair<universal_cell_id, bool>& n) : move(m), next_field_paired_enable(n) {}
		};

		template<std::size_t COUNT_NON_TARGET_PIECES = 3>
		inline std::size_t get_all_optimal_solutions(
			move_one_piece_calculator<COUNT_NON_TARGET_PIECES>& move_one_piece_c,
			const universal_cell_id& p_target_cell,
			const universal_cell_id& p_target_piece,
			std::array<universal_cell_id, COUNT_NON_TARGET_PIECES>&& p_non_target_pieces
		) {

			// to be used as a pointer to a game state
			using map_iterator = typename partial_solution_connections<COUNT_NON_TARGET_PIECES>::map_iterator_type;


			const auto initial_state = positions_of_pieces<COUNT_NON_TARGET_PIECES>(p_target_piece, std::move(p_non_target_pieces));


			// all game states that have been found yet,
			// each one with pointers to their predecessors as well as a counter for successors.
			typename partial_solution_connections<COUNT_NON_TARGET_PIECES>::partial_solutions_map_type solutions_map;

			// All game states that have been found yet, ordered by their shortest distance from initial state.
			// .back() contains all game states to be explored if one deepening step just finished.
			std::vector<std::vector<map_iterator>> visited_game_states;

			// number of steps needed by any optimal solution
			std::size_t optimal_solution_size{ std::numeric_limits<std::size_t>::max() };


			/*	insert the initial game state into map of all visited states : */
			solutions_map[initial_state].steps = 0; // insert initial_state
			// solutions_map[initial_state].predecessors.clear(); // already empty be default
			// solutions_map[initial_state].count_successors = 0; // already empty be default

			/*	insert the initial game state into vector of all visited states : */
			visited_game_states.push_back(std::vector<map_iterator>{solutions_map.begin()});


			// what if initial state is already final? check this! ####

			for (std::size_t expand_size{ 0 }; expand_size < optimal_solution_size; ++expand_size) {
				visited_game_states.emplace_back(); // possibly invalidates iterators on sub-vectors, but on most compilers it will work anyway.
				// But please do not rely on this behaviour.

				for (const auto& current_iterator : visited_game_states[expand_size]) {
					//const auto current_iterator{ visited_game_states[index_next_exploration] };

					std::vector<move_candidate> candidates_for_successor_states;

					// get next fields in our world with respect to current state
					candidates_for_successor_states.emplace_back(
						piece_move(COUNT_NON_TARGET_PIECES, piece_move::WEST),
						move_one_piece_c.get_next_field_on_west_move(current_iterator->first.target_piece, current_iterator->first)
					);
					candidates_for_successor_states.emplace_back(
						piece_move(COUNT_NON_TARGET_PIECES, piece_move::EAST),
						move_one_piece_c.get_next_field_on_east_move(current_iterator->first.target_piece, current_iterator->first)
					);
					candidates_for_successor_states.emplace_back(
						piece_move(COUNT_NON_TARGET_PIECES, piece_move::NORTH),
						move_one_piece_c.get_next_field_on_north_move(current_iterator->first.target_piece, current_iterator->first)
					);
					candidates_for_successor_states.emplace_back(
						piece_move(COUNT_NON_TARGET_PIECES, piece_move::SOUTH),
						move_one_piece_c.get_next_field_on_south_move(current_iterator->first.target_piece, current_iterator->first)
					);

					for (std::size_t rob_id{ 0 }; rob_id < COUNT_NON_TARGET_PIECES; ++rob_id) {
						candidates_for_successor_states.emplace_back(
							piece_move(static_cast<piece_move::piece_id_type>(rob_id), piece_move::WEST),
							move_one_piece_c.get_next_field_on_west_move(current_iterator->first.non_target_pieces[rob_id], current_iterator->first)
						);
						candidates_for_successor_states.emplace_back(
							piece_move(static_cast<piece_move::piece_id_type>(rob_id), piece_move::EAST),
							move_one_piece_c.get_next_field_on_east_move(current_iterator->first.non_target_pieces[rob_id], current_iterator->first)
						);
						candidates_for_successor_states.emplace_back(
							piece_move(static_cast<piece_move::piece_id_type>(rob_id), piece_move::NORTH),
							move_one_piece_c.get_next_field_on_north_move(current_iterator->first.non_target_pieces[rob_id], current_iterator->first)
						);
						candidates_for_successor_states.emplace_back(
							piece_move(static_cast<piece_move::piece_id_type>(rob_id), piece_move::SOUTH),
							move_one_piece_c.get_next_field_on_south_move(current_iterator->first.non_target_pieces[rob_id], current_iterator->first)
						);
					}

					// check if reached goal
					for (std::size_t candidate{ 0 }; candidate < 4; ++candidate) {
						if (candidates_for_successor_states[candidate].next_field_paired_enable.first == p_target_cell) {
							optimal_solution_size = current_iterator->second.steps + 1;
						}
					}

					// add candidates to map if they are valid:
					for (auto& c : candidates_for_successor_states) {
						if (c.next_field_paired_enable.second) { // there is a real move

							// create next state
							// c.next_field_paired_enable.first; // new cell id
							auto new_state = positions_of_pieces<COUNT_NON_TARGET_PIECES>(current_iterator->first);

							if (c.move.piece_id < COUNT_NON_TARGET_PIECES) {
								new_state.non_target_pieces[c.move.piece_id] = c.next_field_paired_enable.first;
								new_state.sort_non_target_pieces();
							}
							else {
								new_state.target_piece = c.next_field_paired_enable.first;
							}

							if (solutions_map[new_state].steps > current_iterator->second.steps + 1) { // check if path to successor state is an optimal one (as far as we have seen)
								// to make it more efficient: use an .insert(...) get the iterator to newly inserted element.

								// hint: on map entry creation by if condition, steps defaults to MAX value of std::size_t

								// delete all predecessors!
								//for (const auto& tuple : solutions_map[new_state].predecessors) { // it is always empty because of fifo order of visited_game_states
								//	--(std::get<0>(tuple)->second.count_successors);
								//}
								//solutions_map[new_state].predecessors.clear();

								solutions_map[new_state].steps = current_iterator->second.steps + 1;
								solutions_map[new_state].predecessors.emplace_back(current_iterator, c.move);
								++(current_iterator->second.count_successors);
								visited_game_states[expand_size + 1].push_back(solutions_map.find(new_state));
							}
							else {
								if (solutions_map[new_state].steps == current_iterator->second.steps + 1) {
									solutions_map[new_state].predecessors.emplace_back(current_iterator, c.move);
									++(current_iterator->second.count_successors);
									// visited_game_states.push_back(solutions_map.find(new_state)); don't add, already added on first path reaching new_state
								}
							}
						}
					}
				}

			}
			return optimal_solution_size;

		}


	}

}
