#pragma once

#include "direction.h"

namespace tobor {
	namespace v1_0 {

		/*
		*	@brief Equivalent to a pair of a piece_id and a direction where to move it.
		*
		*	@details Does not define how piece_id is interpreted.
		*/
		template<class Piece_Id_Type>
		struct piece_move {
		public:

			using piece_id_type = Piece_Id_Type;

			using pieces_quantity_type = typename piece_id_type::pieces_quantity_type;

			piece_id_type pid;
			direction dir;

			piece_move(const piece_id_type& pid, const direction& dir) : pid(pid), dir(dir) {}

			piece_move() : pid(0), dir(direction::NORTH()) {}

			piece_move(const piece_move&) = default;

			piece_move& operator=(const piece_move&) = default;

			piece_move(piece_move&&) = default;

			piece_move& operator=(piece_move&&) = default;

			inline bool operator<(const piece_move& another) const {
				return pid == another.pid ?
																												dir < another.dir :
					pid < another.pid;
			}

			inline bool operator==(const piece_move& another) const {
				return pid == another.pid && dir == another.dir;
			}
		};

	}

	namespace v1_1 {
		/*
		*	@brief Equivalent to a pair of a piece_id and a direction where to move it.
		*
		*	@details Does not define how piece_id is interpreted.
		*/
		template<class Piece_Id_Type>
		using piece_move = tobor::v1_0::piece_move<Piece_Id_Type>;
	}
}
