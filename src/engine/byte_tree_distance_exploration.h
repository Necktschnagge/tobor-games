#pragma once

/*
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
byte_tree_distance_exploration
*/

#include "exploration_policy.h"

#include "../models/simple_state_bigraph.h"

#include "../fsl/container/byte_tree_map.h"

#include <vector>

namespace tobor {
	namespace v1_1 {

		/**
		*	@brief A state space explorer for game boards. This one is based on byte_tree_map, a multi-level finite branching tree inplace map structure.
		*	@details It always has a fixed initial state which is the root of all state space exploration.
		*			It can explore the entire reachable state space or until a target is reached or until some threshold.
		*/
		template <class Move_Engine_T, class Positions_Of_Pieces_T>
		class byte_tree_distance_exploration {

		public:
			using move_engine_type = Move_Engine_T;

			using positions_of_pieces_type = Positions_Of_Pieces_T;

			using pieces_quantity_type = typename positions_of_pieces_type::pieces_quantity_type;

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
			fsl::byte_tree_map<positions_of_pieces_type, distance_int> _distance_map;

			/**
			*	@brief unexplored states classified by their optimal distance from initial state
			*
			*	@details Must always contain at least one entry for the last depth not beeing explored deeper.
			*	If entirely explored, and the farest found state is at depth x, then must contain an entry (x+1, {empty vector}).
			*	It means depth x has been explored, leafs of x+1 are empty.
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
			*	@brief Explores one level deeper.
			*
			*	@return Returns 4 if _unexplored_by_depth empty
						Returns 2 if leading unexplored vector is empty but there are more than one.
						Returns 1 if leading unexplored vector is empty and is the only vector, means entirely explored
						Returns 0 if explored a non-empty leading vector of unexplored states.
			*/
			inline uint8_t explore_one_level(const move_engine_type& engine) {
				if (_unexplored_by_depth.empty()) {
					return 0b100; // should not be reached
				}

				const auto begin{ _unexplored_by_depth.begin() };

				if (begin->second.empty()) {
					if (_unexplored_by_depth.size() <= 1) {
						return 0b001;
					}
					// should not be reached:
					_unexplored_by_depth.erase(begin);
					return 0b010;
				}

				states_vector& destination{ _unexplored_by_depth[begin->first + 1] };

				for (std::size_t i = 0; i < begin->second.size(); ++i) {
					add_successors_to_map_and_obtain_further_exploration_leafs(engine, begin->second[i], begin->first, std::back_inserter(destination));
				}

				_unexplored_by_depth.erase(begin);
				return 0b000;
			}

		public:

			/**
			*	@brief Constructs an object with empty exploration state space.
			*/
			byte_tree_distance_exploration(const positions_of_pieces_type& initial_state, const move_engine_type& engine) :
				_distance_map(NOT_REACHED),
				_unexplored_by_depth(),
				_max_exploration_depth_updated(),
				_reached_states_counter(0),
				_updated_target_distance_cache(engine.board().count_cells(), std::make_pair(NOT_REACHED, states_vector()))
			{
				update_distance_map(initial_state, 0); // to also update the target cache
				_unexplored_by_depth[0].push_back(initial_state);
			}

			/**
			*	@brief Returns the total number of states reached from initial state during exploration, including the initial state itself.
			*/
			inline size_type count_states() const noexcept { return _reached_states_counter; }

			/**
			*	@brief Returns true if and only if the entire state space has been explored.
			*/
			inline bool entirely_explored() const noexcept {
				for (auto iter = _unexplored_by_depth.cbegin(); iter != _unexplored_by_depth.cend(); ++iter) {
					if (!(iter->second.empty())) {
						return false;
					}
				}
				return true;
			}

			/**
			*	@brief Returns the max depth of previously executed exploration.
			*/
			inline size_type exploration_depth() const noexcept {
				if (_unexplored_by_depth.empty()) {
					return SIZE_TYPE_MAX; // should never be reached
				}
				return _unexplored_by_depth.cbegin()->first;
			}

			/**
			*	@brief Explores according to \p policy (until entirely explored or until running into some policy threshold)
			*/
			inline void explore(
				const move_engine_type& engine,
				const exploration_policy& policy
			) {
				while (
					exploration_depth() < policy.max_depth() /* policy abort */
					&&
					count_states() < policy.state_count_threshold() /* policy abort */
					&&
					!entirely_explored()
					)
				{
					explore_one_level(engine);
				}
			}

			/**
			*	@brief Explores according to \p policy until reaching \p target_cell (or until running into policy threshold or untile entirely explored)
			*/
			inline distance_int explore_until_target(
				const move_engine_type& engine,
				const cell_id_type& target_cell,
				const exploration_policy& policy
			) {
				while ( /* no break condition fulfilled */
					exploration_depth() < policy.max_depth() // also filteres ONLY_CASHED and ONLY_EXPLORED here
					&&
					count_states() < policy.state_count_threshold()
					&&
					!(access_target_cache(target_cell.get_id()).first < NOT_REACHED)
					&&
					!entirely_explored()
					) {
					// explore one depth deeper.
					explore_one_level(engine);
				}

				return access_target_cache(target_cell.get_id()).first;

			}

			/**
			*	@brief Explores until reaching \p target_cell (without any restriction on exploration depth)
			*
			*	@return Returns the optimal path length for reaching \p target_cell. Returns SIZE_TYPE_MAX in case no optimal path was found, perhaps due to \p policy.
			*
			*/
			inline distance_int explore_until_target(const move_engine_type& engine, const cell_id_type& target_cell) {

				return explore_until_target(engine, target_cell, exploration_policy::FORCE_EXPLORATION_UNRESTRICTED());
				//### does not return SIZE_MAX in not reached case!

			}

			/**
			*	@brief Explores until reaching \p target_cell, restricted to max exploration depth \p max_depth_exploration
			*	@return Returns the optimal path length for reaching \p target_cell. Returns SIZE_TYPE_MAX in case no optimal path was found, perhaps due to \p policy.
			*/
			inline distance_int explore_until_target(const move_engine_type& engine, const cell_id_type& target_cell, const size_type& max_depth_exploration) {

				return explore_until_target(engine, target_cell, exploration_policy::FORCE_EXPLORATION_UNTIL_DEPTH(max_depth_exploration));
				//### does not return SIZE_MAX in not reached case!

			}

			/**
			*	@brief Explores until reaching \p target_cell, if allowed by \p policy. The policy determines if it performs additional exploration or if it only looks up in previously explored solutions.
			*
			*	@return Returns all final states covering \p target_cell.
			*/
			inline states_vector optimal_final_states(const move_engine_type& engine, const cell_id_type& target_cell, const exploration_policy& policy = exploration_policy::ONLY_EXPLORED()) {

				explore_until_target(engine, target_cell, policy);
				return access_target_cache(target_cell.get_id()).second;

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
				const exploration_policy& policy = exploration_policy::ONLY_EXPLORED()
			) {
				using bigraph = simple_state_bigraph<positions_of_pieces_type, State_Label_T>;

				destination.clear();

				states_vector final_states = optimal_final_states(engine, target_cell, policy);
				std::sort(final_states.begin(), final_states.end());

				const distance_int FINAL_DEPTH{ explore_until_target(engine, target_cell, policy) }; // double explore here, policy can be depth-0

				if (FINAL_DEPTH == NOT_REACHED || final_states.empty()) { // should be none or both true
					return;
				}


				for (const auto& state : final_states) {
					destination.map.insert(
						destination.map.end(), // are these states sorted? no!, hint is useless ##### -> changed... sorted at the beginning
						std::pair<typename bigraph::state_type, typename bigraph::node_links>(
							state,
							typename bigraph::node_links()
						)
					);
				}

				distance_int backward_explore_distance = FINAL_DEPTH;
				states_vector states = final_states;

				while (backward_explore_distance > 0) {
					--backward_explore_distance;

					// move on here!!!

					std::vector<std::pair<positions_of_pieces_type, positions_of_pieces_type>> possible_edges;

					possible_edges.reserve(states.size() * pieces_quantity_type::COUNT_ALL_PIECES * 4);

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

					possible_edges.erase(
						std::remove_if(possible_edges.begin(), possible_edges.end(), [&](const std::pair<positions_of_pieces_type, positions_of_pieces_type>& edge) {
							return _distance_map.get(edge.first) != backward_explore_distance;
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
