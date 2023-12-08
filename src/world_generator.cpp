#include "predefined.h"

#include "world_generator.h"


inline void tobor::v1_0::world_generator::original_4_of_16::set_red_planet_0(world_type& world){
		world.block_center_cells(2, 2);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(4, 0, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 6, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(1, 5, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(6, 2, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(2, 1, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ gear },       // NW
			{ cross },      // NE
			{ moon },       // SW
			{ planet }      // SE
		);
}

inline void tobor::v1_0::world_generator::original_4_of_16::set_wall_corners(world_type& world, const std::vector<cell_id_type>& W_wall, const std::vector<cell_id_type>& S_wall, const std::vector<cell_id_type>& NW_corners, const std::vector<cell_id_type>& NE_corners, const std::vector<cell_id_type>& SW_corners, const std::vector<cell_id_type>& SE_corners)
{
	for (const auto& cell_id : W_wall) {
		world.west_wall_by_id(cell_id.get_id()) = true;
	}
	for (const auto& cell_id : S_wall) {
		world.south_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
	}
	for (const auto& cell_id : SW_corners) {
		world.west_wall_by_id(cell_id.get_id()) = true;
		world.south_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
	}
	for (const auto& cell_id : NW_corners) {
		world.west_wall_by_id(cell_id.get_id()) = true;
		world.north_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
	}
	for (const auto& cell_id : NE_corners) {
		world.east_wall_by_id(cell_id.get_id()) = true;
		world.north_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
	}
	for (const auto& cell_id : SE_corners) {
		world.east_wall_by_id(cell_id.get_id()) = true;
		world.south_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
	}
}

void tobor::v1_0::world_generator::original_4_of_16::create_quadrants(std::array<std::vector<world_type>, 4>& all_quadrants) {

	set_red_planet_0(all_quadrants[RED_PLANET].emplace_back(16, 16));

	{
		all_quadrants[RED_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[RED_PLANET].back();
		world.block_center_cells(2, 2);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(5, 0, world);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 2, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(6, 1, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 6, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 4, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ planet },     // NW
			{ cross },      // NE
			{ moon },       // SW
			{ gear }        // SE
		);
	}
	{
		all_quadrants[RED_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[RED_PLANET].back();
		world.block_center_cells(2, 2);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 6, world);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(4, 0, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(2, 3, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(5, 4, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 5, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ planet },     // NW
			{ cross },      // NE
			{ moon },       // SW
			{ gear }        // SE
		);
	}
	{
		all_quadrants[RED_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[RED_PLANET].back();
		world.block_center_cells(2, 2);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 3, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(2, 5, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(7, 4, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 1, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ gear },       // NW
			{ cross },      // NE
			{ moon },       // SW
			{ planet }      // SE
		);
	}
	{
		all_quadrants[GREEN_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[GREEN_PLANET].back();
		world.block_center_cells(2, 2);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(5, 0, world);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(6, 5, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(1, 2, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(4, 6, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
		auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(7, 3, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ moon },               // NW
			{ cross, swirl },       // NE
			{ gear },               // SW
			{ planet }              // SE
		);
	}
	{
		all_quadrants[GREEN_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[GREEN_PLANET].back();
		world.block_center_cells(2, 2);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 6, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(5, 2, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(4, 5, world);
		auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(5, 7, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ gear },               // NW
			{ cross, swirl },       // NE
			{ planet },             // SW
			{ moon }                // SE
		);
	}
	{
		all_quadrants[GREEN_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[GREEN_PLANET].back();
		world.block_center_cells(2, 2);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(3, 2, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(5, 1, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(3, 3, world);
		auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(7, 5, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ moon },               // NW
			{ cross, swirl },       // NE
			{ gear },               // SW
			{ planet }              // SE
		);
	}
	{
		all_quadrants[GREEN_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[GREEN_PLANET].back();
		world.block_center_cells(2, 2);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(4, 0, world);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 3, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 5, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(4, 3, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(6, 1, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 6, world);
		auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(2, 7, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ gear },               // NW
			{ cross, swirl },       // NE
			{ planet },             // SW
			{ moon }                // SE
		);
	}
	{
		all_quadrants[BLUE_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[BLUE_PLANET].back();
		world.block_center_cells(2, 2);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(1, 6, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(2, 1, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 6, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ planet },     // NW
			{ cross },      // NE
			{ gear },       // SW
			{ moon }        // SE
		);
	}
	{
		all_quadrants[BLUE_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[BLUE_PLANET].back();
		world.block_center_cells(2, 2);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 2, world);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 4, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(3, 6, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(2, 1, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ gear },       // NW
			{ cross },      // NE
			{ moon },       // SW
			{ planet }      // SE
		);
	}
	{
		all_quadrants[BLUE_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[BLUE_PLANET].back();
		world.block_center_cells(2, 2);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 6, world);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(4, 6, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(6, 2, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ gear },       // NW
			{ cross },      // NE
			{ moon },       // SW
			{ planet }      // SE
		);
	}
	{
		all_quadrants[BLUE_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[BLUE_PLANET].back();
		world.block_center_cells(2, 2);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 2, world);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 1, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(3, 6, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(1, 3, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(6, 4, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ planet },     // NW
			{ cross },      // NE
			{ gear },       // SW
			{ moon }        // SE
		);
	}
	{
		all_quadrants[YELLOW_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[YELLOW_PLANET].back();
		world.block_center_cells(2, 2);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(3, 0, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(4, 3, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(6, 5, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ moon },       // NW
			{ cross },      // NE
			{ planet },     // SW
			{ gear }        // SE
		);
	}
	{
		all_quadrants[YELLOW_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[YELLOW_PLANET].back();
		world.block_center_cells(2, 2);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 2, world);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 4, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 5, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ moon },       // NW
			{ cross },      // NE
			{ planet },     // SW
			{ gear }        // SE
		);
	}
	{
		all_quadrants[YELLOW_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[YELLOW_PLANET].back();
		world.block_center_cells(2, 2);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(5, 0, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(2, 5, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(5, 7, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(6, 1, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ moon },       // NW
			{ cross },      // NE
			{ planet },     // SW
			{ gear }        // SE
		);
	}
	{
		all_quadrants[YELLOW_PLANET].emplace_back(16, 16);
		auto& world = all_quadrants[YELLOW_PLANET].back();
		world.block_center_cells(2, 2);
		auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
		auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
		auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
		auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
		auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(1, 1, world);
		auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 7, world);
		set_wall_corners(
			world,
			{ left },
			{ bottom },
			{ moon },       // NW
			{ cross },      // NE
			{ planet },     // SW
			{ gear }        // SE
		);
	}

}
