#include "gtest/gtest.h"

#include "../src/solver_1_0.h"

#include <type_traits>
#include <array>


/*
TEST(engine, example_integration) {
	// check the following scenario:
	// create world
	// create initial robots position
	// run solver

	auto w = tobor::v1_0::default_world(16, 16);

	w.block_center_cells(2, 2);

	// add vertical walls row by row, starting left
	w.west_wall_by_id(w.coordinates_to_cell_id(6, 0)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(12, 0)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(2, 1)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(10, 1)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(14, 3)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(1, 4)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(11, 4)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(13, 5)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(4, 6)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(12, 9)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(7, 10)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(14, 10)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(3, 11)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(7, 13)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(10, 13)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(1, 14)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(13, 14)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(4, 15)) = true;
	w.west_wall_by_id(w.coordinates_to_cell_id(12, 15)) = true;

	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(0, 2)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(0, 10)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(1, 4)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(1, 14)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(2, 2)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(2, 11)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(3, 7)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(6, 3)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(6, 14)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(7, 11)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(9, 2)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(9, 13)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(10, 4)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(11, 10)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(13, 6)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(13, 15)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(14, 3)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(14, 10)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(15, 7)) = true;
	w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(15, 12)) = true;

	// specify initial state
	auto target_piece = tobor::v1_0::default_cell_id::create_by_coordinates(6, 9, w);
	auto green_robot = tobor::v1_0::default_cell_id::create_by_coordinates(12, 7, w);
	auto red_robot = tobor::v1_0::default_cell_id::create_by_coordinates(12, 12, w);
	auto yellow_robot = tobor::v1_0::default_cell_id::create_by_coordinates(6, 14, w);

	std::array<tobor::v1_0::default_cell_id, 3> other_robots{ green_robot, red_robot, yellow_robot };
	std::array<tobor::v1_0::default_cell_id, 1> target_robots{ target_piece };

	//auto initial_state = tobor::v1_0::positions_of_pieces<3>(target_piece, std::move(other_robots));

	// specify target cell
	auto target = tobor::v1_0::default_cell_id::create_by_coordinates(9, 1, w);

	auto w_analyzer = tobor::v1_0::default_move_one_piece_calculator(w);

	auto partial_state_graph = tobor::v1_0::partial_state_graph<tobor::v1_0::default_move_one_piece_calculator>(target_robots, other_robots);

	partial_state_graph.explore_until_optimal_solution_distance(w_analyzer, target);

	ASSERT_EQ(9, partial_state_graph.get_optimal_path_length());

}
*/

