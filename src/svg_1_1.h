#pragma once

#include "models/dynamic_rectangle_world.h"
#include "models/min_size_cell_id.h"

#include "svggen/piece_drawer.h"
#include "svggen/path.h"
#include "svggen/piece_drawer/duck.h"
#include "svggen/piece_drawer/spiky_ball.h"


#include "svg_1_0.h"

namespace tobor {
	namespace latest {
		namespace svggen {

			enum class general_piece_shape_selection { BALL, DUCK };

			template <class Board, class PositionsOfPieces>
			class board_composer {
			public:
				using board = Board;

				using positions_of_pieces_type = PositionsOfPieces;

				using cell_id_type = typename positions_of_pieces_type::cell_id_type;

				using cell_size_int = typename cell_id_type::int_size_type;

				using cell_narrow_int = typename cell_id_type::int_cell_id_type;

				using pieces_quantity_type = typename positions_of_pieces_type::pieces_quantity_type;

				using pq_size_type = typename pieces_quantity_type::int_type;

				static constexpr pq_size_type COUNT_ALL_PIECES{ pieces_quantity_type::COUNT_ALL_PIECES };

				struct coloring {
					using array_type = std::array<std::string, COUNT_ALL_PIECES>;

					array_type colors;

					template <class... T>
					coloring(T&&... args) : colors{ std::forward<T>(args)... } {}
				};

				using piece_shape_selection = general_piece_shape_selection;

			private:
				inline static std::unique_ptr<fsl::i::stringable> draw_tobor_background(const board& source_board, const style& dss) {
					auto svg_background            = std::make_unique<path>();
					svg_background->fill()         = "lightyellow";
					svg_background->stroke_width() = "0";
					svg_background->stroke()       = svg_background->fill();

					const double horizontal_size = dss.left_padding + dss.cell_width * source_board.get_horizontal_size() + dss.right_padding;
					const double vertical_size   = dss.top_padding + dss.cell_height * source_board.get_vertical_size() + dss.bottom_padding;

					auto start_at_0_0            = std::make_shared<pathc::M<double>>(0, 0);
					auto go_to_right_bottom_left = std::make_shared<pathc::l<double>>(horizontal_size, 0, 0, vertical_size, 0 - horizontal_size, 0);
					auto go_back                 = std::make_shared<pathc::Z>();

					svg_background->path_commands.push_back(start_at_0_0);
					svg_background->path_commands.push_back(go_to_right_bottom_left);
					svg_background->path_commands.push_back(go_back);

					return svg_background;
				}

				inline static std::unique_ptr<path> get_vertical_grid_element(const board& source_board, const style& dss, std::size_t cell_count_offset) {
					auto grid_element = std::make_unique<path>();

					grid_element->fill()         = "dimgrey"; // !80 #0F54DA";
					grid_element->stroke()       = grid_element->fill();
					grid_element->stroke_width() = "0";

					auto start_at_upper_left = std::make_shared<pathc::M<double>>(dss.left_padding - dss.half_grid_line_width + dss.cell_width * cell_count_offset,
					                                                                               dss.top_padding - dss.half_grid_line_width);
					auto go_to_right_bottom_left = std::make_shared<pathc::l<double>>(dss.half_grid_line_width * 2,
					                                                                                   0,
					                                                                                   0,
					                                                                                   dss.cell_height * source_board.get_vertical_size() +
					                                                                                      dss.half_grid_line_width * 2,
					                                                                                   -dss.half_grid_line_width * 2,
					                                                                                   0);
					auto go_back                 = std::make_shared<pathc::Z>();

					grid_element->path_commands = { start_at_upper_left, go_to_right_bottom_left, go_back };

					return grid_element;
				}

				inline static std::unique_ptr<path> get_horizontal_grid_element(const board& source_board, const style& dss, std::size_t cell_count_offset) {
					auto grid_element = std::make_unique<path>();

					grid_element->fill()         = "dimgrey"; // !80 #0F54DA";
					grid_element->stroke()       = grid_element->fill();
					grid_element->stroke_width() = "0";

					auto start_at_upper_left    = std::make_shared<pathc::M<double>>(dss.left_padding - dss.half_grid_line_width,
                                                                                              dss.top_padding - dss.half_grid_line_width +
                                                                                                 dss.cell_height * cell_count_offset);
					auto go_to_bottom_right_top = std::make_shared<pathc::l<double>>(0,
					                                                                                  dss.half_grid_line_width * 2,
					                                                                                  dss.cell_width * source_board.get_vertical_size() +
					                                                                                     dss.half_grid_line_width * 2,
					                                                                                  0,
					                                                                                  0,
					                                                                                  -dss.half_grid_line_width * 2);
					auto go_back                = std::make_shared<pathc::Z>();

					grid_element->path_commands = { start_at_upper_left, go_to_bottom_right_top, go_back };

					return grid_element;
				}

				inline static std::unique_ptr<fsl::i::stringable> draw_tobor_grid(const board& source_board, const style& dss) {
					auto svg_grid = std::make_unique<latest::svggen::compound>();

					for (std::size_t i{ 0 }; i <= source_board.get_horizontal_size(); ++i) { // draw vertical grid lines
						svg_grid->elements.push_back(get_vertical_grid_element(source_board, dss, i));
					}

					for (std::size_t i{ 0 }; i <= source_board.get_vertical_size(); ++i) { // draw horizontal grid lines
						svg_grid->elements.push_back(get_horizontal_grid_element(source_board, dss, i));
					}

					return svg_grid;
				}

				inline static std::unique_ptr<path> get_horizontal_south_wall(const board& source_board, const style& dss, std::size_t x, std::size_t y) {
					auto horizontal_wall_element = std::make_unique<path>();

					horizontal_wall_element->fill()         = "black"; // !80 #0F54DA";
					horizontal_wall_element->stroke()       = horizontal_wall_element->fill();
					horizontal_wall_element->stroke_width() = "0";

					auto start_at_upper_left = std::make_shared<pathc::M<double>>(dss.left_padding + dss.cell_width * x,
					                                                                               dss.top_padding - dss.half_wall_line_width +
					                                                                                  dss.cell_height * (source_board.get_vertical_size() - y));

					auto left_semicircle = std::make_shared<pathc::c<double>>(-dss.half_wall_line_width * 2 * dss.wall_corner_stretch,
					                                                                           0,
					                                                                           -dss.half_wall_line_width * 2 * dss.wall_corner_stretch,
					                                                                           2 * dss.half_wall_line_width,
					                                                                           0,
					                                                                           2 * dss.half_wall_line_width);

					auto go_right = std::make_shared<pathc::l<double>>(dss.cell_width, 0);

					auto right_semicircle = std::make_shared<pathc::c<double>>(dss.half_wall_line_width * 2 * dss.wall_corner_stretch,
					                                                                            0,
					                                                                            dss.half_wall_line_width * 2 * dss.wall_corner_stretch,
					                                                                            -2 * dss.half_wall_line_width,
					                                                                            0,
					                                                                            -2 * dss.half_wall_line_width);

					auto go_back = std::make_shared<pathc::Z>();

					horizontal_wall_element->path_commands = { start_at_upper_left, left_semicircle, go_right, right_semicircle, go_back };

					return horizontal_wall_element;
				}

				inline static std::unique_ptr<path> get_vertical_west_wall(const board& source_board, const style& dss, std::size_t x, std::size_t y) {
					auto vertical_wall_element = std::make_unique<path>();

					vertical_wall_element->fill()         = "black"; // !80 #0F54DA";
					vertical_wall_element->stroke()       = vertical_wall_element->fill();
					vertical_wall_element->stroke_width() = "0";

					auto start_at_lower_left = std::make_shared<pathc::M<double>>(dss.left_padding - dss.half_wall_line_width + dss.cell_width * x,
					                                                                               dss.top_padding + dss.cell_height * (source_board.get_vertical_size() - y));

					auto bottom_semicircle = std::make_shared<pathc::c<double>>(0,
					                                                                             dss.half_wall_line_width * 2 * dss.wall_corner_stretch,
					                                                                             2 * dss.half_wall_line_width,
					                                                                             dss.half_wall_line_width * 2 * dss.wall_corner_stretch,
					                                                                             2 * dss.half_wall_line_width,
					                                                                             0);

					auto go_north = std::make_shared<pathc::l<double>>(0, -dss.cell_width);

					auto top_semicircle = std::make_shared<pathc::c<double>>(0,
					                                                                          -dss.half_wall_line_width * 2 * dss.wall_corner_stretch,
					                                                                          -2 * dss.half_wall_line_width,
					                                                                          -dss.half_wall_line_width * 2 * dss.wall_corner_stretch,
					                                                                          -2 * dss.half_wall_line_width,
					                                                                          0);

					auto go_back = std::make_shared<pathc::Z>();

					vertical_wall_element->path_commands = { start_at_lower_left, bottom_semicircle, go_north, top_semicircle, go_back };

					return vertical_wall_element;
				}

				inline static std::unique_ptr<path>
				   fill_whole_cell(const board& source_board, const style& dss, const cell_id_type& cell_id, const std::string& color) {
					auto whole_cell = std::make_unique<path>();

					whole_cell->fill()         = color;
					whole_cell->stroke()       = whole_cell->fill();
					whole_cell->stroke_width() = "0";

					auto start_at_north_west = std::make_shared<pathc::M<double>>(dss.left_padding + dss.cell_width * cell_id.get_x_coord(source_board),
					                                                                               dss.top_padding + dss.cell_height * (source_board.get_vertical_size() -
					                                                                                                                    cell_id.get_y_coord(source_board) - 1));

					auto go_east = std::make_shared<pathc::l<double>>(dss.cell_width, 0);

					auto go_south = std::make_shared<pathc::l<double>>(0, dss.cell_height);

					auto go_west = std::make_shared<pathc::l<double>>(-dss.cell_width, 0);

					auto go_back = std::make_shared<pathc::Z>();

					whole_cell->path_commands = { start_at_north_west, go_east, go_south, go_west, go_back };

					return whole_cell;
				}

				inline static std::unique_ptr<fsl::i::stringable> draw_blocked_cells(const board& source_board, const style& dss) {

					auto blocked_cells = std::make_unique<latest::svggen::compound>();

					for (cell_size_int raw_id{ 0 }; raw_id < source_board.count_cells(); ++raw_id) {

						const cell_id_type    cell_id = cell_id_type::create_by_id(raw_id, source_board);
						const cell_narrow_int nid{ board::narrow(raw_id) };
						const cell_narrow_int ntid{ cell_id.get_transposed_id(source_board) };

						if (source_board.west_wall_by_id(nid) && source_board.east_wall_by_id(nid) && source_board.south_wall_by_transposed_id(ntid) &&
						    source_board.north_wall_by_transposed_id(ntid)) {

							auto block = fill_whole_cell(source_board, dss, cell_id, "black");
							blocked_cells->elements.push_back(std::move(block));
						}
					}

					return blocked_cells;
				}

				inline static std::unique_ptr<fsl::i::stringable> draw_walls(const board& source_board, const style& dss) {
					auto svg_walls = std::make_unique<latest::svggen::compound>();

					// horizontal walls:
					for (cell_size_int x = 0; x < source_board.get_horizontal_size(); ++x) {
						for (cell_size_int y = 0; y <= source_board.get_vertical_size(); ++y) {
							if (source_board.south_wall_by_transposed_id(source_board.coordinates_to_transposed_cell_id(board::narrow(x), board::narrow(y)))) {
								// ! Note that y exceeds its natural range by 1. But it is okay, since we operate on an infinite repeating 2D landscape
								svg_walls->elements.push_back(get_horizontal_south_wall(source_board, dss, x, y));
							}
						}
					}

					// vertical walls:
					for (cell_size_int x = 0; x <= source_board.get_horizontal_size(); ++x) {
						for (cell_size_int y = 0; y < source_board.get_vertical_size(); ++y) {
							if (source_board.west_wall_by_id(source_board.coordinates_to_cell_id(board::narrow(x), board::narrow(y)))) {
								// ! Note that y exceeds its natural range by 1. But it is okay, since we operate on an infinite repeating 2D landscape
								svg_walls->elements.push_back(get_vertical_west_wall(source_board, dss, x, y));
							}
						}
					}
					return svg_walls;
				}

			public:
				static_assert(pieces_quantity_type::COUNT_TARGET_PIECES == 1, "Not yet supported: multiple target pieces");

				inline static std::string
				   draw_source_board(const board& tw, const positions_of_pieces_type& pop, const cell_id_type& target_cell, const coloring& c, piece_shape_selection shape) {
					std::unique_ptr<piece_drawer<board>> piece_drawer;
					if (shape == piece_shape_selection::BALL) {
						piece_drawer = std::make_unique<pieces::spiky_ball<board>>();
					} else if (shape == piece_shape_selection::DUCK) {
						piece_drawer = std::make_unique<pieces::duck_piece_drawer<board>>();
					} else {
						throw std::invalid_argument("Unknown shape in SVG draw process.");
					}

					style dss;

					auto svg_target = fill_whole_cell(tw, dss, target_cell, c.colors[0]);

					auto svg_pieces = std::make_unique<latest::svggen::compound>();

					for (pq_size_type pid = 0; pid < pieces_quantity_type::COUNT_ALL_PIECES; ++pid) {
						svg_pieces->elements.push_back(

						   // draw_duck_piece(tw, dss, pop.piece_positions[pid], true, c.colors[pid])

						   (*piece_drawer)(tw, dss, pop.piece_positions()[pid], c.colors[pid])

						);
					}

					// The following order is final:
					auto svg_body = std::make_unique<latest::svggen::compound>(draw_tobor_background(tw, dss),
					                                                           draw_blocked_cells(tw, dss),
					                                                           std::move(svg_target),
					                                                           draw_tobor_grid(tw, dss),
					                                                           draw_walls(tw, dss),
					                                                           std::move(svg_pieces));
					/* ORDER REQUIREMENTS

					   background is behind everything else					// otherwise cannot see anything
					   grid is behind walls									// walls should cover grid of cause

					   grid is in front of target								// otherwise grid would be covered partially
					   walls are in front of target							// since walls partially covered otherwise

					   target is in front of blocked							// if by error, target is blocked, we still see target, we also indirectly see it is blocked by seeing
					   walls all around

					   pieces are in front of tagert							// since piece covered otherwise when in target cell

					   background   <---   blocked   <---   target   <---   grid   <---   walls
					                                 |
					                                 |

					                                 <---   pieces

					   By now there are three variants to place pieces behind/in front of  ...  grid / walls
					*/

					const std::string svg_root_height = std::to_string(dss.cell_height * tw.get_vertical_size() + dss.top_padding + dss.bottom_padding);
					const std::string svg_root_width  = std::to_string(dss.cell_width * tw.get_vertical_size() + dss.left_padding + dss.right_padding);
					auto              svg_root        = std::make_unique<::tobor::latest::svggen::xml_root_element>(svg_root_height, svg_root_width, std::move(svg_body));

					return svg_root->to_string();
				}

				template <class... T>
				inline static std::string draw_source_board_with_cell_markers(const tobor::v1_1::dynamic_rectangle_world<T...>& tw, const std::vector<cell_id_type>& markers
				                                                              /*, coloring*/
				) {
					pieces::spiky_ball<tobor::v1_1::dynamic_rectangle_world<T...>> spiky_ball_drawer;

					style dss;

					auto svg_cell_markers = std::make_unique<latest::svggen::compound>();

					for (const cell_id_type& marker_position : markers) {
						svg_cell_markers->elements.push_back(spiky_ball_drawer(tw, dss, marker_position, "red"));
					}

					auto svg_body = std::make_unique<latest::svggen::compound>(draw_tobor_background(tw, dss),
					                                                           draw_blocked_cells(tw, dss),
					                                                           draw_tobor_grid(tw, dss),
					                                                           draw_walls(tw, dss),
					                                                           std::move(svg_cell_markers));

					const std::string svg_root_height = std::to_string(dss.cell_height * tw.get_vertical_size() + dss.top_padding + dss.bottom_padding);
					const std::string svg_root_width  = std::to_string(dss.cell_width * tw.get_vertical_size() + dss.left_padding + dss.right_padding);
					auto              svg_root        = std::make_unique<::tobor::latest::svggen::xml_root_element>(svg_root_height, svg_root_width, std::move(svg_body));

					return svg_root->to_string();
				}
			};
		} // namespace svggen
	}    // namespace latest
} // namespace tobor
