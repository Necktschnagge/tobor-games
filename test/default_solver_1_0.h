#pragma once


#include "../src/engine/legacy_quick_move_cache.h"
#include "../src/engine/legacy_move_engine.h"
#include "../src/engine/state_graph_node.h"
#include "../src/engine/partial_state_graph.h"

#include "default_models_1_0.h"

namespace tobor {
	namespace v1_0 {

		using default_quick_move_cache = legacy_quick_move_cache<default_legacy_world>;

		using default_legacy_move_engine = legacy_move_engine<default_positions_of_pieces, default_quick_move_cache, default_piece_move>;

		using default_state_graph_node = state_graph_node<default_positions_of_pieces, default_piece_move>;

	}
}


