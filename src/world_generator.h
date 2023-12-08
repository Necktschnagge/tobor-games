#pragma once

#include "models.h"

#include <vector>
#include <array>
#include <optional>

namespace tobor {

	namespace v1_0 {

		namespace world_generator {

			//template<class Cell_Id_Type>
			class original_4_of_16 {
			public:

				using world_type = tobor::v1_0::default_world;

				using cell_id_type = tobor::v1_0::default_cell_id;

				constexpr static std::size_t RED_PLANET{ 0 };
				constexpr static std::size_t GREEN_PLANET{ 1 };
				constexpr static std::size_t BLUE_PLANET{ 2 };
				constexpr static std::size_t YELLOW_PLANET{ 3 };

			private:
				inline static void set_wall_corners(
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

				inline static world_type get_quadrant(std::size_t planet_color, std::size_t quadrant_index) {

					static std::optional<std::array<std::vector<world_type>, 4>> quadrants;

					if (!quadrants.has_value()) {
						quadrants.emplace();
						create_quadrants(quadrants.value());
					}

					return quadrants.value()[planet_color][quadrant_index];
				}

			public:

				constexpr static uint64_t COUNT_CHOOSE_PLANET{ 4 };
				constexpr static uint64_t COUNT_CHOOSE_GEAR{ 4 };
				constexpr static uint64_t COUNT_CHOOSE_CROSS{ 4 };
				constexpr static uint64_t COUNT_CHOOSE_MOON{ 4 };


				constexpr static uint64_t COUNT_ALIGNED_WORLDS{
					COUNT_CHOOSE_PLANET * COUNT_CHOOSE_GEAR * COUNT_CHOOSE_CROSS * COUNT_CHOOSE_MOON *
					3 * // choose the second quadrant's position
					2 // choose the third quadrant's position
				};

				constexpr static uint64_t COUNT_ROTATIONS{ 4 };

				constexpr static uint64_t COUNT_ALL_WORLDS{
					COUNT_ALIGNED_WORLDS * COUNT_ROTATIONS
				};

#if false
				struct turn_access {

					bool is_horizontal = 0;
					bool big_inversion = 0;
					bool little_inversion = 0;

					void turn(uint8_t rotation) {
						auto next_is_horizontal = is_horizontal /*XOR:*/ != !!(rotation % 2);
						auto next_big_inversion = (big_inversion /*XOR*/ != !!()) != !!();
					}

				};

#endif
				inline static void copy_walls_turned(const world_type& source, uint8_t rotation, world_type& destination) {
					rotation %= 4;
					auto  turned = source;
					while (rotation--) {
						turned = turned.turn_left_90();
					}

					/*
					auto rotate_coordinates = [&destination, &source](const cell_id_type& cell_id, uint8_t rotation) {
						//90: yy = x; xx = (YMAX - y)
						cell_id_type::int_type x = cell_id.get_x_coord();
						cell_id_type::int_type y = cell_id.get_y_coord();
						while (rotation--) {
							auto copy = y;
							y = x;
							x = source.get_vertical_size() - copy;
						}
						return cell_id_type::create_by_coordinates(x, y, destination);
						};
					*/

					for (cell_id_type::int_type cell_id = 0; cell_id < source.count_cells(); ++cell_id) {
						destination.west_wall_by_id(cell_id) |= turned.west_wall_by_id(cell_id);
						destination.south_wall_by_transposed_id(cell_id) |= turned.south_wall_by_transposed_id(cell_id);
					}
				}

				static world_type get_world(uint64_t select_aligned_world, uint64_t rotation) {
					rotation %= 4;

					const std::size_t CHOOSE_RED_PLANET = select_aligned_world % 4;
					select_aligned_world /= 4;
					const std::size_t CHOOSE_GREEN_PLANET = select_aligned_world % 4;
					select_aligned_world /= 4;
					const std::size_t CHOOSE_BLUE_PLANET = select_aligned_world % 4;
					select_aligned_world /= 4;
					const std::size_t CHOOSE_YELLOW_PLANET = select_aligned_world % 4;
					select_aligned_world /= 4;

					const uint8_t PERMUTATION_3 = select_aligned_world % 3;
					select_aligned_world /= 3;

					const uint8_t PERMUTATION_2 = select_aligned_world % 2;
					select_aligned_world /= 2;

					if (select_aligned_world) {
						/* number too big*/
					}

					auto aligned_result = world_type(16, 16);
					// ad red planet without turn:
					//copy_walls_turned(get_quadrant(RED_PLANET, CHOOSE_RED_PLANET), 0, aligned_result);

					std::vector<world_type> other_quardants{
						get_quadrant(RED_PLANET, CHOOSE_RED_PLANET),
						get_quadrant(GREEN_PLANET, CHOOSE_GREEN_PLANET),
						get_quadrant(BLUE_PLANET, CHOOSE_BLUE_PLANET),
						get_quadrant(YELLOW_PLANET, CHOOSE_YELLOW_PLANET)
					};
					
					//permutation:
					std::swap(other_quardants[1], other_quardants[1+ PERMUTATION_3]);
					std::swap(other_quardants[2], other_quardants[2+ PERMUTATION_2]);

					for (uint8_t i{ 0 }; i < 4; ++i) {
						//other_quardants[i].second += i;
						copy_walls_turned(other_quardants[i], (rotation + i) % 4, aligned_result);
					}

					return aligned_result;

				}


			};

		}
	}
}


