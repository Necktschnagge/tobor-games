#pragma once

#include "models/min_size_cell_id.h"
#include "models/dynamic_rectangle_world.h"
#include "models/pieces_quantity.h"
#include "models/augmented_positions_of_pieces.h"
#include "models/piece_id.h"
#include "models/piece_move.h"


namespace tobor {
	namespace v1_1 {

		using default_min_size_cell_id = min_size_cell_id<default_dynamic_rectangle_world>;

		template<uint8_t COUNT_TARGET_PIECES_V, uint8_t COUNT_NON_TARGET_PIECES_V>
		using uint8_t_pieces_quantity = pieces_quantity<uint8_t, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		using default_pieces_quantity = pieces_quantity<uint8_t, 1, 3>;

		using default_positions_of_pieces = positions_of_pieces<default_pieces_quantity, default_min_size_cell_id, false, true>;

		using default_augmented_positions_of_pieces = augmented_positions_of_pieces< default_pieces_quantity, default_min_size_cell_id, false, false>;

		using default_piece_id = piece_id<default_pieces_quantity>;

		using default_piece_move = piece_move<default_piece_id>;

	}
}

