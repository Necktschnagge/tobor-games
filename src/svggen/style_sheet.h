#pragma once

namespace tobor {
	namespace latest {
		namespace svggen {

			struct style {
				double half_grid_line_width{ 1.5 };
				double half_wall_line_width{ 5 };
				double wall_corner_stretch{ 0.6 };
				double piece_line_width{ 0.3 * half_wall_line_width };
				double vertical_piece_padding{ half_wall_line_width * 3 };
				double horizontal_piece_padding{ half_wall_line_width * 3 };
				double piece_foot_thickness_factor{ 0.15 };
				double piece_head_x_radius_factor{ 0.3 };
				double piece_head_y_radius_factor{ 0.2 };
				double piece_head_axis_rotation{ -50 };
				// each cell is 100 x 100
				// on each side we define a padding of 50. -> canvas is 1700 ^2
				double cell_height{ 100.0 };
				double cell_width{ cell_height };
				double left_padding{ 0.5 * cell_height };
				double right_padding{ 0.5 * cell_height };
				double top_padding{ 0.5 * cell_height };
				double bottom_padding{ 0.5 * cell_height };
			};

		} // namespace svggen
	}    // namespace latest
} // namespace tobor
