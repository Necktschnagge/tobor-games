#pragma once

#include "models/errors.h"
#include "models/direction.h"
#include "models/wall.h"
#include "models/legacy_world.h"
#include "models/redundant_cell_id.h"
#include "models/pieces_quantity.h"
#include "models/positions_of_pieces.h"
#include "models/piece_id.h"
#include "models/piece_move.h"
#include "models/move_path.h"




#include <map>
#include <array>
#include <vector>

#include <algorithm>
#include <utility>

#include <iterator>
#include <string>
#include <execution>
#include <compare>
#include <functional>


namespace tobor {

	namespace v1_0 {


		using default_legacy_world = legacy_world<std::size_t>;

		template<uint8_t COUNT_TARGET_PIECES_V, uint8_t COUNT_NON_TARGET_PIECES_V>
		using uint8_t_pieces_quantity = pieces_quantity<uint8_t, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		using default_pieces_quantity = pieces_quantity<uint8_t, 1, 3>;

		using default_piece_id = piece_id<default_pieces_quantity>;

		using default_cell_id = redundant_cell_id<default_legacy_world>;

		using default_positions_of_pieces = positions_of_pieces<default_pieces_quantity, default_cell_id, false, true>;

		using default_piece_move = piece_move<default_piece_id>;

		using default_move_path = move_path<default_piece_move>;

	}


}

namespace std {

	template <class Pieces_Quantity_Type, class Cell_Id_Type_T, bool SORTED_TARGET_PIECES_V, bool SORTED_NON_TARGET_PIECES_V>
	inline void swap(
		tobor::v1_0::positions_of_pieces<Pieces_Quantity_Type, Cell_Id_Type_T, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>& a,
		tobor::v1_0::positions_of_pieces<Pieces_Quantity_Type, Cell_Id_Type_T, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>& b
	) {
		std::swap(a._piece_positions, b._piece_positions);
	}
}
