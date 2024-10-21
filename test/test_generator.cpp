#include "gtest/gtest.h"

#include "../src/world_generator_1_0.h"
#include "../src/world_generator_1_1.h"

#include <nlohmann/json.hpp>

#include <iostream>
#include <map>

TEST(generator, original_4_of_16_always_produces_4_blocked_cells) {

	tobor::v1_0::world_generator::original_4_of_16 g(0, 1);

	auto max_counter{ g.get_counter() };
	do {
		max_counter = g.get_counter();
		ASSERT_EQ(g.get_tobor_world().blocked_cells(), 4);
		++g;
	} while (g.get_counter() != 0);
	std::cout << "generator max counter:   " << max_counter << std::endl;
}


TEST(generator, original_4_of_16_always_produces_17_target_cells) {

	tobor::v1_0::world_generator::original_4_of_16 g(0, 1);

	std::map<uint64_t, std::vector<uint64_t>> number_of_target_cells_to_witness_counters;

	auto max_counter{ g.get_counter() };
	do {
		max_counter = g.get_counter();
		//std::cout << "current counter:   " << g.get_counter() << std::endl;

		//ASSERT_EQ(tobor::v1_0::world_generator::original_4_of_16::get_target_cell_id_vector(g.get_tobor_world()).size(), 17);

		const auto c{ tobor::v1_0::world_generator::original_4_of_16::get_target_cell_id_vector(g.get_tobor_world()).size() };
		if (c != 17) {
			number_of_target_cells_to_witness_counters[c].push_back(g.get_counter());
		}

		++g;
	} while (g.get_counter() != 0);

	nlohmann::json witnesses;
	witnesses = number_of_target_cells_to_witness_counters;

	std::cout << witnesses.dump(3) << std::endl;

	ASSERT_TRUE(number_of_target_cells_to_witness_counters.empty());


	std::cout << "generator max counter:   " << max_counter << std::endl;

}

TEST(generator, original_4_of_16_set_walls) {

	using world_type = tobor::v1_1::dynamic_rectangle_world<uint16_t, uint8_t>;

	world_type w(12, 12);

	auto cell_id = tobor::v1_1::min_size_cell_id<world_type>::create_by_coordinates(3, 4, w);

	tobor::v1_1::world_generator::original_4_of_16::set_wall_corners(w, {}, {}, { cell_id }, {}, {}, {}); // NW corner // in static lib <---------------------------------

	EXPECT_TRUE(w.west_wall_by_id(w.coordinates_to_cell_id(3,4)) == true); // check wall.





	tobor::v1_1::world_generator::original_4_of_16 world_generator;

	auto w2 = world_generator.get_tobor_world();

	auto target = world_generator.get_target_cell(); // in static lib <---------------------------------

	std::cout << int(target.get_x_coord(w2)) << "  " << int(target.get_y_coord(w2)) << std::endl;

}

