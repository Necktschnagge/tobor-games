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
		template<class Position_Of_Pieces_T, class Quick_Move_Cache_T, class Piece_Move_Type>
		class move_one_piece_calculator { // logic engine
		public:

			using positions_of_pieces_type = Position_Of_Pieces_T;
			using cell_id_type = typename positions_of_pieces_type::cell_id_type;
			using world_type = typename positions_of_pieces_type::world_type;

			using cell_id_int_type = typename cell_id_type::int_cell_id_type;


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

			using id_getter_type = cell_id_int_type(cell_id_type::*)(const world_type&);
			using cell_id_creator = cell_id_type(*)(cell_id_int_type, const world_type&);
			using cache_direction_getter = cell_id_int_type(quick_move_cache_type::*)(cell_id_int_type);

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving in given direction until any obstacle (wall or piece).
			*/
			inline cell_id_int_type next_cell_max_move_raw(
				const cell_id_int_type& raw_start_cell_id,
				const positions_of_pieces_type& state,
				const id_getter_type& get_raw_id,
				const cache_direction_getter& get_cache_direction
			) {
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
			inline cell_id_int_type next_cell_max_move_raw(
				const cell_id_int_type& raw_start_cell_id,
				const positions_of_pieces_type& state,
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
			inline cell_id_type next_cell_max_move(
				const cell_id_type& start_cell,
				const positions_of_pieces_type& state,
				const id_getter_type& get_raw_id,
				const cell_id_creator& create_cell_id_by,
				const cache_direction_getter& get_cache_direction
			) {
				const cell_id_int_type raw_start_cell_id{ (start_cell.*get_raw_id)(my_world) };

				const cell_id_int_type raw_next_cell_id{ next_cell_max_move_raw(raw_start_cell_id, state, get_raw_id,get_cache_direction) };

				return create_cell_id_by(raw_next_cell_id, my_world);
			}

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving in given direction until any obstacle (wall or piece).
			*/
			inline cell_id_type next_cell_max_move(
				const cell_id_type& start_cell,
				const positions_of_pieces_type& state,
				const direction& d
			) const {
				const cell_id_int_type raw_start_cell_id{ start_cell.get_raw_id(d,my_world) };

				const cell_id_int_type raw_next_cell_id{ next_cell_max_move_raw(raw_start_cell_id, state, d) };

				return cell_id_type::create_by_raw_id(d, raw_next_cell_id, my_world);
			}

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
			) {
				const cell_id_int_type raw_start_cell_id{ (start_cell.*get_raw_id)(my_world) };

				const cell_id_int_type raw_next_cell_id{ next_cell_max_move_raw(raw_start_cell_id, state, get_raw_id,get_cache_direction) };

				return create_cell_id_by(raw_next_cell_id, my_world);
			}


			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving west until obstacle.
			*/
			[[deprecated]] inline std::pair<cell_id_type, bool> get_next_cell_on_west_move(const cell_id_type& start_cell, const positions_of_pieces_type& state) {
				cell_id_int_type next_cell{ cache.get_west(start_cell.get_id()) };

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
			[[deprecated]] inline std::pair<cell_id_type, bool> get_next_cell_on_east_move(const cell_id_type& start_cell, const positions_of_pieces_type& state) {

				//decltype(cell_id_type::get_id) cell_id_type::* get_id_type = &cell_id_type::get_id;
				//decltype(quick_move_cache::get_west) quick_move_cache::* next_cache_direction = &quick_move_cache::get_east;

				cell_id_int_type next_cell{ cache.get_east(start_cell.get_id()) };

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
			[[deprecated]] inline std::pair<cell_id_type, bool> get_next_cell_on_south_move(const cell_id_type& start_cell, const positions_of_pieces_type& state) {
				cell_id_int_type next_cell{ cache.get_south(start_cell.get_transposed_id(my_world)) };

				//if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
				//	return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), false);
				//}

				// iterate over all pieces since they may appear as obstacle

				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					if (next_cell <= state.piece_positions[i].get_transposed_id(my_world) && state.piece_positions[i].get_transposed_id(my_world) < start_cell.get_transposed_id(my_world)) { // target piece
						next_cell = state.piece_positions[i].get_transposed_id(my_world) + 1;
					}
				}

				if (start_cell.get_transposed_id(my_world) == next_cell) { // no move possible by walls
					return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), false);
				}

				return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), true);
			}

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving north until obstacle.
			*/
			[[deprecated]] inline std::pair<cell_id_type, bool> get_next_cell_on_north_move(const cell_id_type& start_cell, const positions_of_pieces_type& state) {
				cell_id_int_type next_cell{ cache.get_north(start_cell.get_transposed_id(my_world)) };

				//if (start_cell.get_transposed_id() == next_cell) { // no move possible by walls
				//	return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), false);
				//}

				// iterate over all pieces since they may appear as obstacle

				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					if (start_cell.get_transposed_id(my_world) < state.piece_positions[i].get_transposed_id(my_world) && state.piece_positions[i].get_transposed_id(my_world) <= next_cell) { // target piece
						next_cell = state.piece_positions[i].get_transposed_id(my_world) - 1;
					}
				}

				if (start_cell.get_transposed_id(my_world) == next_cell) { // no move possible by walls
					return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), false);
				}

				return std::make_pair(cell_id_type::create_by_transposed_id(next_cell, my_world), true);
			}

			inline std::vector<positions_of_pieces_type> predecessor_states(
				const positions_of_pieces_type& state,
				const typename piece_move_type::piece_id_type& _piece_id,
				const direction& _direction_from)
			{
				const cell_id_type start_cell{ state.piece_positions[_piece_id.value] };

				const cell_id_int_type raw_start_cell_id{ start_cell.get_raw_id(_direction_from, my_world) };

				auto raw_far_id = next_cell_max_move_raw(
					raw_start_cell_id,
					state,
					!_direction_from
				);

				auto result = std::vector<positions_of_pieces_type>(
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

			inline std::vector<positions_of_pieces_type> predecessor_states(
				const positions_of_pieces_type& state,
				const typename piece_move_type::piece_id_type& _piece_id)
			{
				auto result = std::vector<positions_of_pieces_type>();
				result.reserve(static_cast<std::size_t>(my_world.get_horizontal_size() + my_world.get_vertical_size()) * 2);
				for (direction d = direction::begin(); d != direction::end(); ++d) {
					auto part = predecessor_states(state, _piece_id, d);
					std::copy(std::begin(part), std::end(part), std::back_inserter(result));
				}
				result.shrink_to_fit();
				return result;
			}

			inline std::vector<positions_of_pieces_type> predecessor_states(const positions_of_pieces_type& state) {
				auto result = std::vector<positions_of_pieces_type>();
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


			[[deprecated]] inline std::pair<positions_of_pieces_type, bool> successor_state_deprecated(
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

			inline positions_of_pieces_type successor_state(
				const positions_of_pieces_type& state,
				const typename piece_move_type::piece_id_type& _piece_id,
				const direction& _direction
			) const {
				positions_of_pieces_type result(state);

				result.piece_positions[_piece_id.value] = next_cell_max_move(
					state.piece_positions[_piece_id.value],
					state,
					_direction
				);
				result.sort_pieces();

				return result;
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

			inline bool is_target_state(const positions_of_pieces_type& state, const cell_id_type& target_cell) const {
				return state.is_final(target_cell); // ### wrapper to be deleted
			}

		};

		using default_move_one_piece_calculator = move_one_piece_calculator<default_positions_of_pieces, default_quick_move_cache, default_piece_move>;

		/*
				template<class Positions_Of_Pieces_Type>
				class backward_graph {
				public:
					using positions_of_pieces_type = Positions_Of_Pieces_Type;



					struct graph_node {
						positions_of_pieces_type state;
						//std::size_t;
					};

				};
		*/

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


		template <class Move_One_Piece_Calculator>
		class distance_exploration {

		public:
			using move_one_piece_calculator_type = Move_One_Piece_Calculator;

			using positions_of_pieces_type = typename Move_One_Piece_Calculator::positions_of_pieces_type;

			using piece_move_type = typename move_one_piece_calculator_type::piece_move_type;

			using cell_id_type = typename positions_of_pieces_type::cell_id_type;

			using size_type = std::size_t;

			static constexpr size_type SIZE_TYPE_MAX{ std::numeric_limits<size_type>::max() };

			using move_candidate = typename tobor::v1_0::state_graph_node<positions_of_pieces_type, piece_move_type>::move_candidate;

		private:
			std::vector<std::vector<positions_of_pieces_type>> reachable_states_by_distance;

			// number of steps needed by any optimal solution
			size_type optimal_path_length;

		public:
			distance_exploration(const positions_of_pieces_type& initial_state) :
				optimal_path_length(SIZE_TYPE_MAX)
				//initial_state(initial_state)
			{
				reachable_states_by_distance.emplace_back(std::vector<positions_of_pieces_type>{ initial_state });

			}

			inline size_type get_optimal_path_length() { return optimal_path_length; };

			inline std::vector<positions_of_pieces_type> optimal_final_states(const cell_id_type& target_cell) const {
				std::vector<positions_of_pieces_type> result;
				for (auto dist_iter = reachable_states_by_distance.cbegin(); dist_iter != reachable_states_by_distance.cend(); ++dist_iter) {
					for (auto state_iter = dist_iter->cbegin(); state_iter != dist_iter->cend(); ++state_iter) {
						if (*state_iter.is_final(target_cell)) {
							result.push_back(*state_iter);
						}
					}
					if (!result.empty()) {
						return result;
					}
				}
				return result;
			}

			template<class State_Label_Type>
			void get_simple_bigraph(
				move_one_piece_calculator_type& engine,
				const cell_id_type& target_cell,
				simple_state_bigraph<positions_of_pieces_type, State_Label_Type>& destination
			) {
				if (optimal_path_length == SIZE_TYPE_MAX) {
					throw 0;
				}
				using bigraph = simple_state_bigraph<positions_of_pieces_type>;

				//std::vector<bool> flagged_states(reachable_states_by_distance[optimal_path_length].size(), false);
				//std::vector<bool> next_flagged_states;
				std::vector<positions_of_pieces_type> states;

				for (std::size_t i{ 0 }; i < reachable_states_by_distance[optimal_path_length].size(); ++i) {
					if (reachable_states_by_distance[optimal_path_length][i].is_final(target_cell)) {
						//flagged_states[i] = true;
						destination.map.insert(
							destination.map.end(),
							std::make_pair(
								reachable_states_by_distance[optimal_path_length][i],
								bigraph::node_links()
							)
						);
						states.push_back(reachable_states_by_distance[optimal_path_length][i]);
					}
				}
				std::size_t backward_explore_distance = optimal_path_length;

				while (backward_explore_distance > 0) {
					--backward_explore_distance;

					std::vector<std::pair<positions_of_pieces_type, positions_of_pieces_type>> possible_edges;

					// all maybe-edges
					for (const auto& state : states) {
						auto vec = engine.predecessor_states(state);
						possible_edges.reserve(possible_edges.size() + vec.size());
						std::transform(std::execution::par, vec.cbegin(), vec.cend(), [&](const positions_of_pieces_type& pred_state) {
							possible_edges.emplace_back(pred_state, state);
							});
					}
					// sort by from-state
					std::sort(possible_edges.begin(), possible_edges.end());

					//remove if from state not in distance state vector
					std::size_t compare_index{ 0 };

					possible_edges.erase(
						std::remove_if(possible_edges.begin(), possible_edges.end(), [&](const std::pair<positions_of_pieces_type, positions_of_pieces_type>& edge) {
							while (compare_index < reachable_states_by_distance[backward_explore_distance].size() && reachable_states_by_distance[backward_explore_distance][compare_index] < edge.first) {
								++compare_index;
							}
							if (compare_index < reachable_states_by_distance[backward_explore_distance].size() && reachable_states_by_distance[backward_explore_distance][compare_index] == edge.first) {
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

			inline std::vector<positions_of_pieces_type> optimal_final_states(const cell_id_type& target_cell, size_type min_distance_level) const {
				std::vector<positions_of_pieces_type> result;
				if (!(min_distance_level < reachable_states_by_distance.size())) {
					return result;
				}
				auto dist_iter = reachable_states_by_distance.cbegin() + min_distance_level;
				for (; dist_iter != reachable_states_by_distance.cend(); ++dist_iter) {
					for (auto state_iter = dist_iter->cbegin(); state_iter != dist_iter->cend(); ++state_iter) {
						if (*state_iter.is_final(target_cell)) {
							result.push_back(*state_iter);
						}
					}
					if (!result.empty()) {
						return result;
					}
				}
				return result;
			}

			/*
			inline std::map<positions_of_pieces_type, std::vector<move_path_type>> optimal_move_paths(const cell_id_type& target_cell) {
				throw "not implemented";
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
				throw "behavior not defined yet";
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
				throw "not yet defined"
					return remove_dead_states(optimal_final_state_iterators(target_cell_defining_live_states));
			}
			*/

			// ### offer step-wise exploration instead of exploration until optimal.
			inline void explore_until_optimal_solution_distance(
				move_one_piece_calculator_type& engine,
				const cell_id_type& target_cell
			) {
				const size_type INDEX_LAST_EXPLORATION{ reachable_states_by_distance.size() - 1 };

				if (!(INDEX_LAST_EXPLORATION < optimal_path_length)) {
					//Already executed exploration before. Any state to be explored would add states into map beyond the optimal path length
					return;
				}

				if (reachable_states_by_distance[0][0].is_final(target_cell)) {
					optimal_path_length = 0;
					return;
				}

				for (size_type expand_level_index{ INDEX_LAST_EXPLORATION }; expand_level_index < optimal_path_length; ++expand_level_index) {

					reachable_states_by_distance[expand_level_index].shrink_to_fit();

					if (reachable_states_by_distance[expand_level_index].size() == 0) {
						return; // no more states to find
					}

					reachable_states_by_distance.emplace_back();
					reachable_states_by_distance[expand_level_index + 1].reserve(reachable_states_by_distance[expand_level_index].size() * 3 + 10);


					for (std::size_t expand_index_inside_level = 0; expand_index_inside_level < reachable_states_by_distance[expand_level_index].size(); ++expand_index_inside_level) {

						std::vector<move_candidate> candidates_for_successor_states; // can be array with fixed size(?)

						// compute all successor state candidates:
						for (typename piece_move_type::piece_id_type::int_type pid = 0; pid < positions_of_pieces_type::COUNT_ALL_PIECES; ++pid) {
							for (direction direction_iter = direction::begin(); direction_iter < direction::end(); ++direction_iter) {
								candidates_for_successor_states.emplace_back(
									piece_move_type(pid, direction_iter),
									engine.successor_state(reachable_states_by_distance[expand_level_index][expand_index_inside_level], pid, direction_iter)
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
							// only check candidates arising from moved target pieces:
							index_candidate < static_cast<typename std::vector<move_candidate>::size_type>(4) * positions_of_pieces_type::COUNT_TARGET_PIECES;
							++index_candidate
							)
						{
							if (!candidates_for_successor_states[index_candidate].is_true_move) {
								continue;
							}

							if constexpr (positions_of_pieces_type::SORTED_TARGET_PIECES) {
								// general case:
								if (candidates_for_successor_states[index_candidate].successor_state.is_final(target_cell)) {
									optimal_path_length = expand_level_index + 1;
								}

							}
							else {
								// optimized case:
								if (candidates_for_successor_states[index_candidate].successor_state.piece_positions[index_candidate / 4] == target_cell) {
									// does not work for sorted final pieces! In that case we do not know where the moved piece is located.
									optimal_path_length = expand_level_index + 1;
								}
							}

							reachable_states_by_distance.back().emplace_back(candidates_for_successor_states[index_candidate].successor_state);

						}
						//....

					}
					// sort and unique "new" states
					std::sort(std::execution::par, reachable_states_by_distance.back().begin(), reachable_states_by_distance.back.end());
					reachable_states_by_distance.back().erase(
						unique(std::execution::par, reachable_states_by_distance.back().begin(), reachable_states_by_distance.back().end()),
						reachable_states_by_distance.back().end()
					);
					// delete states already seen before
					{
						using sub_iterator = typename std::vector<positions_of_pieces_type>::iterator;
						std::vector<sub_iterator> check_iterators;
						sub_iterator check_next = reachable_states_by_distance.back().begin();
						sub_iterator free_next = reachable_states_by_distance.back().begin();

						for (size_type i = 0; i < expand_level_index + 1; ++i) {
							check_iterators.emplace_back(reachable_states_by_distance[i].begin());
						}
					continue_outer_loop:
						while (check_next != reachable_states_by_distance.back().end()) {
							for (size_type i_level = 0; i_level < check_iterators.size(); ++i_level) {
								while (
									(check_iterators[i_level] != reachable_states_by_distance[i_level].end())
									&&
									(*(check_iterators[i_level]) < *check_next)
									)
								{
									++check_iterators[i_level];
								}
								if (
									(check_iterators[i_level] != reachable_states_by_distance[i_level].end())
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
						reachable_states_by_distance.back().erase(free_next, reachable_states_by_distance.back().end());
					}

					reachable_states_by_distance[expand_level_index + 1].shrink_to_fit();

				}
			}

		};

	}

}


