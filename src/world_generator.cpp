#include "predefined.h"

#include "world_generator.h"

void tobor::v1_0::world_generator::original_4_of_16::set_wall_corners(
    world_type &world, const std::vector<cell_id_type> &W_wall,
    const std::vector<cell_id_type> &S_wall,
    const std::vector<cell_id_type> &NW_corners,
    const std::vector<cell_id_type> &NE_corners,
    const std::vector<cell_id_type> &SW_corners,
    const std::vector<cell_id_type> &SE_corners) {
  for (const auto &cell_id : W_wall) {
    world.west_wall_by_id(cell_id.get_id()) = true;
  }
  for (const auto &cell_id : S_wall) {
    world.south_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
  }
  for (const auto &cell_id : SW_corners) {
    world.west_wall_by_id(cell_id.get_id()) = true;
    world.south_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
  }
  for (const auto &cell_id : NW_corners) {
    world.west_wall_by_id(cell_id.get_id()) = true;
    world.north_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
  }
  for (const auto &cell_id : NE_corners) {
    world.east_wall_by_id(cell_id.get_id()) = true;
    world.north_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
  }
  for (const auto &cell_id : SE_corners) {
    world.east_wall_by_id(cell_id.get_id()) = true;
    world.south_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
  }
}

void tobor::v1_0::world_generator::original_4_of_16::set_red_planet_0(
    world_type &world) {
  world.block_center_cells(2, 2);
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(4, 0, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 6, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(1, 5, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(6, 2, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(2, 1, world);
  set_wall_corners(world, {left}, {bottom}, {gear}, // NW
                   {cross},                         // NE
                   {moon},                          // SW
                   {planet}                         // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_red_planet_1(
    world_type &world) {
  world.block_center_cells(2, 2);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(5, 0, world);
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 2, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(6, 1, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 6, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(5, 4, world);
  set_wall_corners(world, {left}, {bottom}, {planet}, // NW
                   {cross},                           // NE
                   {moon},                            // SW
                   {gear}                             // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_red_planet_2(
    world_type &world) {
  world.block_center_cells(2, 2);
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 6, world);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(4, 0, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(2, 3, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(5, 4, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 5, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
  set_wall_corners(world, {left}, {bottom}, {planet}, // NW
                   {cross},                           // NE
                   {moon},                            // SW
                   {gear}                             // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_red_planet_3(
    world_type &world) {
  world.block_center_cells(2, 2);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 3, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(2, 5, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(7, 4, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(5, 1, world);
  set_wall_corners(world, {left}, {bottom}, {gear}, // NW
                   {cross},                         // NE
                   {moon},                          // SW
                   {planet}                         // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_green_planet_0(
    world_type &world) {
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(5, 0, world);
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(6, 5, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(1, 2, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(4, 6, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
  auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(7, 3, world);
  set_wall_corners(world, {left}, {bottom}, {moon}, // NW
                   {cross, swirl},                  // NE
                   {gear},                          // SW
                   {planet}                         // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_green_planet_1(
    world_type &world) {
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 6, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(5, 2, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(4, 5, world);
  auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(5, 7, world);
  set_wall_corners(world, {left}, {bottom}, {gear}, // NW
                   {cross, swirl},                  // NE
                   {planet},                        // SW
                   {moon}                           // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_green_planet_2(
    world_type &world) {
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(3, 2, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(5, 1, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(3, 3, world);
  auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(7, 5, world);
  set_wall_corners(world, {left}, {bottom}, {moon}, // NW
                   {cross, swirl},                  // NE
                   {gear},                          // SW
                   {planet}                         // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_green_planet_3(
    world_type &world) {
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(4, 0, world);
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 3, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 5, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(4, 3, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(6, 1, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(5, 6, world);
  auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(2, 7, world);
  set_wall_corners(world, {left}, {bottom}, {gear}, // NW
                   {cross, swirl},                  // NE
                   {planet},                        // SW
                   {moon}                           // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_blue_planet_0(
    world_type &world) {
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(1, 6, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(2, 1, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(5, 6, world);
  set_wall_corners(world, {left}, {bottom}, {planet}, // NW
                   {cross},                           // NE
                   {gear},                            // SW
                   {moon}                             // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_blue_planet_1(
    world_type &world) {
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 2, world);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 4, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(3, 6, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(2, 1, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
  set_wall_corners(world, {left}, {bottom}, {gear}, // NW
                   {cross},                         // NE
                   {moon},                          // SW
                   {planet}                         // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_blue_planet_2(
    world_type &world) {
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 6, world);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(4, 6, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(6, 2, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
  set_wall_corners(world, {left}, {bottom}, {gear}, // NW
                   {cross},                         // NE
                   {moon},                          // SW
                   {planet}                         // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_blue_planet_3(
    world_type &world) {
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 2, world);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 1, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(3, 6, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(1, 3, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(6, 4, world);
  set_wall_corners(world, {left}, {bottom}, {planet}, // NW
                   {cross},                           // NE
                   {gear},                            // SW
                   {moon}                             // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_yellow_planet_0(
    world_type &world) {
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(3, 0, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(4, 3, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(6, 5, world);
  set_wall_corners(world, {left}, {bottom}, {moon}, // NW
                   {cross},                         // NE
                   {planet},                        // SW
                   {gear}                           // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_yellow_planet_1(
    world_type &world) {
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 2, world);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 4, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(5, 5, world);
  set_wall_corners(world, {left}, {bottom}, {moon}, // NW
                   {cross},                         // NE
                   {planet},                        // SW
                   {gear}                           // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_yellow_planet_2(
    world_type &world) {
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(5, 0, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(2, 5, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(5, 7, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(6, 1, world);
  set_wall_corners(world, {left}, {bottom}, {moon}, // NW
                   {cross},                         // NE
                   {planet},                        // SW
                   {gear}                           // SE
  );
}

void tobor::v1_0::world_generator::original_4_of_16::set_yellow_planet_3(
    world_type &world) {
  auto bottom =
      tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
  auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
  auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
  auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
  auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(1, 1, world);
  auto planet =
      tobor::v1_0::default_cell_id::create_by_coordinates(5, 7, world);
  set_wall_corners(world, {left}, {bottom}, {moon}, // NW
                   {cross},                         // NE
                   {planet},                        // SW
                   {gear}                           // SE
  );
}

inline void tobor::v1_0::world_generator::original_4_of_16::create_quadrants(
    std::array<std::vector<world_type>, 4> &all_quadrants) {

  set_red_planet_0(all_quadrants[RED_PLANET].emplace_back(16, 16));
  set_red_planet_1(all_quadrants[RED_PLANET].emplace_back(16, 16));
  set_red_planet_2(all_quadrants[RED_PLANET].emplace_back(16, 16));
  set_red_planet_3(all_quadrants[RED_PLANET].emplace_back(16, 16));

  set_green_planet_0(all_quadrants[GREEN_PLANET].emplace_back(16, 16));
  set_green_planet_1(all_quadrants[GREEN_PLANET].emplace_back(16, 16));
  set_green_planet_2(all_quadrants[GREEN_PLANET].emplace_back(16, 16));
  set_green_planet_3(all_quadrants[GREEN_PLANET].emplace_back(16, 16));

  set_blue_planet_0(all_quadrants[BLUE_PLANET].emplace_back(16, 16));
  set_blue_planet_1(all_quadrants[BLUE_PLANET].emplace_back(16, 16));
  set_blue_planet_2(all_quadrants[BLUE_PLANET].emplace_back(16, 16));
  set_blue_planet_3(all_quadrants[BLUE_PLANET].emplace_back(16, 16));

  set_yellow_planet_0(all_quadrants[YELLOW_PLANET].emplace_back(16, 16));
  set_yellow_planet_1(all_quadrants[YELLOW_PLANET].emplace_back(16, 16));
  set_yellow_planet_2(all_quadrants[YELLOW_PLANET].emplace_back(16, 16));
  set_yellow_planet_3(all_quadrants[YELLOW_PLANET].emplace_back(16, 16));
}

tobor::v1_0::world_generator::original_4_of_16::world_type
tobor::v1_0::world_generator::original_4_of_16::get_quadrant(
    std::size_t planet_color, std::size_t quadrant_index) {
  static std::optional<std::array<std::vector<world_type>, 4>> quadrants;

  if (!quadrants.has_value()) {
    quadrants.emplace();
    create_quadrants(quadrants.value());
  }
  return quadrants.value()[planet_color][quadrant_index];
}

void tobor::v1_0::world_generator::original_4_of_16::copy_walls_turned(
    const world_type &source, uint8_t rotation, world_type &destination) {
  rotation %= 4;
  auto turned = source;
  while (rotation--) {
    turned = turned.turn_left_90();
  }
  for (cell_id_type::int_type i = 0; i < source.count_cells(); ++i) {
    destination.west_wall_by_id(i) |= turned.west_wall_by_id(i);
    destination.south_wall_by_transposed_id(i) |=
        turned.south_wall_by_transposed_id(i);
  }
}

tobor::v1_0::world_generator::original_4_of_16::world_type
tobor::v1_0::world_generator::original_4_of_16::get_world(
    uint64_t select_aligned_world, uint64_t rotation) {
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

  std::vector<world_type> other_quardants{
      get_quadrant(RED_PLANET, CHOOSE_RED_PLANET),
      get_quadrant(GREEN_PLANET, CHOOSE_GREEN_PLANET),
      get_quadrant(BLUE_PLANET, CHOOSE_BLUE_PLANET),
      get_quadrant(YELLOW_PLANET, CHOOSE_YELLOW_PLANET)};

  // permutation:
  std::swap(other_quardants[1], other_quardants[1 + PERMUTATION_3]);
  std::swap(other_quardants[2], other_quardants[2 + PERMUTATION_2]);

  for (uint8_t i{0}; i < 4; ++i) {
    copy_walls_turned(other_quardants[i], static_cast<uint8_t>(rotation) + i,
                      aligned_result);
  }

  return aligned_result;
}
