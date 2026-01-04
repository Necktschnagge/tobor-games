#pragma once

#include "models/augmented_positions_of_pieces.h"
#include "engine/quick_move_cache.h"

#include "engine/move_engine.h"
#include "models/simple_state_digraph.h"

#include "engine/distance_exploration.h"


#include "default_models_1_1.h"

#include "solver_1_0.h"

//#include <map>
//#include <set>
//#include <array>
//#include <algorithm>
//#include <limits>
//#include <execution>
//#include <numeric>

namespace tobor {

	namespace v1_1 {


		using default_quick_move_cache = quick_move_cache<dynamic_rectangle_world<std::size_t, std::size_t>>;

		using default_move_engine = move_engine<default_min_size_cell_id, default_quick_move_cache, default_piece_move>;

	}

}


