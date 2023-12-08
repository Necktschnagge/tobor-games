#pragma once

#include "models.h"

#include <vector>
#include <array>
#include <optional>

namespace tobor {

	namespace v1_0 {

		namespace world_generator {

			class original_4_of_16 {
			public:

				using world_type = tobor::v1_0::default_world;

				using cell_id_type = tobor::v1_0::default_cell_id;

				constexpr static std::size_t RED_PLANET{ 0 };
				constexpr static std::size_t GREEN_PLANET{ 1 };
				constexpr static std::size_t BLUE_PLANET{ 2 };
				constexpr static std::size_t YELLOW_PLANET{ 3 };

			private:

				static void set_wall_corners(
					world_type& world,
					const std::vector<cell_id_type>& W_wall,
					const std::vector<cell_id_type>& S_wall,
					const std::vector<cell_id_type>& NW_corners,
					const std::vector<cell_id_type>& NE_corners,
					const std::vector<cell_id_type>& SW_corners,
					const std::vector<cell_id_type>& SE_corners);

				static void set_red_planet_0(world_type& world);
				static void set_red_planet_1(world_type& world);
				static void set_red_planet_2(world_type& world);
				static void set_red_planet_3(world_type& world);

				static void set_green_planet_0(world_type& world);
				static void set_green_planet_1(world_type& world);
				static void set_green_planet_2(world_type& world);
				static void set_green_planet_3(world_type& world);

				static void set_blue_planet_0(world_type& world);
				static void set_blue_planet_1(world_type& world);
				static void set_blue_planet_2(world_type& world);
				static void set_blue_planet_3(world_type& world);

				static void set_yellow_planet_0(world_type& world);
				static void set_yellow_planet_1(world_type& world);
				static void set_yellow_planet_2(world_type& world);
				static void set_yellow_planet_3(world_type& world);

				static void create_quadrants(std::array<std::vector<world_type>, 4>& all_quadrants);

			public:

				constexpr static uint64_t COUNT_PLANET_R{ 4 };
				constexpr static uint64_t COUNT_PLANET_G{ 4 };
				constexpr static uint64_t COUNT_PLANET_B{ 4 };
				constexpr static uint64_t COUNT_PLANET_Y{ 4 };

				constexpr static uint64_t COUNT_ALIGNED_WORLDS{
					COUNT_PLANET_R * COUNT_PLANET_G * COUNT_PLANET_B * COUNT_PLANET_Y *
					3 * // choose the second quadrant's position
					2 // choose the third quadrant's position
				};

				constexpr static uint64_t COUNT_ROTATIONS{ 4 };

				constexpr static uint64_t COUNT_ALL_WORLDS{ COUNT_ALIGNED_WORLDS * COUNT_ROTATIONS };

				constexpr static uint64_t COUNT_TARGET_CELLS{ 11 };

				constexpr static uint64_t COUNT_ALL_WORLDS_WITH_SELECTED_TARGET{ COUNT_ALL_WORLDS * COUNT_TARGET_CELLS };


				static world_type get_quadrant(std::size_t planet_color, std::size_t quadrant_index);

				static void copy_walls_turned(const world_type& source, uint8_t rotation, world_type& destination);

				static world_type get_world(uint64_t select_aligned_world, uint64_t rotation);

			private:
				// standard generator:
				// 4 times: select a quadrant
				// 6 permutations
				// 4 board rotation
				// 11 select a target
				// 
				// 4   x   4   x   4   x   4   x   6   x   4   x   11
				//  1       2       3       0       5                1

				static constexpr uint64_t STANDARD_GENERATOR{
					1 +
					2 * 4 +
					3 * 4 * 4 +
					0 * 4 * 4 * 4 +
					5 * 4 * 4 * 4 * 4 +
					1 * 4 * 4 * 4 * 4 * 6 * 4
				};

				static constexpr uint64_t SECOND_GENERATOR{
					1 +
					0 * 4 +
					1 * 4 * 4 +
					2 * 4 * 4 * 4 +
					3 * 4 * 4 * 4 * 4 +
					1 * 4 * 4 * 4 * 4 * 6 +
					7 * 4 * 4 * 4 * 4 * 6 * 4
				};

				static constexpr uint64_t CYCLIC_GROUP_SIZE{
					COUNT_ALL_WORLDS_WITH_SELECTED_TARGET
				};

				uint64_t generator;
				uint64_t counter;

				static constexpr uint64_t gcd(uint64_t x, uint64_t y) {
					if (x == 0)
						return y;
					return gcd(y % x, x);
				}

				//static_assert(gcd(STANDARD_GENERATOR, CYCLIC_GROUP_SIZE) == 1, "check generator");
				//static_assert(gcd(SECOND_GENERATOR, CYCLIC_GROUP_SIZE) == 1, "check generator");

				inline uint64_t& increment_generator_until_gcd_1() {
					while (gcd(generator, CYCLIC_GROUP_SIZE) != 1) {
						++generator;
						generator %= CYCLIC_GROUP_SIZE;
					}
					return generator;
				}

				std::tuple<uint64_t, uint64_t, uint64_t> split_element() const {
					uint64_t global_select = (counter * generator) % CYCLIC_GROUP_SIZE;

					uint64_t select_aligned_world = global_select % COUNT_ALIGNED_WORLDS;
					global_select /= COUNT_ALIGNED_WORLDS;

					uint64_t rotation = global_select % COUNT_ROTATIONS;
					global_select /= COUNT_ROTATIONS;

					uint64_t select_target = global_select / COUNT_TARGET_CELLS;

					return std::make_tuple(select_aligned_world, rotation, select_target);
				}

			public:
				original_4_of_16(const std::size_t& counter_p = 0, const std::size_t& generator_p = STANDARD_GENERATOR) {
					counter = counter_p % CYCLIC_GROUP_SIZE;
					generator = generator_p % CYCLIC_GROUP_SIZE;
					increment_generator_until_gcd_1();
				}

				template<class Aggregation_Type>
				Aggregation_Type obtain_standard_4_coloring_permutation(const Aggregation_Type& original_ordered_colors) {
					Aggregation_Type result = original_ordered_colors;
					auto permutation = (counter * SECOND_GENERATOR % CYCLIC_GROUP_SIZE) / (CYCLIC_GROUP_SIZE / (4 * 3 * 2));
					std::swap(result[0], result[permutation % 4]);
					permutation /= 4;
					std::swap(result[1], result[1 + permutation % 3]);
					permutation /= 3;
					std::swap(result[2], result[2 + permutation % 2]);
					return result;
				}

				world_type get_tobor_world() const {
					auto [select_aligned_world, rotation, select_target] = split_element();
					return get_world(select_aligned_world, rotation);
				}

				tobor::v1_0::default_cell_id get_target_cell() const {
					auto w = get_tobor_world();
					std::vector<tobor::v1_0::default_cell_id::int_type> cell_ids;
					const tobor::v1_0::default_cell_id::int_type MIN = 0;
					const tobor::v1_0::default_cell_id::int_type MAX = 15;

					for (tobor::v1_0::default_cell_id::int_type i = 0; i < w.count_cells(); ++i) {
						auto cid = tobor::v1_0::default_cell_id::create_by_id(i, w);
						if (cid.get_x_coord() == MIN || cid.get_x_coord() == MAX)
							continue;
						if (cid.get_y_coord() == MIN || cid.get_y_coord() == MAX)
							continue;

						uint8_t count_walls =
							w.west_wall_by_id(i) +
							w.east_wall_by_id(i) +
							w.south_wall_by_transposed_id(cid.get_transposed_id()) +
							w.north_wall_by_transposed_id(cid.get_transposed_id());

						if (count_walls > 1 && count_walls < 4) {
							cell_ids.push_back(i);
						}
					}

					auto [select_aligned_world, rotation, select_target] = split_element();

					// cell_ids.size() // should always be 11. test this.!!!

					auto index = select_target % cell_ids.size();
					return tobor::v1_0::default_cell_id::create_by_id(cell_ids[index], w);
				}

				inline original_4_of_16& operator++() {
					++counter;
					counter %= CYCLIC_GROUP_SIZE;
				}

				inline original_4_of_16& operator--() {
					counter += CYCLIC_GROUP_SIZE - 1;
					counter %= CYCLIC_GROUP_SIZE;
				}

			};

		}
	}
}


