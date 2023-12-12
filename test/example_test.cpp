#include "gtest/gtest.h"
#include "../src/solver.h"

TEST(example_test, dummy_test_assert_true){

	tobor::v1_0::default_world w(16,16);

	tobor::v1_0::default_move_one_piece_calculator c(w);

	auto state = tobor::v1_0::default_positions_of_pieces(
		{
			tobor::v1_0::default_cell_id::create_by_id(5,w)
		},
		{
			tobor::v1_0::default_cell_id::create_by_id(20,w),
			tobor::v1_0::default_cell_id::create_by_id(69,w),
			tobor::v1_0::default_cell_id::create_by_id(3,w)
		}
		);

	const bool is_target_cell = c.is_target_state(state, tobor::v1_0::default_cell_id::create_by_id(5, w));

	ASSERT_TRUE(is_target_cell);
}
