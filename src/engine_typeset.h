#pragma once

#include "models/dynamic_rectangle_world.h"
#include "models/min_size_cell_id.h"
#include "models/piece_id.h"
#include "models/augmented_positions_of_pieces.h"
#include "models/piece_move.h"
#include "engine/quick_move_cache.h"
#include "engine/move_engine.h"
#include "models/move_path.h"
#include "models/state_path.h"


template<class Pieces_Quantity_T>
struct ClassicEngineTypeSet {

	using world_type = tobor::v1_1::dynamic_rectangle_world<uint16_t, uint8_t>;

	using cell_id_type = tobor::v1_1::min_size_cell_id<world_type>;

	using pieces_quantity_type = Pieces_Quantity_T;

	using piece_id_type = tobor::v1_1::piece_id<pieces_quantity_type>;

	using positions_of_pieces_type_solver = tobor::v1_1::positions_of_pieces<pieces_quantity_type, cell_id_type, true, true>;

	using positions_of_pieces_type_interactive = tobor::v1_1::augmented_positions_of_pieces<pieces_quantity_type, cell_id_type, true, true>;

	using piece_move_type = tobor::v1_1::piece_move<piece_id_type>;

	using quick_move_cache_type = tobor::v1_1::quick_move_cache<world_type>;

	using move_engine_type = tobor::v1_1::move_engine<cell_id_type, quick_move_cache_type, piece_move_type>;

	using move_path_type = tobor::v1_1::move_path<piece_move_type>;

	using state_path_type_interactive = tobor::v1_1::state_path<positions_of_pieces_type_interactive>;

	using state_path_type_solver = tobor::v1_1::state_path<positions_of_pieces_type_solver>;

};
