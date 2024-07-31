#pragma once

#include "../models/direction.h"

#include <vector>

namespace tobor {
	namespace v1_1 {

		/**
		*	@brief Calculates successor states by calcualting successor cell of single pieces for moving in a given direction.
		*/
		template<class Cell_Id_T, class Quick_Move_Cache_T, class Piece_Move_T>
		class move_engine {
		public:

			using quick_move_cache_type = Quick_Move_Cache_T;

			using world_type = typename quick_move_cache_type::world_type;

			using cell_id_type = Cell_Id_T;

			using cell_id_int_type = typename cell_id_type::int_cell_id_type;

			using piece_move_type = Piece_Move_T;

			using piece_id_type = typename piece_move_type::piece_id_type;

			struct arithmetic_error {

				struct no_move {
					std::vector<piece_move_type> zero_moves;
				};

				struct multi_move {
					std::vector<piece_move_type> moves;
				};
			};

		private:

			quick_move_cache_type _cache;

		public:

			/**
			* @brief Constructs a move_engine.
			*
			* @details \p my_word must not be changed externally. This is constructing a quick_move_cache inside which would be invalidated.
			*/
			move_engine(const world_type& my_world) : _cache(my_world) {
			}

			inline const world_type& board() const noexcept { return _cache.board(); }

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving in given direction \p d until any obstacle (wall or piece).
			*/
			template<class Position_Of_Pieces_T>
			inline cell_id_int_type next_cell_max_move_raw(
				const cell_id_int_type& raw_start_cell_id,
				const Position_Of_Pieces_T& state,
				const direction& d
			) const {
				cell_id_int_type raw_next_cell_id{ _cache.get(d, raw_start_cell_id) };

				for (std::size_t i = 0; i < state.COUNT_ALL_PIECES; ++i) { // iterate over all pieces
					const cell_id_int_type current_raw_id{ state.piece_positions()[i].get_raw_id(d, board()) };

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
			template<class Position_Of_Pieces_T>
			inline cell_id_type next_cell_max_move(
				const cell_id_type& start_cell,
				const Position_Of_Pieces_T& state,
				const direction& d
			) const {
				const cell_id_int_type raw_start_cell_id{ start_cell.get_raw_id(d,board()) };

				const cell_id_int_type raw_next_cell_id{ next_cell_max_move_raw(raw_start_cell_id, state, d) };

				return cell_id_type::create_by_raw_id(d, raw_next_cell_id, board());
			}

			/**
			*	@brief Determines all possible predecessor states of \p state when the piece \p _piece_id was moved from \p _direction_from (i.e. into opposite direction compared to \p _direction_from).
			*
			*	@details Note, if given piece may have come from a specified direction, but it is not able to stop at it's position according to \p state, then there is no predecessor.
			*/
			template<class Position_Of_Pieces_T>
			inline std::vector<Position_Of_Pieces_T> predecessor_states(
				const Position_Of_Pieces_T& state,
				const typename piece_move_type::piece_id_type& _piece_id,
				const direction& _direction_from
			) const {
				const cell_id_type start_cell{ state.piece_positions()[_piece_id.value] };

				const cell_id_int_type raw_start_cell_id{ start_cell.get_raw_id(_direction_from, board()) };

				// need to check if we can stop here coming from _direction_from.
				// return if there is no obstacle in opposite direction
				if (next_cell_max_move_raw(raw_start_cell_id, state, !_direction_from) != raw_start_cell_id)
					return std::vector<Position_Of_Pieces_T>();

				auto raw_far_id = next_cell_max_move_raw(
					raw_start_cell_id,
					state,
					_direction_from
				);

				auto result = std::vector<Position_Of_Pieces_T>(
					raw_start_cell_id > raw_far_id ?
					raw_start_cell_id - raw_far_id :
					raw_far_id - raw_start_cell_id,
					state
				);

				const int8_t increment{ static_cast<int8_t>((raw_start_cell_id > raw_far_id) - (raw_start_cell_id < raw_far_id)) };

				auto iter = result.begin();
				for (cell_id_int_type raw_id = raw_far_id; raw_id != raw_start_cell_id; raw_id += increment, ++iter) {
					iter->piece_positions()[_piece_id.value] = cell_id_type::create_by_raw_id(_direction_from, raw_id, board());
					iter->sort_pieces();
				}
				return result;
			}

			/**
			*	@brief Determines all possible predecessor states of \p state when the piece \p _piece_id was moved in any direction.
			*/
			template<class Position_Of_Pieces_T>
			inline std::vector<Position_Of_Pieces_T> predecessor_states(
				const Position_Of_Pieces_T& state,
				const typename piece_move_type::piece_id_type& _piece_id,
				const bool SHRINK = true
			) const {
				auto result = std::vector<Position_Of_Pieces_T>();
				result.reserve(static_cast<std::size_t>(board().get_horizontal_size() + board().get_vertical_size()));
				for (direction d = direction::begin(); d != direction::end(); ++d) {
					auto part = predecessor_states(state, _piece_id, d);
					std::copy(std::begin(part), std::end(part), std::back_inserter(result));
				}
				if (SHRINK) result.shrink_to_fit();
				return result;
			}

			/**
			*	@brief Determines all possible predecessor states of \p state when any piece was moved in any direction.
			*/
			template<class Position_Of_Pieces_T>
			inline std::vector<Position_Of_Pieces_T> predecessor_states(const Position_Of_Pieces_T& state, const bool SHRINK = true) const {
				auto result = std::vector<Position_Of_Pieces_T>();
				result.reserve(
					static_cast<std::size_t>(board().get_horizontal_size() + board().get_vertical_size()) * piece_id_type::pieces_quantity_type::COUNT_ALL_PIECES
				);
				for (piece_id_type piece = piece_id_type::begin(); piece != piece_id_type::end(); ++piece) {
					auto part = predecessor_states(state, piece, false);
					std::copy(std::begin(part), std::end(part), std::back_inserter(result));
				}
				if (SHRINK) result.shrink_to_fit();
				return result;
			}
			// ### should add a function returning void but taking an insert iterator

			/**
			*	@brief Calculates the successor state arising when moving \p _piece_id into direction \p _direction.
			*/
			template<class Position_Of_Pieces_T>
			inline Position_Of_Pieces_T successor_state(const Position_Of_Pieces_T& state, const piece_id_type& _piece_id, const direction& _direction) const {
				Position_Of_Pieces_T result(state);

				result.piece_positions()[_piece_id.value] = next_cell_max_move(state.piece_positions()[_piece_id.value], state, _direction);
				result.sort_pieces();

				return result;
			}

			/**
			*	@brief Calculates the successor state arising when moving \p _piece_id into direction \p _direction.
			*	@details Updates _piece_id to the new id the piece takes after is was moved.
			*/
			template<class Position_Of_Pieces_T>
			inline Position_Of_Pieces_T successor_state_feedback(const Position_Of_Pieces_T& state, piece_id_type& _piece_id, const direction& _direction) const {
				Position_Of_Pieces_T result(state);

				result.piece_positions()[_piece_id.value] = next_cell_max_move(state.piece_positions()[_piece_id.value], state, _direction);
				result.sort_pieces(_piece_id.value);

				return result;
			}

			/**
			*	@brief Calculates the successor state arising when \p move is applied.
			*/
			template<class Position_Of_Pieces_T>
			inline Position_Of_Pieces_T successor_state(const Position_Of_Pieces_T& state, const piece_move_type& move) const { return successor_state(state, move.pid, move.dir); }

			/**
			*	@brief Calculated the piece_move which has to be applied in order to move from \p from_state to \p to_state
			*
			*	@details If (\p from_state == \p to_state) arithmetic_error::no_move is thrown.
			*	If there are multiple moves in sequence needed in order to move from \p from_state to \p to_state, an arithmetic_error::multi_move is thrown.
			*	Also, if there are different single moves one can use from \p from_state to \p to_state, an arithmetic_error::multi_move is thrown.
			*	Note, by theory this last case should be impossible, but it might occur if you pass ill state(s) as parameters.
			*/
			template<class Position_Of_Pieces_T>
			inline piece_move_type state_minus_state(const Position_Of_Pieces_T& to_state, const Position_Of_Pieces_T& from_state) const {
				if (from_state == to_state) { // no move error

					typename arithmetic_error::no_move no_move_exception;

					for (auto pid = piece_id_type::begin(); pid < piece_id_type::end(); ++pid) {
						for (auto dir = direction::begin(); dir < direction::end(); ++dir) {
							if (successor_state(from_state, pid, dir) == to_state) {
								no_move_exception.zero_moves.emplace_back(pid, dir);
							}
						}
					}

					throw no_move_exception;
				}

				typename arithmetic_error::multi_move multi_move_exception; // collect all valid moves

				for (auto pid = piece_id_type::begin(); pid < piece_id_type::end(); ++pid) {
					for (auto dir = direction::begin(); dir < direction::end(); ++dir) {
						if (successor_state(from_state, pid, dir) == to_state) {
							multi_move_exception.moves.emplace_back(pid, dir);
						}
					}
				}

				if (multi_move_exception.moves.size() != 1) {
					throw multi_move_exception;
				}

				return multi_move_exception.moves[0];

			}

		};


	}
}
