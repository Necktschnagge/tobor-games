#pragma once

#include "direction.h"

namespace tobor {
	namespace v1_0 {

		/*
		 *	@brief Equivalent to a pair of a piece_id and a direction where to move it.
		 *
		 *	@details Does not define how piece_id is interpreted.
		 */
		template <class Piece_Id_Type, class Direction_Type = ::tobor::v1_1::direction>
		struct piece_move {
		public:
			using piece_id_type  = Piece_Id_Type;
			using direction_type = Direction_Type;

			using pieces_quantity_type = typename piece_id_type::pieces_quantity_type;

			piece_id_type  piece_id;
			direction_type direction;

			piece_move(const piece_id_type& p, const direction_type& d) : piece_id(p), direction(d) {}

			piece_move() : piece_id(0), direction(direction_type::begin()) {}

			piece_move(const piece_move&) = default;

			piece_move& operator=(const piece_move&) = default;

			piece_move(piece_move&&) = default;

			piece_move& operator=(piece_move&&) = default;

			/** Less Than Comparison Operator. Two instances are sorted by piece_id, then by direction. */
			inline bool operator<(const piece_move& another) const { return piece_id == another.piece_id ? direction < another.direction : piece_id < another.piece_id; }

			/** Equal Comparison Operator. Two instances are equal if both, piece_id and direction are the same. */
			inline bool operator==(const piece_move& another) const { return piece_id == another.piece_id && direction == another.direction; }
		};

	} // namespace v1_0

	namespace v1_1 {
		/*
		 *	@brief Equivalent to a pair of a piece_id and a direction where to move it.
		 *
		 *	@details Does not define how piece_id is interpreted.
		 */
		template <class Piece_Id_Type, class Direction_Type = ::tobor::v1_1::direction>
		using piece_move = tobor::v1_0::piece_move<Piece_Id_Type, Direction_Type>;
	} // namespace v1_1
} // namespace tobor
