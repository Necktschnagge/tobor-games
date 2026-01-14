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
				class spiky_ball : public latest::svggen::piece_drawer<Board> {
				public:
					using world_type = Board;

					using base_type = piece_drawer<world_type>;

					using cell_id_type = typename base_type::cell_id;

				private:
					double param{ 0.2 };

					inline double weighted_sum(double from, double to) { return (1.0 - param) * from + param * to; }

				public:
					virtual std::unique_ptr<fsl::i::stringable>
					   operator()(const world_type& tobor_world, const style& dss, const cell_id_type& cell, const std::string& color) override {

						auto marker = std::make_unique<path>();

						marker->fill()         = color;
						marker->stroke()       = "black";
						marker->stroke_width() = std::to_string(dss.piece_line_width);

						const auto CELL_CORNER_SOUTH_WEST_x = dss.left_padding + dss.cell_width * cell.get_x_coord(tobor_world);
						const auto CELL_CORNER_SOUTH_WEST_y = dss.top_padding + dss.cell_height * (tobor_world.get_vertical_size() - cell.get_y_coord(tobor_world));

						const auto CANVAS_X_SIZE = dss.cell_width - 2 * dss.horizontal_piece_padding;
						const auto CANVAS_Y_SIZE = dss.cell_height - 2 * dss.vertical_piece_padding;

						const auto HALF_CANVAS_X_SIZE = CANVAS_X_SIZE / 2;
						const auto HALF_CANVAS_Y_SIZE = CANVAS_Y_SIZE / 2;

						// const auto MINIMUM_CANVAS_SIZE = std::min(CANVAS_X_SIZE, CANVAS_Y_SIZE);

						// const auto FOOT_THICKNESS_RIGHT = CANVAS_Y_SIZE * dss.PIECE_FOOT_THICKNESS_FACTOR;

						const auto CENTER_X{ CELL_CORNER_SOUTH_WEST_x + dss.cell_width / 2 };
						const auto CENTER_Y{ CELL_CORNER_SOUTH_WEST_y - dss.cell_height / 2 };

						// const auto TOP_Y{ dss.top_padding + dss.cell_height * (tobor_world.get_vertical_size() - cell.get_y_coord() - 1) + dss.vertical_piece_padding };
						// const auto BOTTOM_Y{ CELL_CORNER_SOUTH_WEST_y - dss.vertical_piece_padding };

						const double SINCOS45{ 0.7071 };

						const auto COORD_DISTANCE_CORNER{ (CANVAS_X_SIZE / 2) * SINCOS45 };

						marker->path_commands.push_back(std::make_shared<pathc::M<double>>(CENTER_X, CENTER_Y + HALF_CANVAS_Y_SIZE));

						// remove unused consts in this function!

						marker->path_commands.push_back(std::make_shared<pathc::C<double>>(weighted_sum(CENTER_X, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y + HALF_CANVAS_Y_SIZE, CENTER_Y),
						                                                                                    weighted_sum(CENTER_X + COORD_DISTANCE_CORNER, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y + COORD_DISTANCE_CORNER, CENTER_Y),
						                                                                                    CENTER_X + COORD_DISTANCE_CORNER,
						                                                                                    CENTER_Y + COORD_DISTANCE_CORNER));
						marker->path_commands.push_back(std::make_shared<pathc::C<double>>(weighted_sum(CENTER_X + COORD_DISTANCE_CORNER, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y + COORD_DISTANCE_CORNER, CENTER_Y),
						                                                                                    weighted_sum(CENTER_X + HALF_CANVAS_X_SIZE, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y, CENTER_Y),
						                                                                                    CENTER_X + HALF_CANVAS_X_SIZE,
						                                                                                    CENTER_Y));
						marker->path_commands.push_back(std::make_shared<pathc::C<double>>(weighted_sum(CENTER_X + HALF_CANVAS_X_SIZE, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y, CENTER_Y),
						                                                                                    weighted_sum(CENTER_X + COORD_DISTANCE_CORNER, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y - COORD_DISTANCE_CORNER, CENTER_Y),
						                                                                                    CENTER_X + COORD_DISTANCE_CORNER,
						                                                                                    CENTER_Y - COORD_DISTANCE_CORNER));
						marker->path_commands.push_back(std::make_shared<pathc::C<double>>(weighted_sum(CENTER_X + COORD_DISTANCE_CORNER, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y - COORD_DISTANCE_CORNER, CENTER_Y),
						                                                                                    weighted_sum(CENTER_X, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y - HALF_CANVAS_Y_SIZE, CENTER_Y),
						                                                                                    CENTER_X,
						                                                                                    CENTER_Y - HALF_CANVAS_Y_SIZE));
						marker->path_commands.push_back(std::make_shared<pathc::C<double>>(weighted_sum(CENTER_X, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y - HALF_CANVAS_Y_SIZE, CENTER_Y),
						                                                                                    weighted_sum(CENTER_X - COORD_DISTANCE_CORNER, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y - COORD_DISTANCE_CORNER, CENTER_Y),
						                                                                                    CENTER_X - COORD_DISTANCE_CORNER,
						                                                                                    CENTER_Y - COORD_DISTANCE_CORNER));
						marker->path_commands.push_back(std::make_shared<pathc::C<double>>(weighted_sum(CENTER_X - COORD_DISTANCE_CORNER, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y - COORD_DISTANCE_CORNER, CENTER_Y),
						                                                                                    weighted_sum(CENTER_X - HALF_CANVAS_X_SIZE, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y, CENTER_Y),
						                                                                                    CENTER_X - HALF_CANVAS_X_SIZE,
						                                                                                    CENTER_Y));
						marker->path_commands.push_back(std::make_shared<pathc::C<double>>(weighted_sum(CENTER_X - HALF_CANVAS_X_SIZE, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y, CENTER_Y),
						                                                                                    weighted_sum(CENTER_X - COORD_DISTANCE_CORNER, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y + COORD_DISTANCE_CORNER, CENTER_Y),
						                                                                                    CENTER_X - COORD_DISTANCE_CORNER,
						                                                                                    CENTER_Y + COORD_DISTANCE_CORNER));
						marker->path_commands.push_back(std::make_shared<pathc::C<double>>(weighted_sum(CENTER_X - COORD_DISTANCE_CORNER, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y + COORD_DISTANCE_CORNER, CENTER_Y),
						                                                                                    weighted_sum(CENTER_X, CENTER_X),
						                                                                                    weighted_sum(CENTER_Y + HALF_CANVAS_Y_SIZE, CENTER_Y),
						                                                                                    CENTER_X,
						                                                                                    CENTER_Y + HALF_CANVAS_Y_SIZE));

						marker->path_commands.push_back(std::make_shared<pathc::Z>());

						return marker;
					}

					virtual ~spiky_ball() {}
				};

			} // namespace pieces
		}    // namespace svggen
	}       // namespace latest
} // namespace tobor
