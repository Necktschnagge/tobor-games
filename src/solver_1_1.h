#pragma once

#include "models_1_1.h"
#include "solver_1_0.h"

#include <map>
#include <array>
#include <algorithm>
#include <limits>
#include <execution>

class GameController; // for friend class declaration, to be removed

namespace tobor {

	namespace v1_1 {

		// ### alternative board representation is just two sorted vectors of indices where there are walls, additionally you can uniformly encode pieces like walls, in this case double walls left and right, or top and bottom.


		/**
		* @brief Class for keeping the information about quick jumps
		*
		* @details For a board, it stores for each cell the information which are the cells to move to on one step [west, east, south, north] until wall, assuming that there are no pieces on the way.
		*/
		template<class World_Type_T>
		class quick_move_cache {
		public:

			using world_type = World_Type_T;

			using cell_id_int_type = typename world_type::int_cell_id_type;
			using int_size_type = typename world_type::int_size_type;

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
				const int_size_type VECTOR_SIZE{ board.count_cells() };

				if (!(VECTOR_SIZE > 0)) {
					return;
				}

				go_west = std::vector<cell_id_int_type>(VECTOR_SIZE, 0);
				go_east = std::vector<cell_id_int_type>(VECTOR_SIZE, 0);
				go_south = std::vector<cell_id_int_type>(VECTOR_SIZE, 0);
				go_north = std::vector<cell_id_int_type>(VECTOR_SIZE, 0);

				{
					go_west[0] = 0;
					go_south[0] = 0;

					cell_id_int_type id = 0;
					while (static_cast<int_size_type>(id) + 1 < VECTOR_SIZE) {
						++id;
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
				}
				{
					go_east[VECTOR_SIZE - 1] = VECTOR_SIZE - 1;
					go_north[VECTOR_SIZE - 1] = VECTOR_SIZE - 1;

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
			}

			/**
			* @brief Returns the id of the cell you reach from cell \p id when moving west with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			inline cell_id_int_type get_west(cell_id_int_type id) const { return go_west[id]; }

			/**
			* @brief Returns the id of the cell you reach from cell \p id when moving east with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			inline cell_id_int_type get_east(cell_id_int_type id) const { return go_east[id]; }

			/**
			* @brief Returns the transposed id of the cell you reach from cell \p transposed_id when moving south with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			inline cell_id_int_type get_south(cell_id_int_type transposed_id) const { return go_south[transposed_id]; }

			/**
			* @brief Returns the transposed id of the cell you reach from cell \p transposed_id when moving north with no pieces on the way.
			* @details Has undefined behavior if \p id is out of range. Valid range is [ 0, board.count_cells() - 1 ]
			*/
			inline cell_id_int_type get_north(cell_id_int_type transposed_id) const { return go_north[transposed_id]; }

			inline cell_id_int_type get(const direction& d, cell_id_int_type raw_id) const {
				switch (d.get())
				{
				case direction::encoding::NORTH:
					return get_north(raw_id);
				case direction::encoding::EAST:
					return get_east(raw_id);
				case direction::encoding::SOUTH:
					return get_south(raw_id);
				case direction::encoding::WEST:
					return get_west(raw_id);
				default:
					return raw_id;
				}
			}
		};

		using default_quick_move_cache = quick_move_cache<default_dynamic_rectangle_world>;

		/**
		*	@brief Calculates successor states by calcualting successor cell of single pieces for moving in a given direction.
		*/
		template<class Cell_Id_Type, class Quick_Move_Cache_T, class Piece_Move_Type>
		class move_one_piece_calculator { // logic engine // move_engine
		public:

			//using positions_of_pieces_type = Position_Of_Pieces_T; // remove #### this from template. every fucntion should be template to use different pop types over the same pieces quantity.
			// for every template we must check type compatibility with world (---> int types) and pieces quantity (--> vector length, )

			using quick_move_cache_type = Quick_Move_Cache_T;

			using world_type = typename quick_move_cache_type::world_type;

			using cell_id_type = Cell_Id_Type;

			using cell_id_int_type = typename cell_id_type::int_cell_id_type;

			using piece_move_type = Piece_Move_Type;

			using piece_id_type = typename piece_move_type::piece_id_type;

			//static_assert(
			//	std::is_same<typename positions_of_pieces_type::world_type, typename quick_move_cache_type::world_type>::value,
			//	"Incompatible template arguments. typename Position_Of_Pieces_T::world_type must equal typenname Quick_Move_Cache_T::world_type"
			//	);

			//static_assert(
			//	std::is_same<typename positions_of_pieces_type::pieces_quantity_type, typename piece_move_type::pieces_quantity_type>::value,
			//	"Incompatible template arguments. typename Position_Of_Pieces_T::pieces_quantity_type must equal typenname Piece_Move_Type::pieces_quantity_type"
			//	);

			struct arithmetic_error {

				struct no_move {
					std::vector<piece_move_type> zero_moves;
				};

				struct multi_move {
					std::vector<piece_move_type> zero_moves;
				};
			};

		private:

			const world_type& my_world; // remove this, add member function returning the reference inside cache!!! #####

			quick_move_cache_type cache;

		public:

			/**
			* @brief Constructs a move_one_piece_calculator.
			*
			* @details \p my_word must not be changed externally. This is constructing a quick_move_cache inside which would be invalidated.
			*/
			move_one_piece_calculator(const world_type& my_world) : my_world(my_world), cache(my_world) {
			}

			using id_getter_type = cell_id_int_type(cell_id_type::*)(const world_type&);
			using cell_id_creator = cell_id_type(*)(cell_id_int_type, const world_type&);
			using cache_direction_getter = cell_id_int_type(quick_move_cache_type::*)(cell_id_int_type);

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving in given direction until any obstacle (wall or piece).
			*/
			template<class Position_Of_Pieces_Type>
			inline cell_id_int_type next_cell_max_move_raw(
				const cell_id_int_type& raw_start_cell_id,
				const Position_Of_Pieces_Type& state,
				const id_getter_type& get_raw_id,
				const cache_direction_getter& get_cache_direction
			) const {
				cell_id_int_type raw_next_cell_id{ (cache.*get_cache_direction)(raw_start_cell_id) };

				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					const cell_id_int_type current_raw_id{ (state.piece_positions[i].*get_raw_id)(my_world) };

					if (raw_start_cell_id < current_raw_id && current_raw_id <= raw_next_cell_id) {
						raw_next_cell_id = current_raw_id - 1;
					}

					if (raw_start_cell_id > current_raw_id && current_raw_id >= raw_next_cell_id) {
						raw_next_cell_id = current_raw_id + 1;
					}
				}

				return raw_next_cell_id;
			}


			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving in given direction until any obstacle (wall or piece).
			*/
			template<class Position_Of_Pieces_Type>
			inline cell_id_int_type next_cell_max_move_raw(
				const cell_id_int_type& raw_start_cell_id,
				const Position_Of_Pieces_Type& state,
				const direction& d
			) const {
				cell_id_int_type raw_next_cell_id{ cache.get(d, raw_start_cell_id) };

				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					const cell_id_int_type current_raw_id{ state.piece_positions[i].get_raw_id(d, my_world) };

					if (raw_start_cell_id < current_raw_id && current_raw_id <= raw_next_cell_id) {
						raw_next_cell_id = current_raw_id - 1;
					}

					if (raw_start_cell_id > current_raw_id && current_raw_id >= raw_next_cell_id) {
						raw_next_cell_id = current_raw_id + 1;
					}
				}

				return raw_next_cell_id;
			}


			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving in given direction until any obstacle (wall or piece).
			*/
			template<class Position_Of_Pieces_Type>
			inline cell_id_type next_cell_max_move(
				const cell_id_type& start_cell,
				const Position_Of_Pieces_Type& state,
				const id_getter_type& get_raw_id,
				const cell_id_creator& create_cell_id_by,
				const cache_direction_getter& get_cache_direction
			) const {
				const cell_id_int_type raw_start_cell_id{ (start_cell.*get_raw_id)(my_world) };

				const cell_id_int_type raw_next_cell_id{ next_cell_max_move_raw(raw_start_cell_id, state, get_raw_id,get_cache_direction) };

				return create_cell_id_by(raw_next_cell_id, my_world);
			}

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving in given direction until any obstacle (wall or piece).
			*/
			template<class Position_Of_Pieces_Type>
			inline cell_id_type next_cell_max_move(
				const cell_id_type& start_cell,
				const Position_Of_Pieces_Type& state,
				const direction& d
			) const {
				const cell_id_int_type raw_start_cell_id{ start_cell.get_raw_id(d,my_world) };

				const cell_id_int_type raw_next_cell_id{ next_cell_max_move_raw(raw_start_cell_id, state, d) };

				return cell_id_type::create_by_raw_id(d, raw_next_cell_id, my_world);
			}

#if false
			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving in given direction until any obstacle (wall or piece).
			*/
			template<id_getter_type get_raw_id,
				cell_id_creator create_cell_id_by,
				cache_direction_getter get_cache_direction
			>
			inline cell_id_type static_next_cell_max_move(
				const cell_id_type& start_cell,
				const positions_of_pieces_type& state
			) const {
				const cell_id_int_type raw_start_cell_id{ (start_cell.*get_raw_id)(my_world) };

				const cell_id_int_type raw_next_cell_id{ next_cell_max_move_raw(raw_start_cell_id, state, get_raw_id,get_cache_direction) };

				return create_cell_id_by(raw_next_cell_id, my_world);
			}
#endif

			template<class Position_Of_Pieces_Type>
			inline std::vector<Position_Of_Pieces_Type> predecessor_states(
				const Position_Of_Pieces_Type& state,
				const typename piece_move_type::piece_id_type& _piece_id,
				const direction& _direction_from
			) const {
				const cell_id_type start_cell{ state.piece_positions[_piece_id.value] };

				const cell_id_int_type raw_start_cell_id{ start_cell.get_raw_id(_direction_from, my_world) };

				auto raw_far_id = next_cell_max_move_raw(
					raw_start_cell_id,
					state,
					!_direction_from
				);

				auto result = std::vector<Position_Of_Pieces_Type>(
					raw_start_cell_id > raw_far_id ?
					raw_start_cell_id - raw_far_id :
					raw_far_id - raw_start_cell_id,
					state
				);

				const int8_t increment{ (raw_start_cell_id > raw_far_id) - (raw_start_cell_id < raw_far_id) };

				auto iter = result.begin();
				for (cell_id_int_type raw_id = raw_far_id; raw_id != raw_start_cell_id; raw_id += increment, ++iter) {
					iter->piece_positions[_piece_id.value] = cell_id_type::create_by_raw_id(_direction_from, raw_id, my_world);
					iter->sort_pieces();
				}
				return result;
			}

			template<class Position_Of_Pieces_Type>
			inline std::vector<Position_Of_Pieces_Type> predecessor_states(
				const Position_Of_Pieces_Type& state,
				const typename piece_move_type::piece_id_type& _piece_id
			) const {
				auto result = std::vector<Position_Of_Pieces_Type>();
				result.reserve(static_cast<std::size_t>(my_world.get_horizontal_size() + my_world.get_vertical_size()) * 2);
				for (direction d = direction::begin(); d != direction::end(); ++d) {
					auto part = predecessor_states(state, _piece_id, d);
					std::copy(std::begin(part), std::end(part), std::back_inserter(result));
				}
				result.shrink_to_fit();
				return result;
			}

			template<class Position_Of_Pieces_Type>
			inline std::vector<Position_Of_Pieces_Type> predecessor_states(const Position_Of_Pieces_Type& state) const {
				auto result = std::vector<Position_Of_Pieces_Type>();
				result.reserve(
					static_cast<std::size_t>(my_world.get_horizontal_size() + my_world.get_vertical_size()) * 2 * piece_id_type::pieces_quantity_type::COUNT_ALL_PIECES
				);
				for (piece_id_type piece = piece_id_type::begin(); piece != piece_id_type::end(); ++piece) {
					auto part = predecessor_states(state, piece);
					std::copy(std::begin(part), std::end(part), std::back_inserter(result));
				}
				result.shrink_to_fit();
				return result;
			} // should add a function returning void but taking an insert iterator

			template<class Position_Of_Pieces_Type>
			inline Position_Of_Pieces_Type successor_state(const Position_Of_Pieces_Type& state, const typename piece_move_type::piece_id_type& _piece_id, const direction& _direction) const {
				Position_Of_Pieces_Type result(state);

				result.piece_positions[_piece_id.value] = next_cell_max_move(state.piece_positions[_piece_id.value], state, _direction);
				result.sort_pieces();

				return result;
			}

			template<class Position_Of_Pieces_Type>
			inline std::pair<Position_Of_Pieces_Type, bool> successor_state(const Position_Of_Pieces_Type& state, const piece_move_type& move) const {
				return successor_state(state, move.pid, move.dir);
			}

			template<class Position_Of_Pieces_Type>
			inline Position_Of_Pieces_Type state_plus_move(const Position_Of_Pieces_Type& state, const piece_move_type& move) const {
				return successor_state(state, move).first;
			}

			template<class Position_Of_Pieces_Type>
			inline piece_move_type state_minus_state(const Position_Of_Pieces_Type& to_state, const Position_Of_Pieces_Type& from_state) const {
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

		using default_move_one_piece_calculator = move_one_piece_calculator<default_positions_of_pieces, default_quick_move_cache, default_piece_move>;

		template<class State_Type, class State_Label_Type = void>
		class simple_state_bigraph;

		template<class State_Type>
		class simple_state_bigraph<State_Type, void> {
		public:

			using state_type = State_Type;
			using state_label_type = void;

			struct node_links {
				std::set<state_type> predecessors;
				std::set<state_type> successors;
			};

			using map_type = std::map<state_type, node_links>;

			map_type map;
		};

		template<class State_Type, class State_Label_Type>
		class simple_state_bigraph {
		public:
			using state_type = State_Type;

			using state_label_type = State_Label_Type;

			struct node_links {
				std::set<state_type> predecessors;
				std::set<state_type> successors;
				state_label_type labels;
			};

			using map_type = std::map<state_type, node_links>;

			map_type map;

			inline void clear() {
				return map.clear();
			}
		};

		//template<uint64_t MAX_DISTANCE, uint64_t MAX_WIDTH>
		class indexing_backward_graph {
		public:
			//using positions_of_pieces_type = Positions_Of_Pieces_Type;

			template<class Distance_Int, class Index_Int>
			struct index_edge {
				Distance_Int d_from;
				Index_Int i_from;
				Index_Int i_to;

				friend auto operator <=>(const index_edge& l, const index_edge& r) = default;
			};

			std::vector<index_edge<uint8_t, uint64_t>> edges;

			inline void sort() {
				std::sort(edges.begin(), edges.end());
			}

		};


		template <class Move_One_Piece_Calculator, class Positions_Of_Pieces_Type>
		class distance_exploration {

		public:
			using move_one_piece_calculator_type = Move_One_Piece_Calculator;

			using positions_of_pieces_type = Positions_Of_Pieces_Type; // check compatibility with Move_One_Piece_Calc! #### via static assert 

			using piece_move_type = typename move_one_piece_calculator_type::piece_move_type;

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

				static constexpr exploration_policy FORCE_EXPLORATION_STATE_THRESHOLD(size_type state_count_threshold) { return exploration_policy(std::max(state_count_threshold, 1), SIZE_TYPE_MAX); }

				static constexpr exploration_policy FORCE_EXPLORATION_STATE_THRESHOLD_UNTIL_DEPTH(size_type state_count_threshold, size_type max_depth) { return exploration_policy(std::max(state_count_threshold, 1), max_depth); }

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
			*/
			std::vector<states_vector> _reachable_states_by_distance;

			// number of steps needed by any optimal solution
			//size_type _optimal_path_length;

			/**
			* maps target cells to their minimal distance from initial state
			*/
			target_distance_map_type _optimal_path_length_map;

			bool _entirely_explored{ false };

			inline void sort_unique(const typename std::vector<states_vector>::size_type& index /* new states index */) {
				std::sort(std::execution::par, _reachable_states_by_distance[index].begin(), _reachable_states_by_distance[index].end());
				_reachable_states_by_distance[index].erase(
					unique(std::execution::par, _reachable_states_by_distance[index].begin(), _reachable_states_by_distance[index].end()),
					_reachable_states_by_distance[index].end()
				);
			}

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

			template<class Iterator_Type>
			inline void add_all_nontrivial_successor_states(
				const move_one_piece_calculator_type& engine,
				const positions_of_pieces_type& current_state,
				Iterator_Type destination
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

			template<class Iterator_Type>
			inline bool add_all_nontrivial_successor_states(
				const move_one_piece_calculator_type& engine,
				const positions_of_pieces_type& current_state,
				const cell_id_type& target_cell,
				Iterator_Type destination
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

				/* order of candidates:
				 piece 0: N E S W      <- target pieces come first!
				 piece 1: N E S W
				 ...
				 piece last: N E S W
				*/


				typename std::vector<move_candidate>::size_type index_candidate{ 0 };

				// only check if reached goal for candidates arising from moved target pieces:
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
			* Caller guarantees that target_cell has not yet been found if NOT_YET_FOUND_GUARANTEED == true.
			*/
			inline size_type explore_until_target(
				const move_one_piece_calculator_type& engine,
				const cell_id_type& target_cell,
				const exploration_policy& policy,
				const bool NOT_YET_FOUND_GUARANTEED = false
			) {
				const size_type INDEX_LAST_EXPLORATION{ _reachable_states_by_distance.size() - 1 };

				size_type optimal_depth{ SIZE_TYPE_MAX }; // guaranteed not yet found if  NOT_YET_FOUND_GUARANTEED == true

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
			distance_exploration(const positions_of_pieces_type& initial_state) :
				//_optimal_path_length(SIZE_TYPE_MAX)
				_optimal_path_length_map(),
				_entirely_explored(false)
			{
				_reachable_states_by_distance.emplace_back(std::vector<positions_of_pieces_type>{ initial_state });

			}

			inline size_type count_states() const noexcept {
				return std::accumulate(
					std::begin(_reachable_states_by_distance),
					std::end(_reachable_states_by_distance),
					size_type(0),
					[](const size_type& acc, const auto& el) { return acc + el.size(); });
			}

			inline bool entirely_explored() const noexcept { return _entirely_explored; }

			inline size_type exploration_depth() const noexcept { return _reachable_states_by_distance.size() - 1; }

			/**
			*
			*/
			inline void explore(
				move_one_piece_calculator_type& engine,
				const exploration_policy& policy
			) {
				const size_type INDEX_LAST_EXPLORATION{ _reachable_states_by_distance.size() - 1 };

				size_type states_counter{ count_states() };

				for (
					size_type expand_level_index{ INDEX_LAST_EXPLORATION };
					expand_level_index < policy._max_depth && states_counter < policy._state_count_threshold /* policy abort*/;
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

			inline size_type explore_until_target(const move_one_piece_calculator_type& engine, const cell_id_type& target_cell) {
				return optimal_path_length(engine, target_cell, exploration_policy::FORCE_EXPLORATION_UNRESTRICTED(), 0);
			}

			inline size_type explore_until_target(const move_one_piece_calculator_type& engine, const cell_id_type& target_cell, const size_type& max_depth) {
				return optimal_path_length(engine, target_cell, exploration_policy::FORCE_EXPLORATION_UNTIL_DEPTH(max_depth), 0);
			}

			inline size_type optimal_path_length(const move_one_piece_calculator_type& engine, const cell_id_type& target_cell, const exploration_policy& policy = exploration_policy::ONLY_EXPLORED(), const size_type& min_length_hint = 0) {
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




			inline std::vector<positions_of_pieces_type> optimal_final_states(move_one_piece_calculator_type& engine, const cell_id_type& target_cell, const exploration_policy& policy = exploration_policy::ONLY_EXPLORED(), const size_type& min_length_hint = 0) {
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

			template<class State_Label_Type>
			void get_simple_bigraph(
				const move_one_piece_calculator_type& engine,
				const cell_id_type& target_cell,
				simple_state_bigraph<positions_of_pieces_type, State_Label_Type>& destination,
				const exploration_policy& policy = exploration_policy::ONLY_EXPLORED(),
				const size_type& min_length_hint = 0
			) {
				using bigraph = simple_state_bigraph<positions_of_pieces_type, State_Label_Type>;

				destination.clear();

				const size_type FINAL_DEPTH{ optimal_path_length(engine, target_cell, policy, min_length_hint) };

				if (!(FINAL_DEPTH < _reachable_states_by_distance.size()))
					return;

				std::vector<positions_of_pieces_type> states;

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

			// ### offer step-wise exploration instead of exploration until optimal.
		};

		template<class Position_Of_Pieces_T = default_positions_of_pieces>
		class state_path {
		public:

			using positions_of_pieces_type = Position_Of_Pieces_T;

			using vector_type = std::vector<positions_of_pieces_type>;

		private:

			vector_type state_vector;

		public:

			state_path() {}

			state_path(const vector_type& v) : state_vector(v) {}

			const vector_type& vector() const { return state_vector; }

			vector_type& vector() { return state_vector; }

			inline void make_canonical() {

				typename vector_type::size_type count_duplicates{ 0 };
				typename vector_type::size_type i = 0;

				while (i + count_duplicates + 1 < state_vector.size()) {
					if (state_vector[i] == state_vector[i + count_duplicates + 1]) {
						++count_duplicates;
					}
					else {
						if (count_duplicates)
							state_vector[i + 1] = state_vector[i + count_duplicates + 1];
						++i;
					}
				}

				// now i + count_duplicates + 1 == state_vector.size()
				state_vector.erase(state_vector.begin() + i + 1, state_vector.end());
			}

			inline state_path operator +(const state_path& another) const {
				state_path copy{ *this };
				std::copy(another.state_vector.cbegin(), another.state_vector.cend(), std::back_inserter(copy.state_vector));
				return copy;
			}

			inline state_path operator +(const positions_of_pieces_type& s) const {
				state_path copy{ *this };
				copy.vector().push_back(s);
				return copy;
			}

			inline state_path& operator +=(const state_path& another) {
				state_vector.reserve(state_vector.size() + another.state_vector.size());
				std::copy(another.state_vector.cbegin(), another.state_vector.cend(), std::back_inserter(state_vector));
				return *this;
			}

			inline state_path& operator +=(const positions_of_pieces_type& s) {
				state_vector.push_back(s);
				return *this;
			}

			inline state_path operator *(const state_path& another) {
				if (another.state_vector.empty())
					return *this;
				if (this->state_vector.empty())
					return another;
				if (state_vector.back() == another.state_vector.front()) {
					state_path copy = *this;
					std::copy(
						another.state_vector.cbegin() + 1,
						another.state_vector.cend(),
						std::back_inserter(copy.state_vector)
					);
					return copy;
				}
				// ### error case of non-matching paths is missing here!
			}

		};

		template<class Piece_Move_Type = default_piece_move>
		class move_path {

		public:
			using piece_move_type = Piece_Move_Type;

			using vector_type = std::vector<piece_move_type>;

			using pieces_quantity_type = typename piece_move_type::pieces_quantity_type;

		private:
			vector_type move_vector;

		public:

			move_path() {}

			move_path(std::size_t n) : move_vector(n, piece_move_type()) {}

			move_path(const move_path&) = default;

			move_path& operator=(const move_path&) = default;

			move_path(move_path&&) = default;

			move_path& operator=(move_path&&) = default;

			template<class Position_Of_Pieces_Type, class Move_Once_Piece_Calculator_Type>
			explicit move_path(const state_path<Position_Of_Pieces_Type>& s_path, const Move_Once_Piece_Calculator_Type& move_engine) {
				for (std::size_t i{ 0 }; i + 1 < s_path.vector().size(); ++i) {
					move_vector.emplace_back(move_engine.state_minus_state(s_path.vector()[i + 1], s_path.vector()[i]));
				}
			}

			template <class Pieces_Quantity_Type, class Cell_Id_Type_T, bool SORTED_TARGET_PIECES_V, bool SORTED_NON_TARGET_PIECES_V, class Move_Once_Piece_Calculator_Type>
			inline static move_path extract_unsorted_move_path(
				const state_path<augmented_positions_of_pieces<Pieces_Quantity_Type, Cell_Id_Type_T, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>>& augmented_state_path,
				const Move_Once_Piece_Calculator_Type& move_engine
			)
			{
				move_path result;

				for (std::size_t i{ 0 }; i + 1 < augmented_state_path.vector().size(); ++i) {
					result.move_vector.emplace_back(
						move_engine.state_minus_state(augmented_state_path.vector()[i + 1], augmented_state_path.vector()[i]));
					// roll back permutation
					result.move_vector.back().pid = augmented_state_path.vector()[i].get_permutation()[result.move_vector.back().pid.value];
				}

				return result;
			}

			vector_type& vector() { return move_vector; }

			const vector_type& vector() const { return move_vector; }

			template<class Position_Of_Pieces_Type, class Move_Once_Piece_Calculator_Type>
			inline state_path<Position_Of_Pieces_Type> apply(const Position_Of_Pieces_Type& initial_state, const Move_Once_Piece_Calculator_Type& move_engine) {
				state_path<Position_Of_Pieces_Type> result;
				result.vector().reserve(move_vector.size() + 1);
				result.vector().push_back(initial_state);

				for (std::size_t i{ 0 }; i < move_vector.size(); ++i) {
					result.vector().push_back(move_engine.state_plus_move(result.vector().back(), move_vector[i]));
				}

				return result;
			}

			inline move_path operator +(const move_path& another) {
				move_path copy;
				copy.move_vector.reserve(move_vector.size() + another.move_vector.size());
				std::copy(move_vector.cbegin(), move_vector.cend(), std::back_inserter(copy.move_vector));
				std::copy(another.move_vector.cbegin(), another.move_vector.cend(), std::back_inserter(copy.move_vector));
				return copy;
			}

			inline bool operator==(const move_path& another) const {
				return move_vector == another.move_vector;
			}

			inline bool operator<(const move_path& another) const {
				return move_vector < another.move_vector;
			}

			inline std::vector<move_path> syntactic_interleaving_neighbours() {
				if (move_vector.size() < 2) {
					return std::vector<move_path>();
				}

				auto result = std::vector<move_path>(move_vector.size() - 1, *this);
				auto iter = result.begin();
				for (std::size_t i{ 0 }; i + 1 < move_vector.size(); ++i) {
					if (!(move_vector[i] == move_vector[i + 1])) {
						std::swap(iter->move_vector[i], iter->move_vector[i + 1]);
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
				for (std::size_t i = 0; i + 1 < move_vector.size(); ++i) {
					counter += !(move_vector[i].pid == move_vector[i + 1].pid);
				}
				return counter;
			}

			inline static bool antiprettiness_relation(const move_path& l, const move_path& r) {
				return l.changes() < r.changes();
			}

		};


		template<class Positions_Of_Pieces_Type>
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
			using positions_of_pieces_type = Positions_Of_Pieces_Type;

			using state_path_type = state_path<positions_of_pieces_type>;

			using state_path_vector_type = typename state_path_type::vector_type;

		private:

			template<class State_Label_Type>
			static void extract_all_state_paths_helper(
				const simple_state_bigraph<positions_of_pieces_type, State_Label_Type>& source,
				std::vector<state_path<positions_of_pieces_type>> all_state_paths,
				state_path_vector_type& depth_first_path
				//, const typename simple_state_bigraph<positions_of_pieces_type, State_Label_Type>::map_type::const_iterator& current_state_iter /* not end */
			) {
				auto iter = source.map.find(depth_first_path.back());

				if (iter == source.map.cend()) {
					return; // Never reached by logic when used correctly.
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
			*	@brief
			*	@return Number of partitions found
			*/
			static std::size_t make_state_graph_path_partitioning(simple_state_bigraph<positions_of_pieces_type, std::vector<bool>>& bigraph) {
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

			template<class State_Label_Type>
			static void extract_subgraph_by_label(
				const simple_state_bigraph<positions_of_pieces_type, std::vector<bool>>& source,
				std::size_t label_index,
				simple_state_bigraph<positions_of_pieces_type, State_Label_Type>& destination
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
							std::make_pair(pair.first, typename simple_state_bigraph<positions_of_pieces_type, State_Label_Type>::node_links())
						);
						std::copy_if(pair.second.predecessors.cbegin(), pair.second.predecessors.cend(), std::inserter(iter->second.predecessors, iter->second.predecessors.end()), has_label);
						std::copy_if(pair.second.successors.cbegin(), pair.second.successors.cend(), std::inserter(iter->second.successors, iter->second.successors.end()), has_label);
					}
				}
			}



			template<class State_Label_Type>
			static std::vector<state_path<positions_of_pieces_type>> extract_all_state_paths(const simple_state_bigraph<positions_of_pieces_type, State_Label_Type>& source) {
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

	}

}


