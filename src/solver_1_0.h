#pragma once

#include "default_models_1_0.h"

#include <map>
#include <array>
#include <algorithm>
#include <limits>

namespace tobor {

	namespace v1_0 {

		/**
		* @brief Class for keeping the information about quick jumps
		*
		* @details For a board, it stores for each cell the information which are the cells to move to on one step [west, east, south, north] until wall, assuming that there are no pieces on the way.
		*/
		template<class World_Type_T = default_legacy_world>
		class quick_move_cache {
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
			quick_move_cache(const world_type& board) : board(board) {
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

		using default_quick_move_cache = quick_move_cache<>;

		/**
		*	@brief Calculates successor states by calcualting successor cell of single pieces for moving in a given direction.
		*/
		template<class Position_Of_Pieces_T = default_positions_of_pieces, class Quick_Move_Cache_T = default_quick_move_cache, class Piece_Move_Type = default_piece_move>
		class move_one_piece_calculator { // logic engine
		public:

			using positions_of_pieces_type = Position_Of_Pieces_T;
			using cell_id_type = typename positions_of_pieces_type::cell_id_type;
			using world_type = typename positions_of_pieces_type::world_type;
			using cell_id_int_type = typename cell_id_type::int_type;


			using quick_move_cache_type = Quick_Move_Cache_T;


			using piece_move_type = Piece_Move_Type;
			using piece_id_type = typename piece_move_type::piece_id_type;

			static_assert(
				std::is_same<typename positions_of_pieces_type::world_type, typename quick_move_cache_type::world_type>::value,
				"Incompatible template arguments. typename Position_Of_Pieces_T::world_type must equal typenname Quick_Move_Cache_T::world_type"
				);

			static_assert(
				std::is_same<typename positions_of_pieces_type::pieces_quantity_type, typename piece_move_type::pieces_quantity_type>::value,
				"Incompatible template arguments. typename Position_Of_Pieces_T::pieces_quantity_type must equal typenname Piece_Move_Type::pieces_quantity_type"
				);

			struct arithmetic_error {

				struct no_move {
					std::vector<piece_move_type> zero_moves;
				};

				struct multi_move {
					std::vector<piece_move_type> zero_moves;
				};
			};

		private:

			const world_type& my_world;

			quick_move_cache_type cache;

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
			inline std::pair<cell_id_type, bool> get_next_cell_on_west_move(const cell_id_type& start_cell, const positions_of_pieces_type& state) {
				cell_id_int_type next_cell{ cache.get_west(start_cell.get_id()) };

				//if (start_cell.get_id() == next_cell) { // no move possible by walls
				//	return std::make_pair(cell_id_type::create_by_id(next_cell, my_world), false);
				//}

				// iterate over all pieces since they may appear as obstacle
				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					if (next_cell <= state.piece_positions()[i].get_id() && state.piece_positions()[i].get_id() < start_cell.get_id()) { // target piece
						next_cell = state.piece_positions()[i].get_id() + 1;
					}
				}
				// may be improved for sorted states.

				if (start_cell.get_id() == next_cell) { // no move possible by walls
					return std::make_pair(cell_id_type::create_by_id(next_cell, my_world), false);
				}

				return std::make_pair(cell_id_type::create_by_id(next_cell, my_world), true);
			}

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving east until obstacle.
			*/
			inline std::pair<cell_id_type, bool> get_next_cell_on_east_move(const cell_id_type& start_cell, const positions_of_pieces_type& state) {

				//decltype(cell_id_type::get_id) cell_id_type::* get_id_type = &cell_id_type::get_id;
				//decltype(quick_move_cache::get_west) quick_move_cache::* next_cache_direction = &quick_move_cache::get_east;

				cell_id_int_type next_cell{ cache.get_east(start_cell.get_id()) };

				//if (start_cell.get_id() == next_cell) { // no move possible by walls
				//	return std::make_pair(cell_id_type::create_by_id(next_cell, my_world), false);
				//}

				// iterate over all pieces since they may appear as obstacle

				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					if (start_cell.get_id() < state.piece_positions()[i].get_id() && state.piece_positions()[i].get_id() <= next_cell) { // target piece
						next_cell = state.piece_positions()[i].get_id() - 1;
					}
				}

				if (start_cell.get_id() == next_cell) { // no move possible by walls
					return std::make_pair(cell_id_type::create_by_id(next_cell, my_world), false);
				}

				return std::make_pair(cell_id_type::create_by_id(next_cell, my_world), true);
			}

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving south until obstacle.
			*/
			inline std::pair<cell_id_type, bool> get_next_cell_on_south_move(const cell_id_type& start_cell, const positions_of_pieces_type& state) {
				cell_id_int_type next_cell{ cache.get_south(start_cell.get_transposed_id()) };

				//if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
				//	return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), false);
				//}

				// iterate over all pieces since they may appear as obstacle

				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					if (next_cell <= state.piece_positions()[i].get_transposed_id() && state.piece_positions()[i].get_transposed_id() < start_cell.get_transposed_id()) { // target piece
						next_cell = state.piece_positions()[i].get_transposed_id() + 1;
					}
				}

				if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
					return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), false);
				}

				return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), true);
			}

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving north until obstacle.
			*/
			inline std::pair<cell_id_type, bool> get_next_cell_on_north_move(const cell_id_type& start_cell, const positions_of_pieces_type& state) {
				cell_id_int_type next_cell{ cache.get_north(start_cell.get_transposed_id()) };

				//if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
				//	return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), false);
				//}

				// iterate over all pieces since they may appear as obstacle

				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					if (start_cell.get_transposed_id() < state.piece_positions()[i].get_transposed_id() && state.piece_positions()[i].get_transposed_id() <= next_cell) { // target piece
						next_cell = state.piece_positions()[i].get_transposed_id() - 1;
					}
				}

				if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
					return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), false);
				}

				return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), true);
			}

			inline std::pair<positions_of_pieces_type, bool> successor_state(
				const positions_of_pieces_type& state,
				const typename piece_move_type::piece_id_type& _piece_id,
				const direction& _direction)
			{
				positions_of_pieces_type next_state{ state };
				auto& cell_to_be_moved = next_state.piece_positions()[_piece_id.value];
				std::pair<cell_id_type, bool> next_cell_paired_true_move;
				switch (_direction.operator tobor::v1_0::direction::int_type())
				{
				case direction::encoding::EAST:
					next_cell_paired_true_move = get_next_cell_on_east_move(cell_to_be_moved, state);
					break;
				case direction::encoding::WEST:
					next_cell_paired_true_move = get_next_cell_on_west_move(cell_to_be_moved, state);
					break;
				case direction::encoding::SOUTH:
					next_cell_paired_true_move = get_next_cell_on_south_move(cell_to_be_moved, state);
					break;
				case direction::encoding::NORTH:
					next_cell_paired_true_move = get_next_cell_on_north_move(cell_to_be_moved, state);
					break;
				default:
					return std::make_pair(state, false);
					break;  // error
				}
				if (!next_cell_paired_true_move.second) {
					return std::make_pair(state, false);
				}

				next_state.piece_positions()[_piece_id.value] = next_cell_paired_true_move.first;
				next_state.sort_pieces();

				return std::make_pair(next_state, true);
			}

			inline std::pair<positions_of_pieces_type, bool> successor_state(
				const positions_of_pieces_type& state,
				const piece_move_type& move)
			{
				return successor_state(state, move.pid, move.dir);
			}

			inline positions_of_pieces_type state_plus_move(const positions_of_pieces_type& state, const piece_move_type& move) {
				return successor_state(state, move).first;
			}

			inline piece_move_type state_minus_state(const positions_of_pieces_type& to_state, const positions_of_pieces_type& from_state) {
				if (from_state == to_state) { // no move error

					typename arithmetic_error::no_move no_move_exception;

					for (auto pid = typename piece_move_type::piece_id_type::begin(); pid < typename piece_move_type::piece_id_type::end(); ++pid) {
						for (auto dir = direction::begin(); dir < direction::end(); ++dir) {
							if (state_plus_move(from_state, piece_move_type(pid, dir)) == to_state) {
								no_move_exception.zero_moves.emplace_back(pid, dir);
							}
						}
					}

					throw no_move_exception;
				}

				typename arithmetic_error::multi_move multi_move_exception; // collect all valid moves

				for (auto pid = typename piece_move_type::piece_id_type::begin(); pid < typename piece_move_type::piece_id_type::end(); ++pid) {
					for (auto dir = direction::begin(); dir < direction::end(); ++dir) {
						if (state_plus_move(from_state, piece_move_type(pid, dir)) == to_state) {
							multi_move_exception.zero_moves.emplace_back(pid, dir);
						}
					}
				}

				if (multi_move_exception.zero_moves.size() != 1) {
					throw multi_move_exception;
				}

				return multi_move_exception.zero_moves[0];

			}

		};

		using default_move_one_piece_calculator = move_one_piece_calculator<>;


		/**
		* @brief A struct for all information on a state graph node:
			Iterators to optimal predecessor states and the connecting move for each predecessor,
			The number of successors where this is an optimal predecessor
			The optimal number of steps from initial state
		*/
		template <class Positions_Of_Pieces_Type = default_positions_of_pieces, class Piece_Move_Type = default_piece_move>
		struct state_graph_node {
		public:

			/* dependent types... */

			using type = state_graph_node<Positions_Of_Pieces_Type, Piece_Move_Type>;

			using positions_of_pieces_type = Positions_Of_Pieces_Type;

			using cell_id_type = typename positions_of_pieces_type::cell_id_type;

			using piece_move_type = Piece_Move_Type;



			/* type consistency */

			static_assert(
				std::is_same<typename positions_of_pieces_type::pieces_quantity_type, typename piece_move_type::pieces_quantity_type>::value,
				"Incompatible template arguments: typename Position_Of_Pieces_T::pieces_quantity_type must equal typenname Piece_Move_Type::pieces_quantity_type"
				);



			/* introduced types... */

			using partial_solutions_map_type = std::map<positions_of_pieces_type, type>;

			using map_iterator_type = typename partial_solutions_map_type::iterator;

			using predecessor_tuple = std::tuple<map_iterator_type, piece_move_type>;

			using size_type = std::size_t;

			static constexpr size_type MAX{ std::numeric_limits<size_type>::max() };


			struct move_candidate {

				/** The move consisting of piece_id and direction. */
				piece_move_type move;

				/** The successor state */
				positions_of_pieces_type successor_state;

				/** True if the position indeed changes on this move */
				bool is_true_move;

				move_candidate(
					const piece_move_type& move,
					const std::pair<positions_of_pieces_type, bool>& n
				) :
					move(move),
					successor_state(n.first),
					is_true_move(n.second)
				{
				}

			};


			/* data... */

			std::vector<predecessor_tuple> optimal_predecessors;

			size_type smallest_seen_step_distance_from_initial_state{ MAX };

			size_type count_successors_where_this_is_one_optimal_predecessor{ 0 }; // is leaf iff == 0, warning: can be 0 before exploration or after
		};

		using default_state_graph_node = state_graph_node<>;


		template <class Move_One_Piece_Calculator, class State_Graph_Node = default_state_graph_node>
		class partial_state_graph {
		public:

			/* dependent types... */

			using move_one_piece_calculator_type = Move_One_Piece_Calculator;

			using positions_of_pieces_type = typename move_one_piece_calculator_type::positions_of_pieces_type;

			using cell_id_type = typename positions_of_pieces_type::cell_id_type;

			using piece_move_type = typename move_one_piece_calculator_type::piece_move_type;

			using state_graph_node_type = State_Graph_Node;

			using move_candidate = typename state_graph_node_type::move_candidate;

			static_assert(
				std::is_same<
				typename move_one_piece_calculator_type::positions_of_pieces_type,
				typename state_graph_node_type::positions_of_pieces_type>::value,
				"Incompatible template arguments"
				);

			static_assert(
				std::is_same<
				typename move_one_piece_calculator_type::piece_move_type,
				typename state_graph_node_type::piece_move_type>::value,
				"Incompatible template arguments"
				);


			// all game states that have been found so far,
			using partial_solutions_map_type = typename state_graph_node_type::partial_solutions_map_type;

			using partial_solutions_map_mapped_type = typename partial_solutions_map_type::mapped_type;

			// to be used as a pointer to a game state
			using map_iterator = typename state_graph_node_type::map_iterator_type;

			using size_type = typename state_graph_node_type::size_type;

			using move_path_type = move_path<piece_move_type>;

			static constexpr size_type SIZE_TYPE_MAX = state_graph_node_type::MAX;

		private:

		public:
			partial_solutions_map_type ps_map; // should not be public anymore!
		private:


			// number of steps needed by any optimal solution
			size_type optimal_path_length;

			// initial state
			positions_of_pieces_type initial_state;

			// All game states that have been found yet, ordered by their shortest distance from initial state.
			// .back() contains all game states to be explored if one deepening step just finished.
			std::vector<std::vector<map_iterator>> visited_game_states;

			template<class Insert_Iterator>
			inline void optimal_move_path_helper_back_to_front(map_iterator state, Insert_Iterator destination, const move_path_type& path_suffix = move_path_type()) {

				if (state->second.smallest_seen_step_distance_from_initial_state == 0) {
					destination = path_suffix;
				}

				for (auto& tuple : state->second.optimal_predecessors) {

					auto& predecessor_map_iterator{ std::get<0>(tuple) };
					auto& move{ std::get<1>(tuple) };
					move_path_type path(path_suffix.vector().size() + 1);
					path.vector()[0] = move;
					std::copy(path_suffix.vector().cbegin(), path_suffix.vector().cend(), path.vector().begin() + 1);

					optimal_move_path_helper_back_to_front(predecessor_map_iterator, destination, path);
				}
			}

		public:


			// ### note in case of removing states with no optimal successors, the invalid iterator problem arises.
			partial_state_graph(const positions_of_pieces_type& initial_state) :
				optimal_path_length(SIZE_TYPE_MAX),
				initial_state(initial_state)
			{
				ps_map[initial_state].smallest_seen_step_distance_from_initial_state = 0; // insert initial state into map
				//ps_map[initial_state].count_successors_where_this_is_one_optimal_predecessor; -> default 0
				//ps_map[initial_state].optimal_predecessors; -> default empty vector

				visited_game_states.push_back(std::vector<map_iterator>{ps_map.begin()}); // insert initial state into visited states
			}

			partial_state_graph(
				const typename positions_of_pieces_type::target_pieces_array_type& initial_target_pieces,
				const typename positions_of_pieces_type::non_target_pieces_array_type& initial_non_target_pieces
			) :
				partial_state_graph(
					positions_of_pieces_type(initial_target_pieces, initial_non_target_pieces)
				)
			{
				// ## check if removing this ctor is possible
			}

			inline size_type get_optimal_path_length() { return optimal_path_length; };

			inline std::vector<map_iterator> optimal_final_state_iterators(const cell_id_type& target_cell) const {
				std::vector<map_iterator> result;
				for (const auto& iter : visited_game_states.back()) {
					if (iter->first.is_final(target_cell)) {
						result.push_back(iter);
					}
				}
				return result;
			}

			/*
			inline std::vector<map_iterator> optimal_distance_dead_state_iterators(const cell_id_type& target_cell) const {
				//std::vector<map_iterator> result;
				//for (const auto& iter : visited_game_states.back()) {
				//	if (iter->first.is_final(target_cell)) {
				//		result.push_back(iter);
				//	}
				//}
				//return result;
			}
			*/

			inline std::vector<positions_of_pieces_type> optimal_final_states(const cell_id_type& target_cell) const {
				std::vector<positions_of_pieces_type> result;
				for (const auto& iter : visited_game_states.back()) {
					if (iter->first.is_final(target_cell)) {
						result.push_back(iter->first);
					}
				}
				return result;
			}

			inline std::map<positions_of_pieces_type, std::vector<move_path_type>> optimal_move_paths(const cell_id_type& target_cell) {
				std::map<positions_of_pieces_type, std::vector<move_path_type>> result;
				for (auto iter = ps_map.begin(); iter != ps_map.end(); ++iter) {
					auto& state{ iter->first };
					if (state.is_final(target_cell)) {
						optimal_move_path_helper_back_to_front(iter, std::back_inserter(result[state]));
					}
				}
				return result;
			}


			inline void remove_dead_states(const std::vector<map_iterator>& live_states) {
				for (const auto& map_it : live_states) {
					++map_it->second.count_successors_where_this_is_one_optimal_predecessor;
				}
				std::vector<map_iterator> to_be_removed; // all iterators pointing to states where:   count_successors_where_this_is_one_optimal_predecessor == 0

				for (auto iter = ps_map.begin(); iter != ps_map.end(); ++iter) {
					if (iter->second.count_successors_where_this_is_one_optimal_predecessor == 0) {
						to_be_removed.push_back(iter);
					}
				}

				while (!to_be_removed.empty()) {
					map_iterator removee = to_be_removed.back();
					to_be_removed.pop_back();

					for (auto iter = removee->second.optimal_predecessors.begin(); iter != removee->second.optimal_predecessors.end(); ++iter) {
						auto& pred{ std::get<0>(*iter) };
						--pred->second.count_successors_where_this_is_one_optimal_predecessor;
						if (pred->second.count_successors_where_this_is_one_optimal_predecessor == 0) {
							to_be_removed.push_back(pred);
						}
					}

					ps_map.erase(removee);
				}

				for (const auto& map_it : live_states) {
					--map_it->second.count_successors_where_this_is_one_optimal_predecessor;
				}
			}

			inline void remove_dead_states(const cell_id_type& target_cell_defining_live_states) {
				return remove_dead_states(optimal_final_state_iterators(target_cell_defining_live_states));
			}

			//inline void remove_dead_states2(const std::vector<map_iterator>& dead_states = ) {
			//	//...
			//}


			// ### offer step-wise exploration instead of exploration until optimal.
			inline void explore_until_optimal_solution_distance(
				move_one_piece_calculator_type& engine,
				const cell_id_type& target_cell
			) {
				const size_type SIZE_VISITED_BEFORE{ visited_game_states.size() };

				// condition: SIZE_VISITED_BEFORE == 1 (on first call only)

				if (SIZE_VISITED_BEFORE > optimal_path_length) {
					//Already executed exploration before. Any state to be explored whould add states into map beyond the optimal path length
					return;
				}

				if (visited_game_states[0][0]->first.is_final(target_cell)) {
					optimal_path_length = 0;
					return;
				}

				for (size_type expand_level_index{ SIZE_VISITED_BEFORE - 1 }; expand_level_index < optimal_path_length; ++expand_level_index) {

					// condition: visited_game_states.size() == expand_size + 1

					visited_game_states[expand_level_index].shrink_to_fit();

					visited_game_states.emplace_back();

					visited_game_states[expand_level_index + 1].reserve(visited_game_states[expand_level_index].size() * 3 + 100 * expand_level_index + 10);


					for (std::size_t expand_index_inside_level = 0; expand_index_inside_level < visited_game_states[expand_level_index].size(); ++expand_index_inside_level) {

						const map_iterator& current_iterator{ visited_game_states[expand_level_index][expand_index_inside_level] };

						std::vector<move_candidate> candidates_for_successor_states; // can be array with fixed size(?)

						// compute all successor state candidates:
						for (typename piece_move_type::piece_id_type::int_type pid = 0; pid < positions_of_pieces_type::COUNT_ALL_PIECES; ++pid) {
							for (direction direction_iter = direction::begin(); direction_iter < direction::end(); ++direction_iter) {
								candidates_for_successor_states.emplace_back(
									piece_move_type(pid, direction_iter),
									engine.successor_state(current_iterator->first, pid, direction_iter)
								);
							}
						}

						/* order of candidates:
						 piece 0: N E S W      <- target pieces come first!
						 piece 1: N E S W
						 ...
						 piece last: N E S W
						*/


						// check if reached goal
						for (
							typename std::vector<move_candidate>::size_type index_candidate{ 0 };
							// only check candidates arising from moved target robots:
							index_candidate < static_cast<typename std::vector<move_candidate>::size_type>(4) * positions_of_pieces_type::COUNT_TARGET_PIECES;
							++index_candidate
							)
						{
							if (!candidates_for_successor_states[index_candidate].is_true_move) {
								continue;
							}

							if constexpr (positions_of_pieces_type::SORTED_TARGET_PIECES) {

								if (candidates_for_successor_states[index_candidate].successor_state.is_final(target_cell)) {
									optimal_path_length = current_iterator->second.smallest_seen_step_distance_from_initial_state + 1;
								}

							}
							else {
								if (candidates_for_successor_states[index_candidate].successor_state.piece_positions()[index_candidate / 4] == target_cell) {
									// does not work for sorted final pieces! In that case we do not know where the moved piece is located.
									optimal_path_length = current_iterator->second.smallest_seen_step_distance_from_initial_state + 1;
								}
							}
						}

						// add candidates to map if they are valid:
						for (auto& c : candidates_for_successor_states) {
							if (c.is_true_move) { // there is a real move

								auto [iter_insertion, bool_inserted] = ps_map.insert(std::make_pair(c.successor_state, partial_solutions_map_mapped_type()));
								// Note: on entry creation default distance from 


								// iter_insertion is the iterator to the inserted map entry or the one which prevented insertion.

								auto& entry_value{ iter_insertion->second };

								// check if path to successor state is an optimal one (as far as we have seen):
								if (
									current_iterator->second.smallest_seen_step_distance_from_initial_state + 1
									<
									entry_value.smallest_seen_step_distance_from_initial_state
									)
								{
									/*
										This IF is asking whether the current state is now found via a shorter path than before:
											* We need to do this, if it is possible to find better paths later when a state has already been reached before
												* This never happens here, since we expand states ordered by their distance from init
											* We check here because the default distance is some MAX value.
												* if we find the state for the first time, we compare the optimal distance with the default MAX.
									*/

									// this whole IF therefore might be replaced by asking for the value of bool_inserted! <<<<<

									/*
										delete all predecessors -> not needed, because we know vector is empty
											-> will be needed if we stop relying on the expand-order
									*/


									// set optimal distance seen so far: // here it is assured to be the shortest distance from init:
									entry_value.smallest_seen_step_distance_from_initial_state = current_iterator->second.smallest_seen_step_distance_from_initial_state + 1;


									// add the current expandee as optimal predecessor:
									entry_value.optimal_predecessors.reserve(16); // ### if we do not shrink later this consumes more memory than needed.
									// <<<< we might collect statistics about typical vector sizes.
									entry_value.optimal_predecessors.emplace_back(current_iterator, c.move);


									// ++ optimal successor counter for expandee:
									++(current_iterator->second.count_successors_where_this_is_one_optimal_predecessor);


									// add newly discovered state to the vector of states to expand in the next round:
									visited_game_states[expand_level_index + 1].push_back(iter_insertion);
								}
								else {
									if (
										entry_value.smallest_seen_step_distance_from_initial_state
										==
										current_iterator->second.smallest_seen_step_distance_from_initial_state + 1
										)
									{
										/*
											Check for:
											* The state has already been found with the same distance from init
										*/


										// add the current expandee as optimal predecessor:
										entry_value.optimal_predecessors.emplace_back(current_iterator, c.move);


										// ++ optimal successor counter for expandee:
										++(current_iterator->second.count_successors_where_this_is_one_optimal_predecessor);

									}
								}
							}
						}
					}

					visited_game_states[expand_level_index + 1].shrink_to_fit();

				}
			}

		};

	}

}


