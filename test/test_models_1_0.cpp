#include "gtest/gtest.h"

#include "../src/models_1_0.h"

#include <type_traits>
#include <array>

TEST(tobor__v1_0__division_by_2_error, create_what) {
	EXPECT_NO_THROW(
		tobor::v1_0::division_by_2_error error;
	const char* m = error.what();
	(void)m;

	);
}

TEST(tobor__v1_0__blocked_center_error, create_what) {
	EXPECT_NO_THROW(
		tobor::v1_0::blocked_center_error error;
	const char* m = error.what();
	(void)m;
	);
}

TEST(tobor__v1_0__direction, default_ctor) {
	static_assert(std::is_default_constructible<tobor::v1_0::direction>::value == false, "direction default constructible");

	ASSERT_TRUE(true);
}
TEST(tobor__v1_0__direction, invert) {

	EXPECT_EQ((!tobor::v1_0::direction::NORTH()), tobor::v1_0::direction::SOUTH());
	EXPECT_EQ((!tobor::v1_0::direction::EAST()), tobor::v1_0::direction::WEST());
	EXPECT_EQ((!tobor::v1_0::direction::SOUTH()), tobor::v1_0::direction::NORTH());
	EXPECT_EQ((!tobor::v1_0::direction::WEST()), tobor::v1_0::direction::EAST());

}

TEST(tobor__v1_0__direction, iterator_and_order) {

	std::vector<tobor::v1_0::direction> all;

	EXPECT_NO_THROW(
		for (auto siter = tobor::v1_0::direction::begin(); siter != tobor::v1_0::direction::end(); ++siter) {
			auto x = siter.get();
			(void)x;
			all.push_back(siter);
		}
	);

	EXPECT_TRUE(
		std::is_sorted(all.cbegin(), all.cend())
	);
}

TEST(tobor__v1_0__wall, create_wall_type_std_constructor) {
	static_assert(std::is_default_constructible<tobor::v1_0::wall>::value == false, "wall type default constructible");

	ASSERT_TRUE(true);
}


TEST(tobor__v1_0__wall, create_wall_type_bool_conversion) {
	auto t_wall = tobor::v1_0::wall(true);
	EXPECT_TRUE(static_cast<bool>(t_wall));
	auto f_wall = tobor::v1_0::wall(false);
	EXPECT_FALSE(static_cast<bool>(f_wall));
}

TEST(tobor__v1_0__tobor_world, create_world) {
	EXPECT_NO_THROW(
		tobor::v1_0::tobor_world world;
	);
}

TEST(tobor__v1_0__universal_cell_id, create) {
	EXPECT_NO_THROW(
		auto cell_id = tobor::v1_0::universal_cell_id();
	(void)cell_id;
	);
}

TEST(tobor__v1_0__universal_cell_id, consistency) {
	// TODO check that id conversion has no inconsistencies...
	ASSERT_TRUE(true);
}

TEST(tobor__v1_0__pieces_quantity, create_wall_type_bool_conversion) {
	using pq_type = tobor::v1_0::pieces_quantity<uint16_t, 1, 5>;
	EXPECT_EQ(pq_type::COUNT_ALL_PIECES, 6);
	EXPECT_EQ(pq_type::COUNT_TARGET_PIECES, 1);
	EXPECT_EQ(pq_type::COUNT_NON_TARGET_PIECES, 5);
}

TEST(tobor__v1_0__positions_of_pieces, create) {

	using pop_type = tobor::v1_0::positions_of_pieces<tobor::v1_0::default_pieces_quantity, tobor::v1_0::default_cell_id, false, false>;

	EXPECT_NO_THROW(
		auto w = tobor::v1_0::default_world(10, 10);

	auto raw = std::vector<tobor::v1_0::default_cell_id>(4, tobor::v1_0::default_cell_id());

	raw[0].set_id(33, w);
	raw[1].set_id(70, w);
	raw[2].set_id(12, w);
	raw[3].set_id(87, w);

	auto pop = pop_type(raw.cbegin());

	(void)pop;

	);

	static_assert(std::is_default_constructible<pop_type>::value == false, "positions_of_pieces default constructible");
}

TEST(tobor__v1_0__piece_id, create) {

	auto x = tobor::v1_0::piece_id(7);

	EXPECT_EQ(x.value, 7);
}

TEST(tobor__v1_0__piece_move, create_and_compare) {

	auto x = tobor::v1_0::piece_move(tobor::v1_0::default_piece_id(7), tobor::v1_0::direction::SOUTH());
	auto y = tobor::v1_0::piece_move(tobor::v1_0::default_piece_id(8), tobor::v1_0::direction::NORTH());

	EXPECT_EQ(x.pid, tobor::v1_0::default_piece_id::int_type(7));
	EXPECT_EQ(y.dir, tobor::v1_0::direction::NORTH());

	EXPECT_TRUE(x < y);
}

/*
TEST(tobor__v1_0__move_path, create) {

	EXPECT_NO_THROW(
		auto mp = tobor::v1_0::move_path();
	(void)mp;
	);

}
*/





#if false
TEST(engine, example_integration) {
	// check the following scenario:
	// create world
	// create initial robots position
	// run solver
	/*
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

	// specify target field
	auto target = tobor::v1_0::default_cell_id::create_by_coordinates(9, 1, w);

	auto w_analyzer = tobor::v1_0::default_move_one_piece_calculator(w);

	auto partial_state_graph = tobor::v1_0::partial_state_graph<tobor::v1_0::default_move_one_piece_calculator>(target_robots, other_robots);

	partial_state_graph.explore_until_optimal_solution_distance(w_analyzer, target);

	ASSERT_EQ(9, partial_state_graph.get_optimal_path_length());
	*/

}
#endif
