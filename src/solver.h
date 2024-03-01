#pragma once

#include "models.h"

#include <map>
#include <array>
#include <algorithm>
#include <limits>

class GameController; // for friend class declaration, to be removed

namespace tobor {

	namespace v1_0 {

		// ### alternative board representation is just two sorted vectors of indices where there are walls, additionally you can uniformly encode pieces like walls, in this case double walls left and right, or top and bottom.


		/**
		* @brief Class for keeping the information about quick jumps
		*
		* @details For a board, it stores for each cell the information which are the cells to move to on one step [west, east, south, north] until wall, assuming that there are no pieces on the way.
		*/
		template<class World_Type_T = default_world>
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
					if (next_cell <= state.piece_positions[i].get_id() && state.piece_positions[i].get_id() < start_cell.get_id()) { // target piece
						next_cell = state.piece_positions[i].get_id() + 1;
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
					if (start_cell.get_id() < state.piece_positions[i].get_id() && state.piece_positions[i].get_id() <= next_cell) { // target piece
						next_cell = state.piece_positions[i].get_id() - 1;
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
					if (next_cell <= state.piece_positions[i].get_transposed_id() && state.piece_positions[i].get_transposed_id() < start_cell.get_transposed_id()) { // target piece
						next_cell = state.piece_positions[i].get_transposed_id() + 1;
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
					if (start_cell.get_transposed_id() < state.piece_positions[i].get_transposed_id() && state.piece_positions[i].get_transposed_id() <= next_cell) { // target piece
						next_cell = state.piece_positions[i].get_transposed_id() - 1;
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
				auto& cell_to_be_moved = next_state.piece_positions[_piece_id.value];
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

				next_state.piece_positions[_piece_id.value] = next_cell_paired_true_move.first;
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

			inline bool is_target_state(const positions_of_pieces_type& state, const cell_id_type& target_cell) {
				for (auto iter = state.target_pieces_cbegin(); iter != state.target_pieces_cend(); ++iter) {
					if (*iter == target_cell)
						return true;
				}
				return false;
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

				piece_move_type move;

				std::pair<positions_of_pieces_type, bool> next_cell_paired_enable;

				move_candidate(const piece_move_type& move, const std::pair<positions_of_pieces_type, bool>& n) : move(move), next_cell_paired_enable(n) {}

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

			friend class GameController; // remove this! is bad design.
		private:

		public:
			partial_solutions_map_type ps_map; // should not be public anymore!
		private:


			// number of steps needed by any optimal solution
			size_type optimal_solution_size;

			// initial state
			positions_of_pieces_type initial_state;

			// All game states that have been found yet, ordered by their shortest distance from initial state.
			// .back() contains all game states to be explored if one deepening step just finished.
			std::vector<std::vector<map_iterator>> visited_game_states;

		public:

			// ### note in case of removing states with no optimal successors, the invalid iterator problem arises.
			partial_state_graph(const positions_of_pieces_type& initial_state) :
				optimal_solution_size(state_graph_node_type::MAX),
				initial_state(initial_state)
			{
				ps_map[initial_state].smallest_seen_step_distance_from_initial_state = 0; // insert initial state into map

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
			}

			inline size_type get_optimal_solution_size() { return optimal_solution_size; };

			template<class Insert_Iterator>
			inline void optimal_path_helper_back_to_front(map_iterator state, Insert_Iterator destination, const move_path_type& rest_path = move_path_type()) {

				if (state->second.smallest_seen_step_distance_from_initial_state == 0) {
					destination = rest_path;
				}

				for (auto& tuple : state->second.optimal_predecessors) {
					auto& predecessor_map_iterator{ std::get<0>(tuple) };
					auto& move{ std::get<1>(tuple) };
					move_path_type path(rest_path.vector().size() + 1);
					path.vector()[0] = move;
					std::copy(rest_path.vector().cbegin(), rest_path.vector().cend(), path.vector().begin() + 1);

					optimal_path_helper_back_to_front(predecessor_map_iterator, destination, path);
				}


			}

			inline std::map<positions_of_pieces_type, std::vector<move_path_type>> optimal_paths(const cell_id_type& target_cell) {

				std::map<positions_of_pieces_type, std::vector<move_path_type>> result;

				//std::vector<positions_of_pieces_type> goal_states;

				for (
					auto iter = ps_map.begin(); iter != ps_map.end(); ++iter
					) {
					auto& state{ iter->first };
					if (state.is_final(target_cell)) {
						//goal_states.push_back(state);

						optimal_path_helper_back_to_front(iter, std::back_inserter(result[state]));
					}
				}

				return result;

			}

			inline void build_state_graph_for_all_optimal_solutions(
				move_one_piece_calculator_type& engine,
				const cell_id_type& target_cell
			) {
				// what if initial state is already final? check this! ####

				for (size_type expand_size{ 0 }; expand_size < optimal_solution_size; ++expand_size) {

					visited_game_states.emplace_back();
					// possibly invalidates iterators on sub-vectors, but on most compilers it will work anyway.
					// But please do not rely on this behaviour.

					//if (expand_size != 0) {
						//visited_game_states[expand_size].shrink_to_fit();
					visited_game_states[expand_size + 1].reserve(visited_game_states[expand_size].size() * 3 + 100 * expand_size + 10);
					//}

					for (std::size_t iii = 0; iii < visited_game_states[expand_size].size(); ++iii) {

						if (!(iii % 1000)) {
							auto x = 5;
							(void)x;
						}

						const map_iterator& current_iterator{ visited_game_states[expand_size][iii] };

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

						// check if reached goal
						for (
							typename std::vector<move_candidate>::size_type candidate{ 0 };
							candidate < static_cast<typename std::vector<move_candidate>::size_type>(4) * positions_of_pieces_type::COUNT_TARGET_PIECES; // only check candidates arising from moved target robots...
							++candidate
							)
						{
							if (candidates_for_successor_states[candidate].next_cell_paired_enable.first.piece_positions[candidate / 4] == target_cell) {
								optimal_solution_size = current_iterator->second.smallest_seen_step_distance_from_initial_state + 1;
							}
						}

						// add candidates to map if they are valid:
						for (auto& c : candidates_for_successor_states) {
							if (c.next_cell_paired_enable.second) { // there is a real move

								auto& new_state_2 = c.next_cell_paired_enable.first;


								/// check this whole comparison again!

								auto [iter_insertion, bool_inserted] = ps_map.insert(std::make_pair(new_state_2, partial_solutions_map_mapped_type()));

								auto& entry_value{ iter_insertion->second };
								//auto& entry_key{ iter_insertion->first };

								if (entry_value.smallest_seen_step_distance_from_initial_state > current_iterator->second.smallest_seen_step_distance_from_initial_state + 1) { // check if path to successor state is an optimal one (as far as we have seen)
									/// ?????????????? if improvement.... only occurs as improving from not seen (=MAX) to some finite value for a distance

									// to make it more efficient: use an .insert(...) get the iterator to newly inserted element.

									// hint: on map entry creation by if condition, steps defaults to MAX value of std::size_t

									// delete all predecessors!
									//for (const auto& tuple : solutions_map[new_state].predecessors) { // it is always empty because of fifo order of visited_game_states
									//	--(std::get<0>(tuple)->second.count_successors);
									//}
									//solutions_map[new_state].predecessors.clear();

									entry_value.smallest_seen_step_distance_from_initial_state = current_iterator->second.smallest_seen_step_distance_from_initial_state + 1;

									entry_value.optimal_predecessors.reserve(16);

									entry_value.optimal_predecessors.emplace_back(current_iterator, c.move); // why not delete old ones?
									auto& c_pred{ current_iterator->second.count_successors_where_this_is_one_optimal_predecessor };
									++c_pred;
									visited_game_states[expand_size + 1].push_back(iter_insertion);
								}
								else {
									if (entry_value.smallest_seen_step_distance_from_initial_state == current_iterator->second.smallest_seen_step_distance_from_initial_state + 1) {
										entry_value.optimal_predecessors.emplace_back(current_iterator, c.move); // can grow to size 9 (why???)

										auto& c_pred{ current_iterator->second.count_successors_where_this_is_one_optimal_predecessor };
										++c_pred;
										// visited_game_states.push_back(solutions_map.find(new_state)); don't add, already added on first path reaching new_state
									}
								}
							}
						}
					}

				}
			}

		};

	}

}


