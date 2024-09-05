#pragma once

/*
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
sorted_level_distance_exploration
*/

#include "../models/simple_state_bigraph.h"

#include <vector>

namespace tobor {
	namespace v1_1 {

		/**
		*	@brief A state space explorer for game boards.
		*	@details It always has a fixed initial state which is the root of all state space exploration.
		*			It can explore the entire reachable state space or only until a target is reached.
		*/
		template <class Move_Engine_T, class Positions_Of_Pieces_T>
		class sorted_level_distance_exploration {

		public:
			using move_engine_type = Move_Engine_T;

			using positions_of_pieces_type = Positions_Of_Pieces_T;

			using piece_move_type = typename move_engine_type::piece_move_type;

			using cell_id_type = typename positions_of_pieces_type::cell_id_type;

			using size_type = std::size_t;

			static constexpr size_type SIZE_TYPE_MAX{ std::numeric_limits<size_type>::max() };

			struct move_candidate {

				/** The move consisting of piece_id and direction. */
				piece_move_type move;

				/** The successor state */
				positions_of_pieces_type successor_state;

				move_candidate(const piece_move_type& move, positions_of_pieces_type successor) : move(move), successor_state(successor) {}

			};

		private:

			using target_distance_map_type = std::map<cell_id_type, size_type>;

			using states_vector = std::vector<positions_of_pieces_type>;

			/**
			* conditions:
			*	- guaranteed to have .size() > 0
			*	- the first entry _reachable_states_by_distance[0] has always length 1 and contains the initial state
			*	- the vector at index i contains exactly the states which are reachable by an optimal path of length i
			*/
			std::vector<states_vector> _reachable_states_by_distance;

			/**
			* maps target cells to their minimal distance from initial state
			*/
			target_distance_map_type _optimal_path_length_cache;

			/**
			*	true if and only if the whole state space reachable from initial state has been fully explored.
			*/
			bool _entirely_explored{ false };

			/**
			*	@brief Sorts states inside _reachable_states_by_distance[index], removes duplicates
			*/
			inline void sort_unique(const typename std::vector<states_vector>::size_type& index /* new states index */) {
				static constexpr bool USE_RADIX_SORT{ true };

				if constexpr (USE_RADIX_SORT) {
					positions_of_pieces_type::collection_sort_unique(_reachable_states_by_distance[index]);
				}
				else {
					std::sort(/*std::execution::par,*/ _reachable_states_by_distance[index].begin(), _reachable_states_by_distance[index].end());

					_reachable_states_by_distance[index].erase(
						unique(/*std::execution::par,*/ _reachable_states_by_distance[index].begin(), _reachable_states_by_distance[index].end()),
						_reachable_states_by_distance[index].end()
					);
				}
			}

			/**
			*	@brief Removes states inside _reachable_states_by_distance[index] which have already been seen within some shorter distance
			*/
			inline void erase_seen_before(const typename std::vector<states_vector>::size_type& index /* new states index */) {

				using sub_iterator = typename std::vector<positions_of_pieces_type>::iterator;
				std::vector<sub_iterator> check_iterators;
				sub_iterator check_next = _reachable_states_by_distance[index].begin();
				sub_iterator free_next = _reachable_states_by_distance[index].begin();

				for (size_type i = 0; i < index; ++i) {
					check_iterators.emplace_back(_reachable_states_by_distance[i].begin());
				}
			continue_outer_loop:
				while (check_next != _reachable_states_by_distance[index].end()) {
					for (size_type i_level = 0; i_level < check_iterators.size(); ++i_level) {
						while (
							(check_iterators[i_level] != _reachable_states_by_distance[i_level].end())
							&&
							(*(check_iterators[i_level]) < *check_next)
							)
						{
							++check_iterators[i_level];
						}
						if (
							(check_iterators[i_level] != _reachable_states_by_distance[i_level].end())
							&&
							*check_iterators[i_level] == *check_next
							)
						{
							++check_next; // already found current state. Do not keep it.
							goto continue_outer_loop;
						}
					}

					// here we know *check_next is indeed a state never found before.
					if (free_next != check_next) {
						*free_next = *check_next;
					}
					++free_next;
					++check_next;
				}
				// shrink the vector by the elements already found earlier.
				_reachable_states_by_distance[index].erase(free_next, _reachable_states_by_distance[index].end());
			}

			/**
			*	@brief Adds all successor states of \p current_state to \p destination
			*/
			template<class Iterator_T>
			inline void add_all_nontrivial_successor_states(
				const move_engine_type& engine,
				const positions_of_pieces_type& current_state,
				Iterator_T destination
			) {
				// compute all successor state candidates:
				for (typename piece_move_type::piece_id_type::int_type pid = 0; pid < positions_of_pieces_type::COUNT_ALL_PIECES; ++pid) {
					for (direction direction_iter = direction::begin(); direction_iter < direction::end(); ++direction_iter) {

						const auto succ = engine.successor_state(current_state, pid, direction_iter);

						if (!(succ == current_state)) {
							*destination = succ;
							++destination;
						}
					}
				}
			}

			/**
			*	@brief Adds all successor states of \p current_state to \p destination
			*	@return true if and only if a target state was found.
			*/
			template<class Iterator_T>
			inline bool add_all_nontrivial_successor_states(
				const move_engine_type& engine,
				const positions_of_pieces_type& current_state,
				const cell_id_type& target_cell,
				Iterator_T destination
			) {
				bool found_final_state{ false };

				constexpr size_type COUNT_SUCC_CANDIDATES{ static_cast<size_type>(4) * positions_of_pieces_type::COUNT_ALL_PIECES };
				constexpr size_type COUNT_SUCC_CANDIDATES_WITH_TARGET_PIECE_MOVED{ static_cast<typename std::vector<move_candidate>::size_type>(4) * positions_of_pieces_type::COUNT_TARGET_PIECES };

				std::vector<move_candidate> candidates_for_successor_states;
				candidates_for_successor_states.reserve(COUNT_SUCC_CANDIDATES);

				// compute all successor state candidates:
				for (typename piece_move_type::piece_id_type::int_type pid = 0; pid < positions_of_pieces_type::COUNT_ALL_PIECES; ++pid) {
					for (direction direction_iter = direction::begin(); direction_iter < direction::end(); ++direction_iter) {
						candidates_for_successor_states.emplace_back(
							piece_move_type(pid, direction_iter),
							engine.successor_state(current_state, pid, direction_iter)
						);
					}
				}

				// order of candidates:
				// piece 0: N E S W      <- target pieces come first!
				// piece 1: N E S W
				// ...
				// piece last: N E S W



				typename std::vector<move_candidate>::size_type index_candidate{ 0 };

				// only check if reached target for candidates arising from moved target pieces:
				for (; index_candidate < COUNT_SUCC_CANDIDATES_WITH_TARGET_PIECE_MOVED; ++index_candidate) {
					if (candidates_for_successor_states[index_candidate].successor_state == current_state) {
						continue;
					}

					if constexpr (positions_of_pieces_type::SORTED_TARGET_PIECES) {
						// general case:
						if (candidates_for_successor_states[index_candidate].successor_state.is_final(target_cell)) {
							found_final_state = true;
							break;
						}

					}
					else {
						// optimized case:
						if (candidates_for_successor_states[index_candidate].successor_state.raw()[index_candidate / 4] == target_cell) {
							// does not work for sorted final pieces! In that case we do not know where the moved piece is located.
							found_final_state = true;
							break;
						}
					}

					*destination = (candidates_for_successor_states[index_candidate].successor_state);
					++destination;
				}
				// add successor states to destination without check for final state:
				for (; index_candidate < candidates_for_successor_states.size(); ++index_candidate) {
					if (candidates_for_successor_states[index_candidate].successor_state == current_state) {
						continue;
					}
					*destination = (candidates_for_successor_states[index_candidate].successor_state);
					++destination;
				}
				return found_final_state;
			}

			/**
			*	@brief Explores according to \p policy until reaching \p target_cell (or until running into policy threshold)
			*
			*	@details Caller guarantees that target_cell has not yet been found if NOT_YET_FOUND_GUARANTEED == true.
			*		If this is not guaranteed, the cache will not be updated.
			*
			*	@return Returns optimal depth for given target cell, or SIZE_TYPE_MAX if not found.
			*/
			inline size_type explore_until_target(
				const move_engine_type& engine,
				const cell_id_type& target_cell,
				const exploration_policy& policy,
				const bool NOT_YET_FOUND_GUARANTEED = false
			) {
				const size_type INDEX_LAST_EXPLORATION{ _reachable_states_by_distance.size() - 1 };

				size_type optimal_depth{ SIZE_TYPE_MAX }; // guaranteed not yet found if NOT_YET_FOUND_GUARANTEED == true

				for (size_type expand_level_index{ INDEX_LAST_EXPLORATION };
					expand_level_index < optimal_depth
					&& expand_level_index < policy.max_depth() /* policy abort*/
					&& count_states() < policy.state_count_threshold() /* policy abort*/;
					++expand_level_index) {

					if (_reachable_states_by_distance[expand_level_index].size() == 0) {
						_entirely_explored = true;
						return SIZE_TYPE_MAX; // no more states to find
					}

					_reachable_states_by_distance.emplace_back();
					_reachable_states_by_distance[expand_level_index + 1].reserve(_reachable_states_by_distance[expand_level_index].size() * 3 + 10);

					for (std::size_t expand_index_inside_level = 0; expand_index_inside_level < _reachable_states_by_distance[expand_level_index].size(); ++expand_index_inside_level) {

						if (
							add_all_nontrivial_successor_states(engine, _reachable_states_by_distance[expand_level_index][expand_index_inside_level], target_cell, std::back_inserter(_reachable_states_by_distance.back()))
							)
						{
							optimal_depth = expand_level_index + 1;
						}
					}

					sort_unique(expand_level_index + 1);
					erase_seen_before(expand_level_index + 1);
					_reachable_states_by_distance[expand_level_index + 1].shrink_to_fit();

					states_counter += _reachable_states_by_distance[expand_level_index + 1].size();
				}

				// finalizing:
				if (NOT_YET_FOUND_GUARANTEED)
					if (optimal_depth != SIZE_TYPE_MAX) {
						_optimal_path_length_cache.insert(std::make_pair(target_cell, optimal_depth));
					}
				return optimal_depth;
			}

		public:
			/**
			*	@brief Constructs an object with empty exploration state space.
			*/
			sorted_level_distance_exploration(const positions_of_pieces_type& initial_state) :
				_optimal_path_length_cache(),
				_entirely_explored(false)
			{
				_reachable_states_by_distance.emplace_back(std::vector<positions_of_pieces_type>{ initial_state });
			}

			/**
			*	@brief Returns the total number of states reached from initial state during exploration, including the initial state itself.
			*/
			inline size_type count_states() const noexcept {
				return std::accumulate(
					std::begin(_reachable_states_by_distance),
					std::end(_reachable_states_by_distance),
					size_type(0),
					[](const size_type& acc, const auto& el) { return acc + el.size(); });
			}

			/**
			*	@brief Returns true if and only if the entire state space has been explored.
			*/
			inline bool entirely_explored() const noexcept { return _entirely_explored; }

			/**
			*	@brief Returns the max depth of previously executed exploration.
			*/
			inline size_type exploration_depth() const noexcept { return _reachable_states_by_distance.size() - 1; }

			/**
			*	@brief Explores according to \p policy (until entirely explored or until running into some policy threshold)
			*/
			inline void explore(
				const move_engine_type& engine,
				const exploration_policy& policy
			) {
				const size_type INDEX_LAST_EXPLORATION{ _reachable_states_by_distance.size() - 1 };

				for (
					size_type expand_level_index{ INDEX_LAST_EXPLORATION };
					expand_level_index < policy.max_depth() && count_states() < policy.state_count_threshold() /* policy abort*/;
					++expand_level_index
					)
				{
					if (_reachable_states_by_distance[expand_level_index].size() == 0) {
						_entirely_explored = true;
						return; // no more states to find
					}

					_reachable_states_by_distance.emplace_back();
					_reachable_states_by_distance[expand_level_index + 1].reserve(_reachable_states_by_distance[expand_level_index].size() * 3 + 10);

					for (std::size_t expand_index_inside_level = 0; expand_index_inside_level < _reachable_states_by_distance[expand_level_index].size(); ++expand_index_inside_level) {
						add_all_nontrivial_successor_states(engine, _reachable_states_by_distance[expand_level_index][expand_index_inside_level], std::back_inserter(_reachable_states_by_distance.back()));
					}

					sort_unique(expand_level_index + 1);
					erase_seen_before(expand_level_index + 1);
					_reachable_states_by_distance[expand_level_index + 1].shrink_to_fit();

					states_counter += _reachable_states_by_distance[expand_level_index + 1].size();
				}
			}

			/**
			*	@brief Explores until reaching \p target_cell (without any restriction on exploration depth)
			*/
			inline size_type explore_until_target(const move_engine_type& engine, const cell_id_type& target_cell) {
				return optimal_path_length(engine, target_cell, exploration_policy::FORCE_EXPLORATION_UNRESTRICTED(), 0);
			}

			/**
			*	@brief Explores until reaching \p target_cell, restricted to max exploration depth \p max_depth
			*/
			inline size_type explore_until_target(const move_engine_type& engine, const cell_id_type& target_cell, const size_type& max_depth) {
				return optimal_path_length(engine, target_cell, exploration_policy::FORCE_EXPLORATION_UNTIL_DEPTH(max_depth), 0);
			}

			/**
			*	@brief Explores until reaching \p target_cell, if allowed by \p policy. The policy determines if it performs additional exploration or if it only looks up in previously cached or explored solutions.
			*
			*	@return Returns the optimal path length for reaching \p target_cell. Returns SIZE_TYPE_MAX in case no optimal path was found, perhaps due to \p policy.
			*/
			inline size_type optimal_path_length(const move_engine_type& engine, const cell_id_type& target_cell, const exploration_policy& policy = exploration_policy::ONLY_EXPLORED(), const size_type& min_length_hint = 0) {
				// checking cache...
				const auto iter = _optimal_path_length_cache.find(target_cell);

				if (iter != _optimal_path_length_cache.cend()) {
					return iter->second;
				}
				if (policy == exploration_policy::ONLY_CASHED()) {
					return SIZE_TYPE_MAX;
				}

				// checking explored states...
				for (size_type depth{ min_length_hint }; depth < _reachable_states_by_distance.size(); ++depth) {
					for (const auto& state : _reachable_states_by_distance[depth]) {
						if (state.is_final(target_cell)) {
							if (min_length_hint == 0) { // only update cache if there was no hint
								_optimal_path_length_cache.insert(std::make_pair(target_cell, depth));
							}
							return depth;
						}
					}
				}
				if (policy == exploration_policy::ONLY_EXPLORED()) {
					return SIZE_TYPE_MAX;
				}

				// further exploration...
				return explore_until_target(engine, target_cell, policy, min_length_hint == 0);
			}

			/**
			*	@brief Explores until reaching \p target_cell, if allowed by \p policy. The policy determines if it performs additional exploration or if it only looks up in previously cached or explored solutions.
			*
			*	@return Returns all final states covering \p target_cell.
			*/
			inline std::vector<positions_of_pieces_type> optimal_final_states(move_engine_type& engine, const cell_id_type& target_cell, const exploration_policy& policy = exploration_policy::ONLY_EXPLORED(), const size_type& min_length_hint = 0) {
				std::vector<positions_of_pieces_type> result;
				const size_type DEPTH{ optimal_path_length(engine, target_cell, policy, min_length_hint) };

				if (!(DEPTH < _reachable_states_by_distance.size()))
					return result;

				for (auto state_iter = _reachable_states_by_distance[DEPTH].cbegin(); state_iter != _reachable_states_by_distance[DEPTH].cend(); ++state_iter) {
					if (*state_iter.is_final(target_cell)) {
						result.push_back(*state_iter);
					}
				}

				return result;
			}

			/**
			*	@brief Extracts the simple_state_bigraph containing all optimal solutions for reaching \p target_cell.
			*
			*	@details Explores the state space according to \p policy
			*/
			template<class State_Label_T>
			void get_simple_bigraph(
				const move_engine_type& engine,
				const cell_id_type& target_cell,
				simple_state_bigraph<positions_of_pieces_type, State_Label_T>& destination,
				const exploration_policy& policy = exploration_policy::ONLY_EXPLORED(),
				const size_type& min_length_hint = 0
			) {
				using bigraph = simple_state_bigraph<positions_of_pieces_type, State_Label_T>;

				destination.clear();

				const size_type FINAL_DEPTH{ optimal_path_length(engine, target_cell, policy, min_length_hint) };

				if (!(FINAL_DEPTH < _reachable_states_by_distance.size()))
					return;

				std::vector<positions_of_pieces_type> states;

				// fill states with all the final states
				for (size_type i{ 0 }; i < _reachable_states_by_distance[FINAL_DEPTH].size(); ++i) {
					const auto& s{ _reachable_states_by_distance[FINAL_DEPTH][i] };
					if (s.is_final(target_cell)) {
						destination.map.insert(
							destination.map.end(),
							std::pair<typename bigraph::state_type, typename bigraph::node_links>(
								_reachable_states_by_distance[FINAL_DEPTH][i],
								typename bigraph::node_links()
							)
						);
						states.push_back(s);
					}
				}

				std::size_t backward_explore_distance = FINAL_DEPTH;

				while (backward_explore_distance > 0) {
					--backward_explore_distance;

					std::vector<std::pair<positions_of_pieces_type, positions_of_pieces_type>> possible_edges;

					// all maybe-edges
					for (const auto& state : states) {
						auto vec = engine.predecessor_states(state);
						//possible_edges.reserve(possible_edges.size() + vec.size());
						for (const positions_of_pieces_type& pred_state : vec) {
							possible_edges.emplace_back(pred_state, state);
						}
					}
					// sort by from-state
					std::sort(possible_edges.begin(), possible_edges.end());

					//remove if from state not in distance state vector
					std::size_t compare_index{ 0 };

					possible_edges.erase(
						std::remove_if(possible_edges.begin(), possible_edges.end(), [&](const std::pair<positions_of_pieces_type, positions_of_pieces_type>& edge) {
							while (compare_index < _reachable_states_by_distance[backward_explore_distance].size() && _reachable_states_by_distance[backward_explore_distance][compare_index] < edge.first) {
								++compare_index;
							}
							if (compare_index < _reachable_states_by_distance[backward_explore_distance].size() && _reachable_states_by_distance[backward_explore_distance][compare_index] == edge.first) {
								return false; // do not remove edge
							}
							return true; // remove edge
							}),
						possible_edges.end()
					);

					// add edges to the bigraph:
					states.clear();

					for (const std::pair<positions_of_pieces_type, positions_of_pieces_type>& edge : possible_edges) {
						if (states.empty() || states.back() != edge.first) {
							states.push_back(edge.first);
						}
						auto& s{ destination.map[edge.first].successors };
						s.insert(s.end(), edge.second);
						auto& p{ destination.map[edge.second].predecessors };
						p.insert(p.end(), edge.first);
					}

					// pass vector of pre-states to next loop run.
				}
			}
		};

	}
}
