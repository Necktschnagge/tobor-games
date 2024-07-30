#pragma once

#include "models/augmented_positions_of_pieces.h"
#include "engine/quick_move_cache.h"

#include "engine/move_engine.h"
#include "models/simple_state_bigraph.h"


#include "default_models_1_1.h"

#include "solver_1_0.h"

#include <map>
#include <set>
#include <array>
#include <algorithm>
#include <limits>
#include <execution>
#include <numeric>

namespace tobor {

	namespace v1_1 {


		using default_quick_move_cache = quick_move_cache<default_dynamic_rectangle_world>;

		using default_move_engine = move_engine<default_min_size_cell_id, default_quick_move_cache, default_piece_move>;



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

			using size_type = std::size_t;

			static constexpr size_type SIZE_TYPE_MAX{ std::numeric_limits<size_type>::max() };

			struct move_candidate {

				/** The move consisting of piece_id and direction. */
				piece_move_type move;

				/** The successor state */
				positions_of_pieces_type successor_state;

				move_candidate(const piece_move_type& move, positions_of_pieces_type successor) : move(move), successor_state(successor) {}

			};

			/**
			*	@brief Describes restrictions for exploration.
			*/
			class exploration_policy {

				size_type _state_count_threshold{ 0 };
				size_type _max_depth{ 0 };

				constexpr  exploration_policy(size_type state_count_threshold, size_type max_depth) : _state_count_threshold(state_count_threshold), _max_depth(max_depth) {}

			public:
				exploration_policy() = delete;

				static constexpr exploration_policy ONLY_CASHED() { return exploration_policy(0, 0); }

				static constexpr exploration_policy ONLY_EXPLORED() { return exploration_policy(0, SIZE_TYPE_MAX); }

				static constexpr exploration_policy FORCE_EXPLORATION_UNRESTRICTED() { return exploration_policy(SIZE_TYPE_MAX, SIZE_TYPE_MAX); }

				static constexpr exploration_policy FORCE_EXPLORATION_UNTIL_DEPTH(size_type max_depth) { return exploration_policy(SIZE_TYPE_MAX, max_depth); }

				static constexpr exploration_policy FORCE_EXPLORATION_STATE_THRESHOLD(size_type state_count_threshold) { return exploration_policy(std::max(state_count_threshold, size_type(1)), SIZE_TYPE_MAX); }

				static constexpr exploration_policy FORCE_EXPLORATION_STATE_THRESHOLD_UNTIL_DEPTH(size_type state_count_threshold, size_type max_depth) { return exploration_policy(std::max(state_count_threshold, size_type(1)), max_depth); }

				inline bool operator ==(const exploration_policy& another) const noexcept { return _state_count_threshold == another._state_count_threshold && _max_depth == another._max_depth; }

				inline size_type state_count_threshold() const { return _state_count_threshold; }

				inline size_type max_depth() const { return _max_depth; }

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
			target_distance_map_type _optimal_path_length_map;

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
			*	@return true if and only if a taregt state was found.
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
			*/
			inline size_type explore_until_target(
				const move_engine_type& engine,
				const cell_id_type& target_cell,
				const exploration_policy& policy,
				const bool NOT_YET_FOUND_GUARANTEED = false
			) {
				const size_type INDEX_LAST_EXPLORATION{ _reachable_states_by_distance.size() - 1 };

				size_type optimal_depth{ SIZE_TYPE_MAX }; // guaranteed not yet found if NOT_YET_FOUND_GUARANTEED == true

				size_type states_counter{ count_states() };

				for (size_type expand_level_index{ INDEX_LAST_EXPLORATION };
					expand_level_index < optimal_depth
					&& expand_level_index < policy.max_depth() /* policy abort*/
					&& states_counter < policy.state_count_threshold() /* policy abort*/;
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
						_optimal_path_length_map.insert(std::make_pair(target_cell, optimal_depth));
					}
				return optimal_depth;
			}

		public:
			/**
			*	@brief Constructs an object with empty exploration state space.
			*/
			distance_exploration(const positions_of_pieces_type& initial_state) :
				_optimal_path_length_map(),
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
			*	@brief Returns the may depth of previously executed exploration.
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

				size_type states_counter{ count_states() };

				for (
					size_type expand_level_index{ INDEX_LAST_EXPLORATION };
					expand_level_index < policy.max_depth() && states_counter < policy.state_count_threshold() /* policy abort*/;
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
				const auto iter = _optimal_path_length_map.find(target_cell);

				if (iter != _optimal_path_length_map.cend()) {
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
								_optimal_path_length_map.insert(std::make_pair(target_cell, depth));
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

		/**
		*	@brief Represents a path by its states.
		*/
		template<class Position_Of_Pieces_T>
		class state_path {
		public:

			using positions_of_pieces_type = Position_Of_Pieces_T;

			using vector_type = std::vector<positions_of_pieces_type>;

		private:

			vector_type _state_vector;

		public:

			state_path() {}

			state_path(const vector_type& v) : _state_vector(v) {}

			const vector_type& vector() const { return _state_vector; }

			vector_type& vector() { return _state_vector; }

			inline void make_canonical() {

				typename vector_type::size_type count_duplicates{ 0 };
				typename vector_type::size_type i = 0;

				while (i + count_duplicates + 1 < _state_vector.size()) {
					if (_state_vector[i] == _state_vector[i + count_duplicates + 1]) {
						++count_duplicates;
					}
					else {
						if (count_duplicates)
							_state_vector[i + 1] = _state_vector[i + count_duplicates + 1];
						++i;
					}
				}

				// now i + count_duplicates + 1 == _state_vector.size()
				_state_vector.erase(_state_vector.begin() + i + 1, _state_vector.end());
			}

			inline state_path operator +(const state_path& another) const {
				state_path copy{ *this };
				std::copy(another._state_vector.cbegin(), another._state_vector.cend(), std::back_inserter(copy._state_vector));
				return copy;
			}

			inline state_path operator +(const positions_of_pieces_type& s) const {
				state_path copy{ *this };
				copy.vector().push_back(s);
				return copy;
			}

			inline state_path& operator +=(const state_path& another) {
				_state_vector.reserve(_state_vector.size() + another._state_vector.size());
				std::copy(another._state_vector.cbegin(), another._state_vector.cend(), std::back_inserter(_state_vector));
				return *this;
			}

			inline state_path& operator +=(const positions_of_pieces_type& s) {
				_state_vector.push_back(s);
				return *this;
			}

			inline state_path operator *(const state_path& another) {
				if (another._state_vector.empty())
					return *this;
				if (this->_state_vector.empty())
					return another;
				if (_state_vector.back() == another._state_vector.front()) {
					state_path copy = *this;
					std::copy(
						another._state_vector.cbegin() + 1,
						another._state_vector.cend(),
						std::back_inserter(copy._state_vector)
					);
					return copy;
				}
				else {
					return state_path();
				}
			}

		};

		/**
		*	@brief Represents a path by its moves.
		*/
		template<class Piece_Move_T>
		class move_path {

		public:
			using piece_move_type = Piece_Move_T;

			using vector_type = std::vector<piece_move_type>;

			using pieces_quantity_type = typename piece_move_type::pieces_quantity_type;

			using pieces_quantity_int_type = typename pieces_quantity_type::int_type;

		private:
			vector_type _move_vector;

		public:

			move_path() {}

			move_path(std::size_t n) : _move_vector(n, piece_move_type()) {}

			move_path(const move_path&) = default;

			move_path& operator=(const move_path&) = default;

			move_path(move_path&&) = default;

			move_path& operator=(move_path&&) = default;

			template<class Position_Of_Pieces_T, class Move_Engine_T>
			explicit move_path(const state_path<Position_Of_Pieces_T>& s_path, const Move_Engine_T& move_engine) {
				for (std::size_t i{ 0 }; i + 1 < s_path.vector().size(); ++i) {
					_move_vector.emplace_back(move_engine.state_minus_state(s_path.vector()[i + 1], s_path.vector()[i]));
				}
			}

			template <class Pieces_Quantity_T, class Cell_Id_Type_T, bool SORTED_TARGET_PIECES_V, bool SORTED_NON_TARGET_PIECES_V, class Move_Engine_T>
			inline static move_path extract_unsorted_move_path(
				const state_path<augmented_positions_of_pieces<Pieces_Quantity_T, Cell_Id_Type_T, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>>& augmented_state_path,
				const Move_Engine_T& move_engine
			)
			{
				using augmented_positions_of_pieces_type = augmented_positions_of_pieces<Pieces_Quantity_T, Cell_Id_Type_T, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>;
				using permutation_type = typename augmented_positions_of_pieces_type::permutation_type;
				using permutation_indexing_type = typename permutation_type::size_type;

				static_assert(std::is_same<permutation_indexing_type, std::size_t>::value, "MSVC does not meet en.cppreference.com");

				move_path result;

				for (std::size_t i{ 0 }; i + 1 < augmented_state_path.vector().size(); ++i) {
					result._move_vector.emplace_back(
						move_engine.state_minus_state(augmented_state_path.vector()[i + 1], augmented_state_path.vector()[i]));
					// roll back permutation
					auto piece_id = result._move_vector.back().pid.value;
					auto permutation_of_piece_id = augmented_state_path.vector()[i].permutation()[piece_id];

					/*is it checked somewhere that no out of range can happen? ### */
					result._move_vector.back().pid.value = static_cast<pieces_quantity_int_type>(permutation_of_piece_id);
					//result._move_vector.back().pid = static_cast<decltype(result._move_vector.back().pid)>(permutation_of_piece_id);
				}

				return result;
			}

			vector_type& vector() { return _move_vector; }

			const vector_type& vector() const { return _move_vector; }

			template<class Position_Of_Pieces_T, class Move_Engine_T>
			[[nodiscard]] inline state_path<Position_Of_Pieces_T> apply(const Position_Of_Pieces_T& initial_state, const Move_Engine_T& move_engine) const {
				state_path<Position_Of_Pieces_T> result;
				result.vector().reserve(_move_vector.size() + 1);
				result.vector().push_back(initial_state);

				for (std::size_t i{ 0 }; i < _move_vector.size(); ++i) {
					result.vector().push_back(move_engine.successor_state(result.vector().back(), _move_vector[i]));
				}

				return result;
			}

			inline move_path operator +(const move_path& another) {
				move_path copy;
				copy._move_vector.reserve(_move_vector.size() + another._move_vector.size());
				std::copy(_move_vector.cbegin(), _move_vector.cend(), std::back_inserter(copy._move_vector));
				std::copy(another._move_vector.cbegin(), another._move_vector.cend(), std::back_inserter(copy._move_vector));
				return copy;
			}

			inline bool operator==(const move_path& another) const {
				return _move_vector == another._move_vector;
			}

			inline bool operator<(const move_path& another) const {
				return _move_vector < another._move_vector;
			}

			inline std::vector<move_path> syntactic_interleaving_neighbours() {
				if (_move_vector.size() < 2) {
					return std::vector<move_path>();
				}

				auto result = std::vector<move_path>(_move_vector.size() - 1, *this);
				auto iter = result.begin();
				for (std::size_t i{ 0 }; i + 1 < _move_vector.size(); ++i) {
					if (!(_move_vector[i] == _move_vector[i + 1])) {
						std::swap(iter->_move_vector[i], iter->_move_vector[i + 1]);
						++iter;
					}
				}
				result.erase(iter, result.end());

				return result;
			}

			inline move_path color_sorted_footprint() const {
				auto result = move_path(*this);

				std::stable_sort(
					result.vector().begin(),
					result.vector().end(),
					[](const piece_move_type& left, const piece_move_type& right) { return left.pid < right.pid; }
				);

				return result;
			}

			inline bool is_interleaving_neighbour(const move_path& another) const {
				if (vector().size() != another.vector().size()) {
					return false;
				}

				typename vector_type::size_type i{ 0 };

				while (i + 1 < vector().size()) { // looking for the switched positions i, i+1

					if (!(vector()[i] == another.vector()[i])) {
						// here it must be switched i, i+1 and the rest must be equal to return true...

						return
							vector()[i] == another.vector()[i + 1] &&
							vector()[i + 1] == another.vector()[i] &&
							std::equal(
								vector().cbegin() + i + 2,
								vector().cend(),
								another.vector().cbegin() + i + 2
							);
					}

					++i;
				}
				return false;
			}

			inline static std::vector<std::vector<move_path>> interleaving_partitioning_improved(const std::vector<move_path>& paths) {
				std::vector<std::vector<move_path>> equivalence_classes;

				using pair_type = std::pair<move_path, uint8_t>; // divide this into two vectors(?)

				using flagged_paths_type = std::vector<pair_type>;
				using flagged_paths_iterator = typename flagged_paths_type::iterator;


				//static constexpr uint8_t EXPLORED{ 0b10 };
				static constexpr uint8_t REACHED{ 0b01 };

				flagged_paths_type flagged_paths;
				flagged_paths.reserve(paths.size());
				std::transform(paths.cbegin(), paths.cend(), std::back_inserter(flagged_paths), [](const move_path& mp) { return std::make_pair(mp, 0); });

				std::sort(flagged_paths.begin(), flagged_paths.end()); // lexicographical sorting of paths by piece_id, then direction.

				flagged_paths_iterator remaining_end{ flagged_paths.end() };

				while (flagged_paths.begin() != remaining_end) { // while there are path not yet put into some equivalence class
					std::size_t diff = remaining_end - flagged_paths.begin();
					(void)diff;
					equivalence_classes.emplace_back();
					auto& equiv_class{ equivalence_classes.back() };
					equiv_class.reserve(remaining_end - flagged_paths.begin());

					flagged_paths.front().second = REACHED;
					equiv_class.push_back(flagged_paths.front().first);

					std::set<std::size_t> indices_to_explore;

					indices_to_explore.insert(0);

					while (!indices_to_explore.empty()) {

						std::size_t current_exploration_index = *indices_to_explore.cbegin();
						indices_to_explore.erase(indices_to_explore.cbegin());

						std::vector<move_path> neighbour_candidates = flagged_paths[current_exploration_index].first.syntactic_interleaving_neighbours();

						std::sort(neighbour_candidates.begin(), neighbour_candidates.end()); // lex sorting of move paths.

						flagged_paths_iterator search_begin{ flagged_paths.begin() };

						for (auto& candidate : neighbour_candidates) {

							search_begin = std::lower_bound( // find in sorted vector
								search_begin,
								remaining_end,
								std::make_pair(candidate, std::size_t(0)),
								[](const auto& l, const auto& r) {
									return l.first < r.first;
								}
							);

							if (search_begin == remaining_end) {
								break;
							}

							if (search_begin->first == candidate && !(search_begin->second & REACHED)) {
								// if found candidate and not reached before

								equiv_class.emplace_back(candidate);
								search_begin->second |= REACHED;
								indices_to_explore.insert(search_begin - flagged_paths.begin());
							}

						}
					}

					remaining_end = std::remove_if(
						flagged_paths.begin(),
						remaining_end,
						[](const pair_type& pair) {
							return pair.second & REACHED;
						}
					);
					equiv_class.shrink_to_fit();
				}
				if (paths.size() != flagged_paths.size()) {
					auto x = paths.size() - flagged_paths.size();
					(void)x;
				}

				return equivalence_classes;
			}

			inline static std::vector<std::vector<move_path>> interleaving_partitioning(const std::vector<move_path>& paths) {

				static constexpr bool USE_IMPROVEMENT{ true };

				if constexpr (USE_IMPROVEMENT) {
					return interleaving_partitioning_improved(paths);
				}
				else {
					std::vector<std::vector<move_path>> equivalence_classes;

					for (const auto& p : paths) {

						std::vector<std::size_t> indices; // all indices of matching equivalence classes
						for (std::size_t i{ 0 }; i < equivalence_classes.size(); ++i) {
							auto& ec{ equivalence_classes[i] };
							for (const auto& el : ec) {
								if (el.is_interleaving_neighbour(p)) {
									indices.push_back(i);
									break;
								}
							}
						}

						if (indices.empty()) {
							equivalence_classes.emplace_back();
							equivalence_classes.back().push_back(p);
						}
						else {
							equivalence_classes[indices[0]].emplace_back(p);
							for (std::size_t j = indices.size() - 1; j != 0; --j) {
								std::copy(
									equivalence_classes[indices[j]].cbegin(),
									equivalence_classes[indices[j]].cend(),
									std::back_inserter(equivalence_classes[indices[0]])
								);
								equivalence_classes.erase(equivalence_classes.begin() + indices[j]);
							}
						}
					}
					return equivalence_classes;
				}
			}

			std::size_t changes() const {
				std::size_t counter{ 0 };
				for (std::size_t i = 0; i + 1 < _move_vector.size(); ++i) {
					counter += !(_move_vector[i].pid == _move_vector[i + 1].pid);
				}
				return counter;
			}

			inline static bool antiprettiness_relation(const move_path& l, const move_path& r) {
				return l.changes() < r.changes();
			}

		};

		/**
		*	@brief Utility class for classifying paths into equivalence classes.
		*/
		template<class Positions_Of_Pieces_T>
		class path_classificator {

			inline static bool contains(const std::vector<bool>& flag_vector, std::size_t flag_index) {
				return (flag_index < flag_vector.size()) && flag_vector[flag_index];
			}

			inline static void set_flag(std::vector<bool>& flag_vector, std::size_t flag_index, bool value = true, bool default_value = false) {
				if (flag_index < flag_vector.size()) {
					flag_vector[flag_index] = true;
					return;
				}
				const std::size_t insert_size{ flag_index + 1 - flag_vector.size() };
				flag_vector.insert(flag_vector.end(), insert_size, default_value);
				flag_vector[flag_index] = value;
			}

		public:

			using positions_of_pieces_type = Positions_Of_Pieces_T;

			using state_path_type = state_path<positions_of_pieces_type>;

			using state_path_vector_type = typename state_path_type::vector_type;

		private:

			/**
			*	@brief Extracts all state paths of given simple_state_bigraph \p source with prefix \p depth_first_path and writes them into \p all_state_paths using emplace_back()
			*/
			template<class State_Label_T>
			static void extract_all_state_paths_helper(
				const simple_state_bigraph<positions_of_pieces_type, State_Label_T>& source,
				std::vector<state_path<positions_of_pieces_type>>& all_state_paths,
				state_path_vector_type& depth_first_path
			) {
				auto iter = source.map.find(depth_first_path.back());

				if (iter == source.map.cend()) {
					return; // Never reached by logic when used correctly.
					// should throw std::unexpected();
				}

				if (
					iter->second.successors.empty()
					) {
					all_state_paths.emplace_back(depth_first_path);
					return;
				}
				for (const auto& succ : iter->second.successors) {
					depth_first_path.push_back(succ);
					extract_all_state_paths_helper(source, all_state_paths, depth_first_path);
					depth_first_path.pop_back();
				}
			}

		public:

			/**
			*	@brief Augments the states of \p bigraph by flags indicating all the equivalence classes a state belongs to.
			*
			*	@return Number of partitions found.
			*/
			static std::size_t make_state_graph_path_partitioning(simple_state_bigraph<positions_of_pieces_type, std::vector<bool>>& bigraph) {

				// ## This function needs to be fixed in order to include crossed components of equivalence classes (which are left in this version.)

				/*
				std::vector<position_of_pieces_type> initials;
				std::vector<position_of_pieces_type> finals;

				for (auto& entry : bigraph.map) {
					entry.second.labels.clear();
					if (entry.second.predecessors.empty()) {
						initials.push_back(entry.first);
					}
					if (entry.second.successors.empty()) {
						finals.push_back(entry.first);
					}
				}
				*/

				std::size_t flag_index{ 0 };
				for (auto iter = bigraph.map.begin(); iter != bigraph.map.end(); ++iter) {
					/* while there is a state not being part of any path partition */
					if (iter->second.labels.empty()) {
						// found iter pointing to a state not belonging to any partition/ i.e. has no label

						std::vector<decltype(bigraph.map.begin())> exploration_iterator_stack; // collect iterators for elements in partition
						exploration_iterator_stack.reserve(bigraph.map.size());
						exploration_iterator_stack.push_back(iter);

						// add new label to *iter state and to all state on some initial path.
						set_flag(iter->second.labels, flag_index, true);

						{
							auto i_back = iter;
							while (!i_back->second.predecessors.empty()) { // can be optimized
								i_back = bigraph.map.find(*i_back->second.predecessors.begin());
								// i_back != end() /* assured by logic, also check it here (?)*/
								if (i_back == bigraph.map.end()) break;
								set_flag(i_back->second.labels, flag_index, true);
								exploration_iterator_stack.push_back(i_back);
							}
						}
						{
							auto i_forward = iter;
							while (!i_forward->second.successors.empty()) { // can be optimized
								i_forward = bigraph.map.find(*i_forward->second.successors.begin());
								if (i_forward == bigraph.map.end()) break;
								// i_forward != end() /* assured by logic, also check it here (?)*/
								set_flag(i_forward->second.labels, flag_index, true);
								exploration_iterator_stack.push_back(i_forward);
							}
						}

						// add all other states to this partition which can be reached by true interleaving:

						/*
						Theory:
							A new element can be found from two common states embracing an interleaving
								A	---->	B1
								-			-
								-			-
								->			->
								B2	---->	C

							wlog we already know B1 but not B2. B2 can be found exploring from A and exploring from C

							use the following algorithm
							Put all known states into a stack.
							while stack not empty pop and explore from that element, always explore in both directions
								if found any new state of same partition, push it onto the stack.
							loop until stack empty.

							This way all states of the same partition will be found.

							::PROOF:: fairly simple, using contradiction:

							Assume Partition = A setunion B
							where all A have been found, but none of B.
							Since all of b are reachable by statewise interleaving from A,
							there must be some b2 and a1 ---> a2 ---> a3 such that also a1 ---> b2 ---> a3.
							So assuming b2 was not found when the stack got empty.
							This means that when a3 was popped, at this moment a1 has not yet been found.
							Otherwise exploration would have found also b2.
							So a1 was found after a3 was popped.
							a1 explored after a1 found, a1 found after a3 popped, a3 popped after a3 found.
							Thus, a1 explored after a3 found.
							Hence b2 was found when a1 got explored. Contradiction.
						*/
						while (!exploration_iterator_stack.empty()) {
							auto exploree = exploration_iterator_stack.back();
							exploration_iterator_stack.pop_back();

							for (const auto& candidate : exploree->second.successors) {
								const auto i_candidate = bigraph.map.find(candidate);
								if (i_candidate == bigraph.map.end()) continue; // never happens by logic if bigraph is sound.
								if (contains(i_candidate->second.labels, flag_index)) continue; // state already labeled as part of current equivalence class

								for (const auto& successor : i_candidate->second.successors) {
									auto i_successor = bigraph.map.find(successor);
									if (i_successor == bigraph.map.end()) continue; // never happens by logic if bigraph is sound.

									if (contains(i_successor->second.labels, flag_index)) {
										if (exploree->first.count_changed_pieces(i_successor->first) == 2) { // true interleaving
											// accept candidate here:
											exploration_iterator_stack.push_back(i_candidate);
											set_flag(i_candidate->second.labels, flag_index);
											goto candidate_accepted_1;
										}
									}
								}
							candidate_accepted_1:
								(void)0;
							}
							for (const auto& candidate : exploree->second.predecessors) {
								const auto i_candidate = bigraph.map.find(candidate);
								if (i_candidate == bigraph.map.end()) continue; // never happens by logic if bigraph is sound.
								if (contains(i_candidate->second.labels, flag_index)) continue; // state already labeled as part of current equivalence class

								for (const auto& predecessor : i_candidate->second.predecessors) {
									auto i_predecessor = bigraph.map.find(predecessor);
									if (i_predecessor == bigraph.map.end()) continue; // never happens by logic if bigraph is sound.

									if (contains(i_predecessor->second.labels, flag_index)) {
										if (exploree->first.count_changed_pieces(i_predecessor->first) == 2) { // true interleaving
											// accept candidate here:
											exploration_iterator_stack.push_back(i_candidate);
											set_flag(i_candidate->second.labels, flag_index);
											goto candidate_accepted_2;
										}
									}
								}
							candidate_accepted_2:
								(void)0;
							}
						}

						++flag_index;
					}

				}

				return flag_index;
			}

			
			/**
			*	@brief Extracts the induced subgraph by the states with flag \p label_index. The extracted subgraph then shows oen single equivalence class.
			*/
			template<class State_Label_T>
			static void extract_subgraph_by_label(
				const simple_state_bigraph<positions_of_pieces_type, std::vector<bool>>& source,
				std::size_t label_index,
				simple_state_bigraph<positions_of_pieces_type, State_Label_T>& destination
			) {
				destination.map.clear();

				const auto has_label{
					[&](const positions_of_pieces_type& e) {
					auto pos = source.map.find(e);
					const bool found_label = pos != source.map.end() && contains(pos->second.labels, label_index);
					return found_label;
					}
				};

				for (const auto& pair : source.map) {
					if (contains(pair.second.labels, label_index)) {
						auto iter = destination.map.insert(
							destination.map.end(),
							std::make_pair(pair.first, typename simple_state_bigraph<positions_of_pieces_type, State_Label_T>::node_links())
						);
						std::copy_if(pair.second.predecessors.cbegin(), pair.second.predecessors.cend(), std::inserter(iter->second.predecessors, iter->second.predecessors.end()), has_label);
						std::copy_if(pair.second.successors.cbegin(), pair.second.successors.cend(), std::inserter(iter->second.successors, iter->second.successors.end()), has_label);
					}
				}
			}

			/**
			*	@brief Returns all state paths of given simple_state_bigraph \p source.
			*/
			template<class State_Label_T>
			static std::vector<state_path<positions_of_pieces_type>> extract_all_state_paths(const simple_state_bigraph<positions_of_pieces_type, State_Label_T>& source) {
				std::vector<state_path<positions_of_pieces_type>> all_state_paths;

				for (auto iter = source.map.cbegin(); iter != source.map.cend(); ++iter) {
					state_path_vector_type depth_first_path{ iter->first };
					if (iter->second.predecessors.empty()) {
						extract_all_state_paths_helper(source, all_state_paths, depth_first_path);
					}
				}
				return all_state_paths;
			}
		};

		class bigraph_operations {
		public:



			/**
			*	@brief requires single initial state in source bigraph
			*/
			template<
				class Source_State_T,
				class Source_Decoration_T,
				class Destination_State_T,
				class Destination_Decoration_T,
				class Cell_Id_T,
				class Quick_Move_Cache_T,
				class Piece_Move_T
			>
				requires std::same_as<typename Source_State_T::cell_id_type, Cell_Id_T>
			&&
				std::same_as<typename Destination_State_T::cell_id_type, Cell_Id_T>

				struct bigraph_simulation_copy {

				using source_bigraph_type = tobor::v1_1::simple_state_bigraph<Source_State_T, Source_Decoration_T>;
				using destination_bigraph_type = tobor::v1_1::simple_state_bigraph<Destination_State_T, Destination_Decoration_T>;

				using source_map_const_iterator = typename source_bigraph_type::map_const_iterator_type;
				using destination_map_iterator = typename destination_bigraph_type::map_iterator_type;

				using source_state_set_const_iterator = typename source_bigraph_type::state_set_type::const_iterator;

				struct simulation_copy_df_record {

					source_map_const_iterator source_map_it;
					source_state_set_const_iterator source_succ_it;
					destination_map_iterator destination_map_it;

				};

				using simulation_links_type = std::map<Source_State_T, std::set<Destination_State_T>>;

				bigraph_simulation_copy() = delete;

				inline static destination_map_iterator copy(
					const source_bigraph_type& source_bigraph,
					destination_bigraph_type& destination_bigraph,
					const Destination_State_T& initial_state_destination,
					const move_engine<Cell_Id_T, Quick_Move_Cache_T, Piece_Move_T>& engine
				) {
					destination_bigraph.clear();

					// find the initial state in source bigraph:
					source_map_const_iterator source_initial_state_iterator{ source_bigraph.map.cend() };
					for (auto iter = source_bigraph.map.cbegin(); iter != source_bigraph.map.cend(); ++iter) {
						if (iter->second.predecessors.empty()) {
							if (source_initial_state_iterator != source_bigraph.map.cend()) {
								throw 0; // error: more than 1 initial state
							}
							source_initial_state_iterator = iter;
						}
					}

					destination_map_iterator iter_destination_map_initial_state = destination_bigraph.map.insert(
						destination_bigraph.map.begin(),
						std::make_pair(initial_state_destination, typename destination_bigraph_type::node_links())
					);

					//auto iter = source_initial_state_iterator;

					simulation_links_type simulation_links; // should also be part of return value


					std::vector<simulation_copy_df_record> df_exploration_stack;
					df_exploration_stack.emplace_back();

					df_exploration_stack.back().source_map_it = source_initial_state_iterator;
					df_exploration_stack.back().source_succ_it = source_initial_state_iterator->second.successors.cbegin();
					df_exploration_stack.back().destination_map_it = iter_destination_map_initial_state;

					simulation_links[source_initial_state_iterator->first].insert(iter_destination_map_initial_state->first);


					while (!df_exploration_stack.empty()) {
						simulation_copy_df_record& current{ df_exploration_stack.back() };

						if (current.source_succ_it == current.source_map_it->second.successors.cend()) {
							// all successors have been explored here
							df_exploration_stack.pop_back();
							continue;
						}

						const auto& source_curr_state{ current.source_map_it->first };
						const auto& source_succ_state{ *current.source_succ_it };
						auto& destination_curr_state{ current.destination_map_it->first };

						++current.source_succ_it;

						Piece_Move_T move{ engine.state_minus_state(source_succ_state, source_curr_state) };

						Destination_State_T similar_successor_state = engine.successor_state(destination_curr_state, move);


						//auto& destination_succ_map_value = destination_bigraph.map[similar_successor_state]; // insert successor state, if not present
						auto [destination_succ_map_iter, inserted_destination_succ] = destination_bigraph.map.insert(
							std::make_pair(similar_successor_state, typename destination_bigraph_type::node_links())
						); // insert successor state, if not present

						//destination_succ_map_value.predecessors.insert(destination_curr_state); // insert predecessor state, if not present
						destination_succ_map_iter->second.predecessors.insert(destination_curr_state); // insert predecessor state, if not present

						current.destination_map_it->second.successors.insert(similar_successor_state); // insert successor state, if not present

						auto [pos, insert_took_place] = simulation_links[source_succ_state].insert(similar_successor_state);

						if (insert_took_place) {
							// found a new simulation_link, so recursive df exploration has to be made there:
							df_exploration_stack.emplace_back(); // invalidates current
							simulation_copy_df_record& sub{ df_exploration_stack.back() };

							sub.source_map_it = source_bigraph.map.find(source_succ_state); // ### not yet checked for end iterator!!!
							sub.source_succ_it = sub.source_map_it->second.successors.cbegin();
							sub.destination_map_it = destination_succ_map_iter;
						}

					}


					return iter_destination_map_initial_state;
				}
			};

		};


	}

}


