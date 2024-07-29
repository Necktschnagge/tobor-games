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
		tobor::v1_0::default_legacy_world world;
	);
}

TEST(tobor__v1_0__universal_cell_id, create) {
	EXPECT_NO_THROW(
		auto cell_id = tobor::v1_0::default_cell_id();
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
		auto w = tobor::v1_0::default_legacy_world(10, 10);

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

	auto x = tobor::v1_0::default_piece_id(7);

	EXPECT_EQ(x.value, 7);
}

TEST(tobor__v1_0__piece_move, create_and_compare) {

	auto x = tobor::v1_0::piece_move(tobor::v1_0::default_piece_id(7), tobor::v1_0::direction::SOUTH());
	auto y = tobor::v1_0::piece_move(tobor::v1_0::default_piece_id(8), tobor::v1_0::direction::NORTH());

	EXPECT_EQ(x.pid, tobor::v1_0::default_piece_id::int_type(7));
	EXPECT_EQ(y.dir, tobor::v1_0::direction::NORTH());

	EXPECT_TRUE(x < y);
}

TEST(tobor__v1_0__move_path, create) {

	EXPECT_NO_THROW(
		auto mp = tobor::v1_0::default_move_path();
	(void)mp;
	);

}
