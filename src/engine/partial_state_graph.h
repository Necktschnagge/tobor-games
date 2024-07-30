#pragma once

#include <vector>
#include <map>

namespace tobor {
	namespace v1_0 {


		template <class Move_One_Piece_Calculator, class State_Graph_Node>
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
