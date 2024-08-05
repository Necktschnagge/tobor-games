#pragma once

#include "../src/models/legacy_world.h"
#include "../src/models/redundant_cell_id.h"
#include "../src/models/pieces_quantity.h"
#include "../src/models/positions_of_pieces.h"
#include "../src/models/piece_id.h"
#include "../src/models/piece_move.h"
#include "../src/models/legacy_move_path.h"


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

		using default_move_path = legacy_move_path<default_piece_move>;

	}
}
