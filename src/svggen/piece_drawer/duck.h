#pragma once

#include "../piece_drawer.h"

#include "../compound.h"
#include "../path.h"

#include "../path_commands/M.h"
#include "../path_commands/Z.h"
#include "../path_commands/a.h"
#include "../path_commands/c.h"
#include "../path_commands/l.h"

namespace tobor {
	namespace latest {
		namespace svggen {

			namespace pieces {

				template <class Board>
				class duck_piece_drawer : public piece_drawer<Board> {
				public:
					using board     = Board;
					using base_type = piece_drawer<board>;
					using cell_id   = typename base_type::cell_id;

				private:
					bool look_right{ false };

				public:
					virtual std::unique_ptr<fsl::i::stringable>
					   operator()(const board& target_board, const style& drawing_style, const cell_id& piece_position, const std::string& color) override {

						(void) look_right; // TODO!

						auto duck = std::make_unique<path>();

						duck->fill()         = color;
						duck->stroke()       = "black";
						duck->stroke_width() = std::to_string(drawing_style.piece_line_width);

						const auto CELL_CORNER_SOUTH_WEST_x = drawing_style.left_padding + drawing_style.cell_width * piece_position.get_x_coord(target_board);
						const auto CELL_CORNER_SOUTH_WEST_y = drawing_style.top_padding +
						                                      drawing_style.cell_height * (target_board.get_vertical_size() - piece_position.get_y_coord(target_board));

						const auto CANVAS_X_SIZE = drawing_style.cell_width - 2 * drawing_style.horizontal_piece_padding;
						const auto CANVAS_Y_SIZE = drawing_style.cell_height - 2 * drawing_style.vertical_piece_padding;

						const auto MINIMUM_CANVAS_SIZE = std::min(CANVAS_X_SIZE, CANVAS_Y_SIZE);

						const auto FOOT_THICKNESS_RIGHT = CANVAS_Y_SIZE * drawing_style.piece_foot_thickness_factor;

						auto start_at_south_west = std::make_shared<pathc::M<double>>(CELL_CORNER_SOUTH_WEST_x + drawing_style.horizontal_piece_padding,
						                                                              CELL_CORNER_SOUTH_WEST_y - drawing_style.vertical_piece_padding);

						auto go_bottom_east = std::make_shared<pathc::l<double>>(drawing_style.cell_width - 2 * drawing_style.horizontal_piece_padding, 0);

						auto go_bottom_right_corner_north = std::make_shared<pathc::l<double>>(0, -FOOT_THICKNESS_RIGHT);

						auto go_back_neck_bottom_up =
						   std::make_shared<pathc::c<double>>(-0.5 * CANVAS_X_SIZE, 0, -0.5 * CANVAS_X_SIZE, 0, -0.4 * CANVAS_X_SIZE, -(0.45 * CANVAS_Y_SIZE - FOOT_THICKNESS_RIGHT));

						auto go_head = std::make_shared<pathc::a<double>>(pathc::a_step(drawing_style.piece_head_x_radius_factor * MINIMUM_CANVAS_SIZE,
						                                                                drawing_style.piece_head_y_radius_factor * MINIMUM_CANVAS_SIZE,
						                                                                drawing_style.piece_head_axis_rotation,
						                                                                true,
						                                                                false,
						                                                                -0.2 * MINIMUM_CANVAS_SIZE,
						                                                                -0.25 * MINIMUM_CANVAS_SIZE));

						auto go_nort_west_for_beak = std::make_shared<pathc::l<double>>(-0.33 * MINIMUM_CANVAS_SIZE, -0.15 * MINIMUM_CANVAS_SIZE);

						auto go_south_east_for_beak_down = std::make_shared<pathc::l<double>>(0.32 * MINIMUM_CANVAS_SIZE, 0.25 * MINIMUM_CANVAS_SIZE);

						auto go_back = std::make_shared<pathc::Z>();

						duck->path_commands = { start_at_south_west,         go_bottom_east, go_bottom_right_corner_north,
							                     go_back_neck_bottom_up,      go_head,        go_nort_west_for_beak,
							                     go_south_east_for_beak_down, go_back };

						auto outer_eye = std::make_unique<path>();

						outer_eye->fill()         = "white";
						outer_eye->stroke()       = "black";
						outer_eye->stroke_width() = std::to_string(drawing_style.piece_line_width);

						auto start_eye = std::make_shared<pathc::M<double>>(CELL_CORNER_SOUTH_WEST_x + drawing_style.horizontal_piece_padding + 0.66 * MINIMUM_CANVAS_SIZE,
						                                                    CELL_CORNER_SOUTH_WEST_y - (drawing_style.vertical_piece_padding + 0.77 * MINIMUM_CANVAS_SIZE));

						outer_eye->path_commands.push_back(start_eye);

						outer_eye->path_commands.push_back(
						   std::make_shared<pathc::a<double>>(pathc::a_step(0.04 * MINIMUM_CANVAS_SIZE, 0.03 * MINIMUM_CANVAS_SIZE, -10.0, true, false, 0.2, 0.2)));
						outer_eye->path_commands.push_back(go_back);

						auto comp = std::make_unique<compound>(std::move(duck), std::move(outer_eye));

						return comp;
					}

					virtual ~duck_piece_drawer() {}
				};

			} // namespace pieces
		}    // namespace svggen
	}       // namespace latest
} // namespace tobor
