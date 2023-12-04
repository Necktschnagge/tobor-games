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

				constexpr static std::size_t RED_PLANET{ 0 };
				constexpr static std::size_t GREEN_PLANET{ 1 };
				constexpr static std::size_t BLUE_PLANET{ 2 };
				constexpr static std::size_t YELLOW_PLANET{ 3 };

			private:
				inline static void set_wall_corners(
					tobor::v1_0::default_world& world,
					const std::vector<tobor::v1_0::default_cell_id>& W_wall,
					const std::vector<tobor::v1_0::default_cell_id>& S_wall,
					const std::vector<tobor::v1_0::default_cell_id>& NW_corners,
					const std::vector<tobor::v1_0::default_cell_id>& NE_corners,
					const std::vector<tobor::v1_0::default_cell_id>& SW_corners,
					const std::vector<tobor::v1_0::default_cell_id>& SE_corners)
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


				inline static void create_quadrants(std::array<std::vector<world_type>, 4>& all_quadrants);

				inline static world_type get_quadrant(std::size_t planet_color, std::size_t quadrant_index) {

					static std::optional<std::array<std::vector<world_type>, 4>> quadrants;

					if (!quadrants.has_value()) {
						quadrants.emplace();
						create_quadrants(quadrants.value());
					}

					return quadrants.value()[planet_color][quadrant_index];
				}

			public:


			};

		}
	}
}


