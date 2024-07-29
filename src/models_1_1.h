#pragma once

#include "models_1_0.h"

#include "models/id_polarisation.h"
#include "models/pieces_quantity.h"
#include "models/dynamic_rectangle_world.h"
#include "models/min_size_cell_id.h"
#include "models/augmented_positions_of_pieces.h"

#include <compare>

namespace tobor {
	namespace v1_1 {

		using default_min_size_cell_id = min_size_cell_id<default_dynamic_rectangle_world>;

		template<uint8_t COUNT_TARGET_PIECES_V, uint8_t COUNT_NON_TARGET_PIECES_V>
		using uint8_t_pieces_quantity = pieces_quantity<uint8_t, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		using default_pieces_quantity = pieces_quantity<uint8_t, 1, 3>;

		using default_positions_of_pieces = positions_of_pieces<default_pieces_quantity, default_min_size_cell_id, false, true>;

		using default_augmented_positions_of_pieces = augmented_positions_of_pieces< default_pieces_quantity, default_min_size_cell_id, false, false>;



		using default_piece_id = piece_id<default_pieces_quantity>;

		/*
		*	@brief Equivalent to a pair of a piece_id and a direction where to move it.
		*
		*	@details Does not define how piece_id is interpreted.
		*/
		template<class Piece_Id_Type>
		using piece_move = tobor::v1_0::piece_move<Piece_Id_Type>;

		using default_piece_move = piece_move<default_piece_id>;

	}
}

namespace std {
	template <class Pieces_Quantity_T, class Cell_Id_Type, bool SORTED_TARGET_PIECES_V, bool SORTED_NON_TARGET_PIECES_V>
	inline void swap(
		tobor::v1_1::augmented_positions_of_pieces<Pieces_Quantity_T, Cell_Id_Type, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>& a,
		tobor::v1_1::augmented_positions_of_pieces<Pieces_Quantity_T, Cell_Id_Type, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>& b
	) {
		std::swap(a._piece_positions, b._piece_positions);
		std::swap(a._permutation, b._permutation);
	}

}
