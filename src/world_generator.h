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

				using world_type = tobor::v1_0::tobor_world<uint16_t>;

				using cell_id_type = tobor::v1_0::universal_cell_id<world_type>;

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

				constexpr static uint64_t COUNT_TARGET_CELLS{ 17 };

				constexpr static uint64_t COUNT_ALL_WORLDS_WITH_SELECTED_TARGET{ COUNT_ALL_WORLDS * COUNT_TARGET_CELLS };


				static world_type get_quadrant(std::size_t planet_color, std::size_t quadrant_index);

				static void copy_walls_turned(const world_type& source, uint8_t rotation, world_type& destination);

				static world_type get_world(uint64_t select_aligned_world, uint64_t rotation);

				static constexpr uint64_t CYCLIC_GROUP_SIZE{
					COUNT_ALL_WORLDS_WITH_SELECTED_TARGET
				};

			private:
				// standard generator:
				// 4 times: select a quadrant
				// 6 permutations
				// 4 board rotation
				// 17 select a target
				// 
				// 4   x   4   x   4   x   4   x   6   x   4   x   17
				//  1       2       3       0       5                1

				static constexpr uint64_t STANDARD_GENERATOR{
					1 +
					2 * 4 +
					3 * 4 * 4 +
					0 * 4 * 4 * 4 +
					5 * 4 * 4 * 4 * 4 +
					0 * 4 * 4 * 4 * 4 * 6 +
					5 * 4 * 4 * 4 * 4 * 6 * 4
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

			public://remove
				std::tuple<uint64_t, uint64_t, uint64_t> split_element() const {
					uint64_t global_select = (counter * generator) % CYCLIC_GROUP_SIZE;

					uint64_t select_aligned_world = global_select % COUNT_ALIGNED_WORLDS;
					global_select /= COUNT_ALIGNED_WORLDS;

					uint64_t rotation = global_select % COUNT_ROTATIONS;
					global_select /= COUNT_ROTATIONS;

					uint64_t select_target = global_select % COUNT_TARGET_CELLS;

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

				inline uint64_t get_counter() {
					return counter;
				}

				world_type get_tobor_world() const {
					auto [select_aligned_world, rotation, select_target] = split_element();
					return get_world(select_aligned_world, rotation);
				}

				inline static std::vector<cell_id_type::int_type> get_target_cell_id_vector(const world_type& w) {
					//auto w = get_tobor_world();
					std::vector<cell_id_type::int_type> cell_ids;
					const cell_id_type::int_type MIN = 0;
					const cell_id_type::int_type MAX = 15;

					for (cell_id_type::int_type i = 0; i < w.count_cells(); ++i) {
						auto cid = cell_id_type::create_by_id(i, w);
						if (cid.get_x_coord() == MIN || cid.get_x_coord() == MAX)
							continue;
						if (cid.get_y_coord() == MIN || cid.get_y_coord() == MAX)
							continue;

						uint8_t count_walls =
							w.west_wall_by_id(i) +
							w.east_wall_by_id(i) +
							w.south_wall_by_transposed_id(cid.get_transposed_id()) +
							w.north_wall_by_transposed_id(cid.get_transposed_id());

						bool WE = w.west_wall_by_id(i) || w.east_wall_by_id(i);
						bool SN = w.south_wall_by_transposed_id(cid.get_transposed_id()) || w.north_wall_by_transposed_id(cid.get_transposed_id());

						if (count_walls > 1 && count_walls < 4 && WE && SN) {
							cell_ids.push_back(i);
						}
					}

					// check for mis-recognized cells:
					auto weak_neighbour = [&](cell_id_type::int_type l, cell_id_type::int_type r) -> bool {
						auto [lx, ly] = w.cell_id_to_coordinates(l);
						auto [rx, ry] = w.cell_id_to_coordinates(r);

						return (lx == rx && (ly == ry + 1 || ly + 1 == ry)) || (ly == ry && (lx == rx + 1 || lx + 1 == rx));
						};

					std::vector<cell_id_type::int_type> neighbor_counter(cell_ids.size(), 0);

					for (std::size_t i{ 0 }; i < cell_ids.size(); ++i) {
						for (std::size_t j{ 0 }; j < cell_ids.size(); ++j) {
							if (weak_neighbour(cell_ids[i], cell_ids[j])) {
								++neighbor_counter[i];
							}
						}
					}

					std::size_t count_shrink{ 0 };
					for (std::size_t i{ 0 }; i < cell_ids.size(); ++i) {
						auto reverse = cell_ids.size() - 1 - i;
						if (neighbor_counter[reverse] > 1) {
							++count_shrink;
							//this element has to be reased.
							for (std::size_t j{ reverse }; j + 1 < cell_ids.size(); ++j) {
								cell_ids[j] = cell_ids[j + 1];
							}
						}
					}
					cell_ids.erase(cell_ids.begin() + (cell_ids.size() - count_shrink), cell_ids.end());

					return cell_ids;
				}


				cell_id_type get_target_cell() const {
					auto w = get_tobor_world();
					const std::vector<cell_id_type::int_type> cell_ids{ get_target_cell_id_vector(w) };
#if false
					const cell_id_type::int_type MIN = 0;
					const cell_id_type::int_type MAX = 15;

					for (cell_id_type::int_type i = 0; i < w.count_cells(); ++i) {
						auto cid = cell_id_type::create_by_id(i, w);
						if (cid.get_x_coord() == MIN || cid.get_x_coord() == MAX)
							continue;
						if (cid.get_y_coord() == MIN || cid.get_y_coord() == MAX)
							continue;

						uint8_t count_walls =
							w.west_wall_by_id(i) +
							w.east_wall_by_id(i) +
							w.south_wall_by_transposed_id(cid.get_transposed_id()) +
							w.north_wall_by_transposed_id(cid.get_transposed_id());

						bool WE = w.west_wall_by_id(i) || w.east_wall_by_id(i);
						bool SN = w.south_wall_by_transposed_id(cid.get_transposed_id()) || w.north_wall_by_transposed_id(cid.get_transposed_id());

						if (count_walls > 1 && count_walls < 4 && WE && SN) {
							cell_ids.push_back(i);
						}
					}
#endif
					auto [select_aligned_world, rotation, select_target] = split_element();

					// cell_ids.size() // should always be 17. test this.!!!

					auto index = select_target % cell_ids.size();
					return cell_id_type::create_by_id(cell_ids[index], w);
				}

				inline original_4_of_16& operator++() {
					++counter;
					counter %= CYCLIC_GROUP_SIZE;
					return *this;
				}

				inline original_4_of_16& operator--() {
					counter += CYCLIC_GROUP_SIZE - 1;
					counter %= CYCLIC_GROUP_SIZE;
					return *this;
				}

				void set_counter(const uint64_t& counter_p) {
					counter = counter_p % CYCLIC_GROUP_SIZE;
				}

				void set_generator(const uint64_t& generator_p) {
					generator = generator_p % CYCLIC_GROUP_SIZE;
					increment_generator_until_gcd_1();
				}

			};

			class board_size_condition_violation : public std::logic_error {
			public:

				enum class reason_code : int {
					BOARD_SIZE = 0,
					BLOCKED_CELLS_COUNT = 1,

					UNDEFINED = -1
				};

			private:

				reason_code r{ reason_code::UNDEFINED };

				inline std::string make_message() {
					return std::string("board size condition error, code = ") + std::to_string(static_cast<int>(r));
				}

			public:

				inline board_size_condition_violation(reason_code reason) : r(reason), std::logic_error(make_message()) {
					// please test if this is working!
				}

				inline reason_code reason() const { return r; }
			};

			template<class Positions_Of_Pieces_Type, uint64_t _BOARD_SIZE, uint64_t _COUNT_TARGET_ROBOTS, uint64_t _COUNT_NON_TARGET_ROBOTS, uint64_t _BLOCKED_CELLS>
			class initial_state_generator {
			public:

				/* types */

				using positions_of_pieces_type = Positions_Of_Pieces_Type;

				using cell_id_type = typename positions_of_pieces_type::cell_id_type;

				using world_type = typename positions_of_pieces_type::world_type;

				/* integer constants */

				constexpr static uint64_t BOARD_SIZE{ _BOARD_SIZE };
				constexpr static uint64_t COUNT_TARGET_ROBOTS{ _COUNT_TARGET_ROBOTS };
				constexpr static uint64_t COUNT_NON_TARGET_ROBOTS{ _COUNT_NON_TARGET_ROBOTS };
				constexpr static uint64_t BLOCKED_CELLS{ _BLOCKED_CELLS };
				//constexpr static uint64_t GOAL_CELLS{ _GOAL_CELLS };

				constexpr static uint64_t NON_BLOCKED_CELLS{ BOARD_SIZE - BLOCKED_CELLS };
				//constexpr static uint64_t NON_TARGET_FREE_CELLS{ NON_BLOCKED_CELLS - GOAL_CELLS };

				static constexpr uint64_t combinations(const uint64_t& max_factor, const uint64_t& count) {
					if (count == 0) {
						return 1;
					}
					return (max_factor * combinations(max_factor - 1, count - 1)) / count; // check overflow here!!!
				}

				static constexpr uint64_t TARGET_COMBINATIONS{
					combinations(NON_BLOCKED_CELLS, COUNT_TARGET_ROBOTS)
				};
				static constexpr uint64_t NON_TARGET_COMBINATIONS{
					combinations(NON_BLOCKED_CELLS - COUNT_TARGET_ROBOTS, COUNT_NON_TARGET_ROBOTS)
				};
				static constexpr uint64_t CYCLIC_GROUP_SIZE{
					TARGET_COMBINATIONS * NON_TARGET_COMBINATIONS
				}; // counting combinations. We are color-agnostic. We do not distinguish among different non-target pieces. We do not distinguish among different target pieces.


				static constexpr uint64_t STANDARD_GENERATOR_SEED{
					0x1357'9BDF'0246'8ACE
				};

				static constexpr uint64_t gcd(uint64_t x, uint64_t y) {
					if (x == 0)
						return y;
					return gcd(y % x, x);
				}

				static constexpr uint64_t make_generator(uint64_t seed) {

					seed %= CYCLIC_GROUP_SIZE;

					if (gcd(seed, CYCLIC_GROUP_SIZE) == 1) {
						return seed;
					}

					return make_generator(seed + 1);
				}

				static constexpr uint64_t STANDARD_GENERATOR{
					make_generator(STANDARD_GENERATOR_SEED)
				};

				static_assert(gcd(STANDARD_GENERATOR, CYCLIC_GROUP_SIZE) == 1, "check generator");

			private:

				uint64_t generator;
				uint64_t counter;

				std::tuple<uint64_t, uint64_t> split_element() const {
					uint64_t global_select = (counter * generator) % CYCLIC_GROUP_SIZE;

					uint64_t select_target_pieces = global_select % TARGET_COMBINATIONS;

					global_select /= TARGET_COMBINATIONS;

					uint64_t select_non_target_pieces = global_select % NON_TARGET_COMBINATIONS;

					/*
					global_select /= NON_TARGET_COMBINATIONS;
					here global_select == 0
					*/

					return std::make_tuple(select_target_pieces, select_non_target_pieces);
				}

				inline uint64_t& increment_generator_until_gcd_1() {
					while (gcd(generator, CYCLIC_GROUP_SIZE) != 1) {
						++generator;
						generator %= CYCLIC_GROUP_SIZE;
					}
					return generator;
				}

			public:

				initial_state_generator(const std::size_t& counter_p = 0, const std::size_t& generator_p = STANDARD_GENERATOR) {
					counter = counter_p % CYCLIC_GROUP_SIZE;
					generator = generator_p % CYCLIC_GROUP_SIZE;
					increment_generator_until_gcd_1();
				}


				inline uint64_t get_counter() {
					return counter;
				}

				// need to define these types, where is the dependency map?
				positions_of_pieces_type get_positions_of_pieces(const world_type& world, const cell_id_type& target_cell) {

					if (world.count_cells() != BOARD_SIZE) {
						throw board_size_condition_violation(board_size_condition_violation::reason_code::BOARD_SIZE);
					}

					if (world.blocked_cells() != BLOCKED_CELLS) {
						throw board_size_condition_violation(board_size_condition_violation::reason_code::BLOCKED_CELLS_COUNT);
					}


					// write a test for board generator to always fulfill the conditions

					auto [select_target_pieces, select_non_target_pieces] = split_element();

					// first place all target pieces:




				}

				/*

					cell_id_type get_target_cell() const {
						auto w = get_tobor_world();
						std::vector<cell_id_type::int_type> cell_ids;
						const cell_id_type::int_type MIN = 0;
						const cell_id_type::int_type MAX = 15;

						for (cell_id_type::int_type i = 0; i < w.count_cells(); ++i) {
							auto cid = cell_id_type::create_by_id(i, w);
							if (cid.get_x_coord() == MIN || cid.get_x_coord() == MAX)
								continue;
							if (cid.get_y_coord() == MIN || cid.get_y_coord() == MAX)
								continue;

							uint8_t count_walls =
								w.west_wall_by_id(i) +
								w.east_wall_by_id(i) +
								w.south_wall_by_transposed_id(cid.get_transposed_id()) +
								w.north_wall_by_transposed_id(cid.get_transposed_id());

							bool WE = w.west_wall_by_id(i) || w.east_wall_by_id(i);
							bool SN = w.south_wall_by_transposed_id(cid.get_transposed_id()) || w.north_wall_by_transposed_id(cid.get_transposed_id());

							if (count_walls > 1 && count_walls < 4 && WE && SN) {
								cell_ids.push_back(i);
							}
						}

						auto [select_aligned_world, rotation, select_target] = split_element();

						// cell_ids.size() // should always be 17. test this.!!!

						auto index = select_target % cell_ids.size();
						return cell_id_type::create_by_id(cell_ids[index], w);
					}

					inline original_4_of_16& operator++() {
						++counter;
						counter %= CYCLIC_GROUP_SIZE;
						return *this;
					}

					inline original_4_of_16& operator--() {
						counter += CYCLIC_GROUP_SIZE - 1;
						counter %= CYCLIC_GROUP_SIZE;
						return *this;
					}

					void set_counter(const uint64_t& counter_p) {
						counter = counter_p % CYCLIC_GROUP_SIZE;
					}
					// statically by size of cells, #target cells, #non-target cells

					// by some reference board?
					*/
			};

			class product_group_generator {

				// main Group generator size M

				// side Group generator size N

				// generator standard 1

				// counter k in 0..M*N

				// in main group select k mod M

				// in side group select (k mode M + k div M ) mod N

			};

			class dynamic_generator {
				// select a generator, an input, give generated board as output.
			};

		}
	}
}


