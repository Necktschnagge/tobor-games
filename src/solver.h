#pragma once

#include "models.h"

#include <map>
#include <array>
#include <algorithm>


namespace tobor {

	namespace v1_0 {

		// ### alternative board representation is just two sorted vectors of indices where there are walls, additionally you can uniformly encode pieces like walls, in this case double walls left and right, or top and bottom.


		/**
		* @brief Class for keeping the information about quick jumps
		*
		* @details For a board, it stores for each cell the information which are the cells to move to on one step [west, east, south, north] until wall, assuming that there are no pieces on the way.
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

			/**
			* @brief Returns the id of the cell you reach from cell \p id when moving west with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			std::size_t get_west(std::size_t id) const { return go_west[id]; }

			/**
			* @brief Returns the id of the cell you reach from cell \p id when moving east with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			std::size_t get_east(std::size_t id) const { return go_east[id]; }

			/**
			* @brief Returns the transposed id of the cell you reach from cell \p transposed_id when moving south with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			std::size_t get_south(std::size_t transposed_id) const { return go_south[transposed_id]; }

			/**
			* @brief Returns the transposed id of the cell you reach from cell \p transposed_id when moving north with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			std::size_t get_north(std::size_t transposed_id) const { return go_north[transposed_id]; }
		};


		/**
		*	@brief Calculates successor states by calcualting successor cell of single pieces for moving in a given direction.
		*/
		template<std::size_t COUNT_TARGET_PIECES_V, std::size_t COUNT_NON_TARGET_PIECES_V>
		class move_one_piece_calculator { // logic engine
		public:
			using world_type = tobor_world;

			static constexpr std::size_t COUNT_TARGET_PIECES{ COUNT_TARGET_PIECES_V };

			static constexpr std::size_t COUNT_NON_TARGET_PIECES{ COUNT_NON_TARGET_PIECES_V };

		private:
			const world_type& my_world;

			quick_move_cache cache;

		public:

			/**
			* @brief Constructs a move_one_piece_calculator.
			*
			* @details \p my_word must not be changed externally. This is constructing a quick_move_cache inside which would be invalidated.
			*/
			move_one_piece_calculator(const world_type& my_world) : my_world(my_world), cache(my_world) {
			}

			// put the following four functions together using some if constexpr. Use the direction as template parameter. ####
			// it may be better to not return a bool, just return the start cell itself if no move happens. (?)

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving west until obstacle.
			*/
			inline std::pair<universal_cell_id, bool> get_next_cell_on_west_move(const universal_cell_id& start_cell, const positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>& state) {
				std::size_t next_cell{ cache.get_west(start_cell.get_id()) };

				//if (start_cell.get_id() == next_cell) { // no move possible by walls
				//	return std::make_pair(universal_cell_id::create_by_id(next_cell, my_world), false);
				//}

				// iterate over all pieces since they may appear as obstacle
				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					if (next_cell <= state.piece_positions[i].get_id() && state.piece_positions[i].get_id() < start_cell.get_id()) { // target piece
						next_cell = state.piece_positions[i].get_id() + 1;
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
			inline std::pair<universal_cell_id, bool> get_next_cell_on_east_move(const universal_cell_id& start_cell, const positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>& state) {

				//decltype(universal_cell_id::get_id) universal_cell_id::* get_id_type = &universal_cell_id::get_id;
				//decltype(quick_move_cache::get_west) quick_move_cache::* next_cache_direction = &quick_move_cache::get_east;

				std::size_t next_cell{ cache.get_east(start_cell.get_id()) };

				//if (start_cell.get_id() == next_cell) { // no move possible by walls
				//	return std::make_pair(universal_cell_id::create_by_id(next_cell, my_world), false);
				//}

				// iterate over all pieces since they may appear as obstacle

				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					if (start_cell.get_id() < state.piece_positions[i].get_id() && state.piece_positions[i].get_id() <= next_cell) { // target piece
						next_cell = state.piece_positions[i].get_id() - 1;
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
			inline std::pair<universal_cell_id, bool> get_next_cell_on_south_move(const universal_cell_id& start_cell, const positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>& state) {
				std::size_t next_cell{ cache.get_south(start_cell.get_transposed_id()) };

				//if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
				//	return std::make_pair(universal_cell_id::create_by_transposed_id(next_cell, my_world), false);
				//}

				// iterate over all pieces since they may appear as obstacle

				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					if (next_cell <= state.piece_positions[i].get_transposed_id() && state.piece_positions[i].get_transposed_id() < start_cell.get_transposed_id()) { // target piece
						next_cell = state.piece_positions[i].get_transposed_id() + 1;
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
			inline std::pair<universal_cell_id, bool> get_next_cell_on_north_move(const universal_cell_id& start_cell, const positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>& state) {
				std::size_t next_cell{ cache.get_north(start_cell.get_transposed_id()) };

				//if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
				//	return std::make_pair(universal_cell_id::create_by_transposed_id(next_cell, my_world), false);
				//}

				// iterate over all pieces since they may appear as obstacle

				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					if (start_cell.get_transposed_id() < state.piece_positions[i].get_transposed_id() && state.piece_positions[i].get_transposed_id() <= next_cell) { // target piece
						next_cell = state.piece_positions[i].get_transposed_id() - 1;
					}
				}

				if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
					return std::make_pair(universal_cell_id::create_by_transposed_id(next_cell, my_world), false);
				}

				return std::make_pair(universal_cell_id::create_by_transposed_id(next_cell, my_world), true);
			}

			inline std::pair<positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>, bool> successor_state(
				const positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>& state,
				const piece_id& _piece_id,
				const direction& _direction) {
				positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES> next_state{ state };
				auto& cell_to_be_moved = next_state.piece_positions[_piece_id.value];
				std::pair<universal_cell_id, bool> next_cell_paired_true_move;
				switch (_direction.value)
				{
				case direction::EAST:
					next_cell_paired_true_move = get_next_cell_on_east_move(cell_to_be_moved, state);
					break;
				case direction::WEST:
					next_cell_paired_true_move = get_next_cell_on_west_move(cell_to_be_moved, state);
					break;
				case direction::SOUTH:
					next_cell_paired_true_move = get_next_cell_on_south_move(cell_to_be_moved, state);
					break;
				case direction::NORTH:
					next_cell_paired_true_move = get_next_cell_on_north_move(cell_to_be_moved, state);
					break;
				default:
					return std::make_pair(state, false);
					break;  // error
				}
				if (!next_cell_paired_true_move.second) {
					return std::make_pair(state, false);
				}

				next_state.piece_positions[_piece_id.value] = next_cell_paired_true_move.first;
				next_state.sort_pieces();
				
				return std::make_pair(next_state, true);
			}

		};


		/*
		*	@brief Equivalent to a pair of a piece_id and a direction where to move it
		*
		*	@details Does not define how piece_id is interpreted.
		*/
		struct piece_move {
		public:
			piece_id _piece_id;
			direction _direction;

			piece_move(const piece_id& _piece_id, const direction& _direction) : _piece_id(_piece_id), _direction(_direction) {}
		};


		/**
		* @brief A struct for all information on a state graph node:
			Iterators to optimal predecessor states and the connecting move for each predecessor,
			The number of successors where this is an optimal predecessor
			The optimal number of steps from initial state
		*/
		template <std::size_t COUNT_TARGET_PIECES, std::size_t COUNT_NON_TARGET_PIECES>
		class state_graph_node {
		public:
			using cell_id_type = universal_cell_id;
			using state_type = positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>;

			using partial_solutions_map_type = std::map<positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>, state_graph_node>;

			using map_iterator_type = typename partial_solutions_map_type::iterator;

			static constexpr std::size_t MAX{ std::numeric_limits<std::size_t>::max() };

			std::vector<std::tuple<map_iterator_type, piece_move>> optimal_predecessors;
			std::size_t smallest_seen_step_distance_from_initial_state{ MAX };
			std::size_t count_successors_where_this_is_one_optimal_predecessor{ 0 }; // is leaf iff == 0, warning: can be 0 before exploration or after
		};

		/**
		*	@brief A struct consisting of
				a piece_move,
				a pair of the next cell to reach in one step and an enable flag
		*/
		class move_candidate {
		public:
			piece_move move;
			std::pair<universal_cell_id, bool> next_cell_paired_enable;

			move_candidate(const piece_move& m, const std::pair<universal_cell_id, bool>& n) : move(m), next_cell_paired_enable(n) {}
		};
		
		template <std::size_t COUNT_TARGET_PIECES, std::size_t COUNT_NON_TARGET_PIECES>
		class move_candidate_2 {
		public:
			
			piece_move move;

			std::pair<positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>, bool> next_cell_paired_enable;

			move_candidate_2(const piece_move& m, const std::pair<positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>, bool>& n) : move(m), next_cell_paired_enable(n) {}

		};

		template <std::size_t COUNT_TARGET_PIECES, std::size_t COUNT_NON_TARGET_PIECES>
		class partial_state_graph {
		public:

			// all game states that have been found so far,
			// each one with pointers to their optimal predecessors as well as a counter for successors.
			using solutions_map_type = typename state_graph_node<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>::partial_solutions_map_type;

			// to be used as a pointer to a game state
			using map_iterator = typename state_graph_node<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>::map_iterator_type;

			solutions_map_type solutions_map;

			// number of steps needed by any optimal solution
			std::size_t optimal_solution_size{ state_graph_node<COUNT_TARGET_PIECES,COUNT_NON_TARGET_PIECES>::MAX };

			// initial state
			positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES> initial_state;

			// All game states that have been found yet, ordered by their shortest distance from initial state.
			// .back() contains all game states to be explored if one deepening step just finished.
			std::vector<std::vector<map_iterator>> visited_game_states;
			// ### note in case of removing states with no optimal successors, the invalid iterator problem arises.


			partial_state_graph(const positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>& p_initial_state) : initial_state(p_initial_state) {}

			// add the logic for exploring into this class., like explore one level
		};

		template<std::size_t COUNT_TARGET_PIECES = 1, std::size_t COUNT_NON_TARGET_PIECES = 3>
		inline partial_state_graph<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES> build_state_graph_for_all_optimal_solutions(
			move_one_piece_calculator<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>& move_one_piece_c,
			const universal_cell_id& p_target_cell,
			const std::array<universal_cell_id, COUNT_TARGET_PIECES>& p_target_pieces,
			const std::array<universal_cell_id, COUNT_NON_TARGET_PIECES>& p_non_target_pieces
		) {
			partial_state_graph<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES> state_graph = partial_state_graph<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>(
				positions_of_pieces<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>(p_target_pieces, p_non_target_pieces)
				);


			using map_iterator = typename partial_state_graph<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>::map_iterator;


			/*	insert the initial game state into map of all visited states : */
			state_graph.solutions_map[state_graph.initial_state].smallest_seen_step_distance_from_initial_state = 0; // insert initial_state
			// solutions_map[initial_state].predecessors.clear(); // already empty be default
			// solutions_map[initial_state].count_successors = 0; // already empty be default

			/*	insert the initial game state into vector of all visited states : */
			state_graph.visited_game_states.push_back(std::vector<map_iterator>{state_graph.solutions_map.begin()});


			// what if initial state is already final? check this! ####

			for (std::size_t expand_size{ 0 }; expand_size < state_graph.optimal_solution_size; ++expand_size) {
				state_graph.visited_game_states.emplace_back(); // possibly invalidates iterators on sub-vectors, but on most compilers it will work anyway.
				// But please do not rely on this behaviour.

				for (const map_iterator& current_iterator : state_graph.visited_game_states[expand_size]) {
					//const auto current_iterator{ visited_game_states[index_next_exploration] };

					std::vector<move_candidate_2<COUNT_TARGET_PIECES, COUNT_NON_TARGET_PIECES>> candidates_for_successor_states;


					// compute all successor state candidates:

					for (piece_id::int_type current_piece_id{ 0 }; current_piece_id < state_graph.initial_state.COUNT_ALL_PIECES ; ++current_piece_id) { // create a dependent type with begin(), end() of piece ids as member class inside piece_position...

						for (direction direction_iter = direction::begin(); direction_iter < direction::end(); ++direction_iter) {

							candidates_for_successor_states.emplace_back(
								piece_move(piece_id(current_piece_id), direction_iter),
								move_one_piece_c.successor_state(current_iterator->first, current_piece_id, direction_iter)
							);

						}

					}

					// check if reached goal
					for (std::size_t candidate{ 0 }; candidate < static_cast<std::size_t>(4) * COUNT_TARGET_PIECES; ++candidate) { // only the first 4 candidates arise from moved target robots...
						if (candidates_for_successor_states[candidate].next_cell_paired_enable.first.piece_positions[candidate/4 ] == p_target_cell) {
							state_graph.optimal_solution_size = current_iterator->second.smallest_seen_step_distance_from_initial_state + 1;
						}
					}

					// add candidates to map if they are valid:
					for (auto& c : candidates_for_successor_states) {
						if (c.next_cell_paired_enable.second) { // there is a real move

							auto& new_state_2 = c.next_cell_paired_enable.first;


							/// check this whole comparison again!

							if (state_graph.solutions_map[new_state_2].smallest_seen_step_distance_from_initial_state > current_iterator->second.smallest_seen_step_distance_from_initial_state + 1) { // check if path to successor state is an optimal one (as far as we have seen)
								/// ?????????????? if improvement.... only occurs as improving from not seen (=MAX) to some finite value for a distance

								// to make it more efficient: use an .insert(...) get the iterator to newly inserted element.

								// hint: on map entry creation by if condition, steps defaults to MAX value of std::size_t

								// delete all predecessors!
								//for (const auto& tuple : solutions_map[new_state].predecessors) { // it is always empty because of fifo order of visited_game_states
								//	--(std::get<0>(tuple)->second.count_successors);
								//}
								//solutions_map[new_state].predecessors.clear();

								state_graph.solutions_map[new_state_2].smallest_seen_step_distance_from_initial_state = current_iterator->second.smallest_seen_step_distance_from_initial_state + 1;
								state_graph.solutions_map[new_state_2].optimal_predecessors.emplace_back(current_iterator, c.move); // why not delete old ones?
								++(current_iterator->second.count_successors_where_this_is_one_optimal_predecessor);
								state_graph.visited_game_states[expand_size + 1].push_back(state_graph.solutions_map.find(new_state_2));
							}
							else {
								if (state_graph.solutions_map[new_state_2].smallest_seen_step_distance_from_initial_state == current_iterator->second.smallest_seen_step_distance_from_initial_state + 1) {
									state_graph.solutions_map[new_state_2].optimal_predecessors.emplace_back(current_iterator, c.move);
									++(current_iterator->second.count_successors_where_this_is_one_optimal_predecessor);
									// visited_game_states.push_back(solutions_map.find(new_state)); don't add, already added on first path reaching new_state
								}
							}
						}
					}
				}

			}
			return state_graph;

		}


	}

}
