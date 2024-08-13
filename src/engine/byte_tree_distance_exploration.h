#pragma once


#pragma once

#include "../models/simple_state_bigraph.h"
#include "exploration_policy.h"

#include "byte_tree_map.h"


#include <vector>

namespace tobor {
	namespace v1_1 {

		/**
		*	@brief A state space explorer for game boards.
		*	@details It always has a fixed initial state which is the root of all state space exploration.
		*			It can explore the entire reachable state space or only until a target is reached.
		*/
		template <class Move_Engine_T, class Positions_Of_Pieces_T>
		class distance_exploration {

		public:
			using move_engine_type = Move_Engine_T;

			using positions_of_pieces_type = Positions_Of_Pieces_T;

			using piece_move_type = typename move_engine_type::piece_move_type;

			using cell_id_type = typename positions_of_pieces_type::cell_id_type;

			using cell_id_narrow_int = typename cell_id_type::int_cell_id_type;


			using size_type = std::size_t;

			static constexpr size_type SIZE_TYPE_MAX{ std::numeric_limits<size_type>::max() };

			struct move_candidate {

				/** The move consisting of piece_id and direction. */
				piece_move_type move;

				/** The successor state */
				positions_of_pieces_type successor_state;

				move_candidate(const piece_move_type& move, positions_of_pieces_type successor) : move(move), successor_state(successor) {}

			};

			using distance_int = uint8_t;

			static constexpr distance_int NOT_REACHED{ std::numeric_limits<distance_int>::max() };

			using states_vector = std::vector<positions_of_pieces_type>;

			static constexpr std::size_t STATE_BYTE_SIZE{ positions_of_pieces_type::byte_size() };

			using unexplored_map = std::map<distance_int, states_vector>;
		
			using target_cache_entry = std::pair<distance_int, states_vector>;

			using target_cache = std::vector<target_cache_entry>;

		private:

			//using target_distance_map_type = std::map<cell_id_type, size_type>;


			/**
			* conditions:
			*	- guaranteed to have .size() > 0
			*	- the first entry _reachable_states_by_distance[0] has always length 1 and contains the initial state
			*	- the vector at index i contains exactly the states which are reachable by an optimal path of length i
			*/
			//std::vector<states_vector> _reachable_states_by_distance;

			// states_vector _unexplored_leafs;


			/**
			*	@brief Mapping each state to its optimal distance from initial state (seen so far) or NOT_REACHED otherwise
			*/
			byte_tree_map<positions_of_pieces_type, distance_int> _distance_map;

			/**
			*	@brief unexplored states classified by their optimal distance from initial state
			*/
			unexplored_map _unexplored_by_depth; // use this to replace _unexplored_leafs and also _max_exploration_depth

			/**
			*	@brief 
			*/
			std::size_t _max_exploration_depth_updated;

			//std::size_t _max_exploration_depth;


			/**
			*	@brief The number of states reached so far.
			*	@details Must always be the number of non-default values in _distance_map.
			*/
			std::size_t _reached_states_counter;


			/**
			* maps target cells to their minimal distance from initial state
			*/
			// target_distance_map_type _optimal_path_length_cache;


			/**
			*	@brief Caches optimal distances and optimal final states for target cells.
			*
			*	@details Every cell is mapped to its optimal distance from initial state or NOT_REACHED and a vector of those final states you reach by an optimal path.
			*		Must always be of size n where {0, ... ,n-1 } are the cell ids, available on the board.
			*/
			target_cache _updated_target_distance_cache;


			/**
			*	@brief Returns cache entry for given cell_id i.e. index. Throws std::logic_error in case index is out of range.
			*/
			inline target_cache_entry& access_target_cache(cell_id_narrow_int cell_id) {
				if (!(cell_id < _updated_target_distance_cache.size())) {
					throw std::logic_error("Cell ID out of range");
				}
				return _updated_target_distance_cache[cell_id];
			}

			/**
			*	@brief Updates target_distance_cache if indicated.
			*
			*	@details Checks if given state at given depth brings any new optimal state to any contained target cell.
			*
			*	@return Returns 0 if cache was not updated, 1 if added new state to already known optimal depth, 3 if found new optimal depth for some target cell.
			*/
			inline uint8_t update_target_cache(const positions_of_pieces_type& state, distance_int depth) {
				uint8_t found_update{ 0 };
				for (auto iter = state.target_pieces_cbegin(); iter != state.target_pieces_cend(); ++iter) {
					target_cache_entry& entry{ access_target_cache(iter->get_id()) };

					if (depth < entry.first) {
						entry.first = depth;
						entry.second.clear();
						found_update |= 0b10;
					}
					if (depth == entry.first) {
						entry.second.push_back(state); // does not check for duplicate states!!! ####
						found_update |= 0b01;
					}
				}
				return found_update;
			}

			/**
			*	@brief Updates distance map for given state and depth if this combination is optimal as far as explored. Also updates cache for new target cells.
			*
			*	@return Returns true if map was updated, since the given state, depth combination is optimal as far as seen.
			*		Returns false if map stays untouched, since this state is already in map with equal or smaller depth.
			*/
			inline bool update_distance_map(const positions_of_pieces_type& state, distance_int depth) {
				distance_int& map_val{ _distance_map[state] };

				if (depth < map_val) {
					if (map_val == NOT_REACHED) {
						++_reached_states_counter;
					}
					map_val = depth;
					update_target_cache(state, depth);
					return true;
				}
				return false;
			}

			/**
			*	@brief Explores for direct successor states of \p current_state which have not yet been seen with shorter or equal depth.
			*
			*	@details Adds those successor states of \p current_state to \p destination_further_exploration_leafs which are not in _distance_map or where we found a shorter depth than _distance_map says.
			*	Also adds newly found states or states with enhanced depth to _distance_map and updates _target_cache.
			*/
			template<class Iterator_T>
			inline void add_successors_to_map_and_obtain_further_exploration_leafs(
				const move_engine_type& engine,
				const positions_of_pieces_type& current_state,
				distance_int current_state_depth,
				Iterator_T destination_further_exploration_leafs
			) {
				// compute all successor state candidates:
				for (typename piece_move_type::piece_id_type::int_type pid = 0; pid < positions_of_pieces_type::COUNT_ALL_PIECES; ++pid) {
					for (direction direction_iter = direction::begin(); direction_iter < direction::end(); ++direction_iter) {

						const auto succ = engine.successor_state(current_state, pid, direction_iter);

						if (succ == current_state)
							continue;

						if (update_distance_map(succ, current_state_depth + 1)) {
							*destination_further_exploration_leafs = succ;
							++destination_further_exploration_leafs;
						}
					}
				}
			}


			/**
			*	@brief Explores for direct successor states of \p current_state which have not yet been seen with shorter or equal depth.
			*
			*	@details Adds those successor states of \p current_state to \p destination_further_exploration_leafs which are not in _distance_map or where we found a shorter depth than _distance_map says.
			*	Also adds newly found states or states with enhanced depth to _distance_map and updates _target_cache.
			*
			*	@return Returns true if and only if a target state was found.
			*/
			template<class Iterator_T>
			[[deprecated]] inline bool __deleted__add_successors_to_map_and_obtain_further_exploration_leafs(
				const move_engine_type& engine,
				const positions_of_pieces_type& current_state,
				distance_int current_state_depth,
				const cell_id_type& target_cell,
				Iterator_T destination_further_exploration_leafs
			) {
				// we can delete this method, we are constantly updating our cache. Therefore we can just run for all successors and ask the cache afterwards if we found a final state.

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

					if (update_distance_map(candidates_for_successor_states[index_candidate].successor_state, current_state_depth + 1)) {
						*destination_further_exploration_leafs = (candidates_for_successor_states[index_candidate].successor_state);
						++destination_further_exploration_leafs;
					}
				}
				// add successor states to destination without check for final state:
				for (; index_candidate < candidates_for_successor_states.size(); ++index_candidate) {
					if (candidates_for_successor_states[index_candidate].successor_state == current_state) {
						continue; // it is not clear if this check makes performance better or worse than without ###
					}

					if (update_distance_map(candidates_for_successor_states[index_candidate].successor_state, current_state_depth + 1)) {
						*destination_further_exploration_leafs = (candidates_for_successor_states[index_candidate].successor_state);
						++destination_further_exploration_leafs;
					}

				}
				return found_final_state;
			}


			/// hier weiter ueberarbeiten... ########################################################################################################################################

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


				//const size_type INDEX_LAST_EXPLORATION{ _reachable_states_by_distance.size() - 1 };

				size_type optimal_depth{ SIZE_TYPE_MAX }; // guaranteed not yet found if NOT_YET_FOUND_GUARANTEED == true

				//size_type states_counter{ count_states() };

				for (
					/*size_type expand_level_index{INDEX_LAST_EXPLORATION}*/
					;
					_max_exploration_depth < optimal_depth // NOT OK!!! here ####
					&& _max_exploration_depth < policy.max_depth() /* policy abort*/
					&& count_states() < policy.state_count_threshold() /* policy abort*/
					;
					//++expand_level_index
					) {


					if (entirely_explored()) {
						return SIZE_TYPE_MAX; // no more states to find /// ########size tpye versus internal depth type conflict here!
					}

					states_vector _unexplored_leafs_next;

					for (std::size_t i = 0; i < _unexplored_leafs.size(); ++i) {

						if (add_successors_to_map_and_obtain_further_exploration_leafs(engine, _unexplored_leafs[i], _max_exploration_depth, target_cell, std::back_inserter(_unexplored_leafs_next))) {
							optimal_depth = this->_max_exploration_depth + 1;
						}
						// delete that overload of add_successors_to_map_and_obtain_further_exploration_leafs and ask the cache afterwards if for target cell we have a finite distance, then it is optimal.

					}

					_unexplored_leafs = _unexplored_leafs_next;
					++_max_exploration_depth;
					// I dont like that this has to be updated here consistently.
					// This is not forcing the programmer to avoid an error of not incrementing _max_exploration_depth
					// we should pass depth of current state as param to add_successors_to_map_and_obtain_further_exploration_leafs.
					// unexplored_leafs should be a map depth -> unexplored of that depth.
					// then we keep track of the correct depth with less coding error porbability
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
			distance_exploration(const positions_of_pieces_type& initial_state, const move_engine_type& engine) :
				_optimal_path_length_cache(),
				_distance_map(NOT_REACHED),
				_unexplored_leafs(),
				_max_exploration_depth(0),
				_reached_states_counter(1),
				_updated_target_distance_cache(engine.board().count_cells(), std::make_pair(NOT_REACHED, states_vector()))
			{
				_distance_map[initial_state] = 0;
				_unexplored_leafs.push_back(initial_state);
			}

			/**
			*	@brief Returns the total number of states reached from initial state during exploration, including the initial state itself.
			*/
			inline size_type count_states() const noexcept { return _reached_states_counter; }

			/**
			*	@brief Returns true if and only if the entire state space has been explored.
			*/
			inline bool entirely_explored() const noexcept { return _unexplored_leafs.empty(); }

			/**
			*	@brief Returns the max depth of previously executed exploration.
			*/
			inline size_type exploration_depth() const noexcept { return _max_exploration_depth; }

			/**
			*	@brief Explores according to \p policy (until entirely explored or until running into some policy threshold)
			*/
			inline void explore(
				const move_engine_type& engine,
				const exploration_policy& policy
			) {
				while (
					_max_exploration_depth < policy.max_depth() /* policy abort */
					&&
					count_states() < policy.state_count_threshold() /* policy abort */
					&&
					!entirely_explored()
					)
				{
					states_vector _unexplored_leafs_next;

					for (std::size_t i = 0; i < _unexplored_leafs.size(); ++i) {
						add_successors_to_map_and_obtain_further_exploration_leafs(engine, _unexplored_leafs[i], _max_exploration_depth, std::back_inserter(_unexplored_leafs_next));
					}

					_unexplored_leafs = _unexplored_leafs_next;
					++_max_exploration_depth;
				}
			}

			/**
			*	@brief Explores until reaching \p target_cell (without any restriction on exploration depth)
			*/
			inline size_type explore_until_target(const move_engine_type& engine, const cell_id_type& target_cell) {
				return optimal_path_length(engine, target_cell, exploration_policy::FORCE_EXPLORATION_UNRESTRICTED());
			}

			/**
			*	@brief Explores until reaching \p target_cell, restricted to max exploration depth \p max_depth
			*/
			inline size_type explore_until_target(const move_engine_type& engine, const cell_id_type& target_cell, const size_type& max_depth) {
				return optimal_path_length(engine, target_cell, exploration_policy::FORCE_EXPLORATION_UNTIL_DEPTH(max_depth));
			}

			/**
			*	@brief Explores until reaching \p target_cell, if allowed by \p policy. The policy determines if it performs additional exploration or if it only looks up in previously cached or explored solutions.
			*
			*	@return Returns the optimal path length for reaching \p target_cell. Returns SIZE_TYPE_MAX in case no optimal path was found, perhaps due to \p policy.
			*/
			inline size_type optimal_path_length(const move_engine_type& engine, const cell_id_type& target_cell, const exploration_policy& policy = exploration_policy::ONLY_EXPLORED()) {
				// checking cache...
				//const auto iter = _optimal_path_length_cache.find(target_cell);

				//if (iter != _optimal_path_length_cache.cend()) {
				//	return iter->second;
				//}

				const target_cache_entry& entry{ access_target_cache(target_cell.get_id()) };

				if (entry.first != NOT_REACHED) {
					return entry.first;
				}

				if (policy == exploration_policy::ONLY_CASHED()) {
					return SIZE_TYPE_MAX;
				}

				// Note that our cache caches everything explored so far.
				// we do not have to look for already explored but not yet cached 

				if (policy == exploration_policy::ONLY_EXPLORED()) {
					return SIZE_TYPE_MAX;
				}

				// checking explored states...

				// here we need to iterate over
				//_distance_map;
				// and build back the states to check if it matches given target cell.

				// hint does not make sense anymore for iterating this structure.


				//for (auto iter = _distance_map.cbegin(); iter != _distance_map.cend(); ++iter) {

					//...

					// dont do this. Switch to cache-on-exploration now!!!!

				//}

				/*
				for (size_type depth{ min_length_hint }; depth < _reachable_states_by_distance.size(); ++depth) {
					for (const auto& state : _reachable_states_by_distance[depth]) {
						if (state.is_final(target_cell)) {
							if (min_length_hint == 0) { // only update cache if there was no hint
								_optimal_path_length_cache.insert(std::make_pair(target_cell, depth));

								// consider building the cache while exploring new states, cache should not be a std::map, instead a vector with const time access!
								// then manual lookup here is not necessary, since target either in cache or not reached.
							}
							return depth;
						}
					}
				}
				*/




				// further exploration...
				return explore_until_target(engine, target_cell, policy, true);
			}

			/**
			*	@brief Explores until reaching \p target_cell, if allowed by \p policy. The policy determines if it performs additional exploration or if it only looks up in previously cached or explored solutions.
			*
			*	@return Returns all final states covering \p target_cell.
			*/
			inline std::vector<positions_of_pieces_type> optimal_final_states(move_engine_type& engine, const cell_id_type& target_cell, const exploration_policy& policy = exploration_policy::ONLY_EXPLORED(), const size_type& min_length_hint = 0) {
				std::vector<positions_of_pieces_type> result;
				const size_type DEPTH{ optimal_path_length(engine, target_cell, policy) };

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

				const size_type FINAL_DEPTH{ optimal_path_length(engine, target_cell, policy) };

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
