#pragma once

#include "../models/direction.h"


namespace tobor {
	namespace v1_0 {

		/**
		*	@brief Calculates successor states by calcualting successor cell of single pieces for moving in a given direction.
		*/
		template<class Position_Of_Pieces_T, class Quick_Move_Cache_T, class Piece_Move_Type >
		class legacy_move_engine { // logic engine
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
			* @brief Constructs a legacy_move_engine.
			*
			* @details \p my_word must not be changed externally. This is constructing a legacy_quick_move_cache inside which would be invalidated.
			*/
			legacy_move_engine(const world_type& my_world) : my_world(my_world), cache(my_world) {
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
				//decltype(legacy_quick_move_cache::get_west) legacy_quick_move_cache::* next_cache_direction = &legacy_quick_move_cache::get_east;

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

	}
}
