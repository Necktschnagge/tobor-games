#pragma once

#include "tobor_svg.h"


namespace tobor {

	namespace v1_1 {

		namespace svg {

			using svg_generator = tobor::v1_0::svg::svg_generator;

			using xml_version = tobor::v1_0::svg::xml_version;

			using svg_environment = tobor::v1_0::svg::svg_environment;

			using svg_compound = tobor::v1_0::svg::svg_compound;

			using svg_primitive = tobor::v1_0::svg::svg_primitive;

			using svg_path_element = tobor::v1_0::svg::svg_path_element;

			namespace svg_path_elements {

				using namespace ::tobor::v1_0::svg::svg_path_elements;
				//template <class Number>
				//using M = tobor::v1_0::svg::svg_path_elements::Number>;
			}

			using svg_path = tobor::v1_0::svg::svg_path;

		}


		struct drawing_style_sheet {
			double HALF_GRID_LINE_WIDTH{ 1.5 };

			double HALF_WALL_LINE_WIDTH{ 5 };
			double WALL_CORNER_STRETCH{ 0.6 };


			double PIECE_LINE_WIDTH{ 0.3 * HALF_WALL_LINE_WIDTH };

			double VERTICAL_PIECE_PADDING{ HALF_WALL_LINE_WIDTH * 3 };
			double HORIZONTAL_PIECE_PADDING{ HALF_WALL_LINE_WIDTH * 3 };

			double PIECE_FOOT_THICKNESS_FACTOR{ 0.15 };

			double PIECE_HEAD_X_RADIUS_FACTOR{ 0.3 };
			double PIECE_HEAD_Y_RADIUS_FACTOR{ 0.2 };

			double PIECE_HEAD_AXIS_ROTATION{ -50 };

			// each cell is 100 x 100
			// on each side we define a padding of 50. -> canvas is 1700 ^2
			double CELL_HEIGHT{ 100.0 };
			double CELL_WIDTH{ CELL_HEIGHT };
			double LEFT_PADDING{ 0.5 * CELL_HEIGHT };
			double RIGHT_PADDING{ 0.5 * CELL_HEIGHT };
			double TOP_PADDING{ 0.5 * CELL_HEIGHT };
			double BOTTOM_PADDING{ 0.5 * CELL_HEIGHT };

		};



		template<class World_Type>
		class piece_drawer {
		public:

			using world_type = World_Type;

			using cell_id_type = tobor::v1_1::min_size_cell_id<world_type>;


			virtual std::unique_ptr<svg::svg_generator> operator()(
				const world_type& tobor_world,
				const drawing_style_sheet& dss,
				const cell_id_type& cell,
				const std::string& color) = 0;

			virtual ~piece_drawer() {}
		};


		template<class World_Type>
		class duck_piece_drawer : public piece_drawer<World_Type> {
		public:

			using world_type = World_Type;

			using base_type = piece_drawer<world_type>;

			using cell_id_type = typename base_type::cell_id_type;

		private:

			bool look_right{ false };

		public:
			virtual std::unique_ptr<svg::svg_generator> operator()(
				const world_type& tobor_world,
				const drawing_style_sheet& dss,
				const cell_id_type& cell,
				const std::string& color
				) override {

				(void)look_right;

				auto duck = std::make_unique<svg::svg_path >();

				duck->fill() = color;
				duck->stroke() = "black";
				duck->stroke_width() = std::to_string(dss.PIECE_LINE_WIDTH);

				const auto CELL_CORNER_SOUTH_WEST_x = dss.LEFT_PADDING + dss.CELL_WIDTH * cell.get_x_coord(tobor_world);
				const auto CELL_CORNER_SOUTH_WEST_y = dss.TOP_PADDING + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - cell.get_y_coord(tobor_world));

				const auto CANVAS_X_SIZE = dss.CELL_WIDTH - 2 * dss.HORIZONTAL_PIECE_PADDING;
				const auto CANVAS_Y_SIZE = dss.CELL_HEIGHT - 2 * dss.VERTICAL_PIECE_PADDING;

				const auto MINIMUM_CANVAS_SIZE = std::min(CANVAS_X_SIZE, CANVAS_Y_SIZE);

				const auto FOOT_THICKNESS_RIGHT = CANVAS_Y_SIZE * dss.PIECE_FOOT_THICKNESS_FACTOR;

				auto start_at_south_west = std::make_shared<svg::svg_path_elements::M<double>>(
					CELL_CORNER_SOUTH_WEST_x + dss.HORIZONTAL_PIECE_PADDING,
					CELL_CORNER_SOUTH_WEST_y - dss.VERTICAL_PIECE_PADDING
				);

				auto go_bottom_east = std::make_shared<svg::svg_path_elements::l<double>>(
					dss.CELL_WIDTH - 2 * dss.HORIZONTAL_PIECE_PADDING, 0
				);

				auto go_bottom_right_corner_north = std::make_shared<svg::svg_path_elements::l<double>>(
					0, -FOOT_THICKNESS_RIGHT
				);

				auto go_back_neck_bottom_up = std::make_shared<svg::svg_path_elements::c<double>>(
					-0.5 * CANVAS_X_SIZE, 0,
					-0.5 * CANVAS_X_SIZE, 0,
					-0.4 * CANVAS_X_SIZE, -(0.45 * CANVAS_Y_SIZE - FOOT_THICKNESS_RIGHT)
				);

				auto go_head = std::make_shared<svg::svg_path_elements::a<double>>(
					svg::svg_path_elements::a<double>::step(dss.PIECE_HEAD_X_RADIUS_FACTOR * MINIMUM_CANVAS_SIZE, dss.PIECE_HEAD_Y_RADIUS_FACTOR * MINIMUM_CANVAS_SIZE, dss.PIECE_HEAD_AXIS_ROTATION, true, false, -0.2 * MINIMUM_CANVAS_SIZE, -0.25 * MINIMUM_CANVAS_SIZE)
				);

				auto go_nort_west_for_beak = std::make_shared<svg::svg_path_elements::l<double>>(
					-0.33 * MINIMUM_CANVAS_SIZE, -0.15 * MINIMUM_CANVAS_SIZE
				);

				auto go_south_east_for_beak_down = std::make_shared<svg::svg_path_elements::l<double>>(
					0.32 * MINIMUM_CANVAS_SIZE, 0.25 * MINIMUM_CANVAS_SIZE
				);

				auto go_back = std::make_shared<svg::svg_path_elements::Z>();

				duck->path_elements = { start_at_south_west, go_bottom_east, go_bottom_right_corner_north, go_back_neck_bottom_up, go_head , go_nort_west_for_beak, go_south_east_for_beak_down, go_back };

				auto outer_eye = std::make_unique<svg::svg_path >();

				outer_eye->fill() = "white";
				outer_eye->stroke() = "black";
				outer_eye->stroke_width() = std::to_string(dss.PIECE_LINE_WIDTH);


				auto start_eye = std::make_shared<svg::svg_path_elements::M<double>>(
					CELL_CORNER_SOUTH_WEST_x + dss.HORIZONTAL_PIECE_PADDING + 0.66 * MINIMUM_CANVAS_SIZE,
					CELL_CORNER_SOUTH_WEST_y - (dss.VERTICAL_PIECE_PADDING + 0.77 * MINIMUM_CANVAS_SIZE)
				);

				outer_eye->path_elements.push_back(start_eye);

				outer_eye->path_elements.push_back(
					std::make_shared<svg::svg_path_elements::a<double>>(
						svg::svg_path_elements::a<double>::step(
							0.04 * MINIMUM_CANVAS_SIZE,
							0.03 * MINIMUM_CANVAS_SIZE,
							-10.0,
							true,
							false,
							0.2,
							0.2
						)
					)
				);
				outer_eye->path_elements.push_back(go_back);


				auto comp = std::make_unique<svg::svg_compound>(std::move(duck), std::move(outer_eye));

				return comp;

			}

			virtual ~duck_piece_drawer() {}
		};

		template<class WorldType>
		class ball_piece_drawer : public piece_drawer<WorldType> {
		public:

			using world_type = WorldType;

			using base_type = piece_drawer<world_type>;

			using cell_id_type = typename base_type::cell_id_type;


		private:

			double param{ 0.2 };

			inline double weighted_sum(double from, double to) {
				return (1.0 - param) * from + param * to;
			}

		public:
			virtual std::unique_ptr<svg::svg_generator> operator()(
				const world_type& tobor_world,
				const drawing_style_sheet& dss,
				const cell_id_type& cell,
				const std::string& color
				) override {

				auto marker = std::make_unique<svg::svg_path>();

				marker->fill() = color;
				marker->stroke() = "black";
				marker->stroke_width() = std::to_string(dss.PIECE_LINE_WIDTH);

				const auto CELL_CORNER_SOUTH_WEST_x = dss.LEFT_PADDING + dss.CELL_WIDTH * cell.get_x_coord(tobor_world);
				const auto CELL_CORNER_SOUTH_WEST_y = dss.TOP_PADDING + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - cell.get_y_coord(tobor_world));

				const auto CANVAS_X_SIZE = dss.CELL_WIDTH - 2 * dss.HORIZONTAL_PIECE_PADDING;
				const auto CANVAS_Y_SIZE = dss.CELL_HEIGHT - 2 * dss.VERTICAL_PIECE_PADDING;

				const auto HALF_CANVAS_X_SIZE = CANVAS_X_SIZE / 2;
				const auto HALF_CANVAS_Y_SIZE = CANVAS_Y_SIZE / 2;

				//const auto MINIMUM_CANVAS_SIZE = std::min(CANVAS_X_SIZE, CANVAS_Y_SIZE);

				//const auto FOOT_THICKNESS_RIGHT = CANVAS_Y_SIZE * dss.PIECE_FOOT_THICKNESS_FACTOR;


				const auto CENTER_X{ CELL_CORNER_SOUTH_WEST_x + dss.CELL_WIDTH / 2 };
				const auto CENTER_Y{ CELL_CORNER_SOUTH_WEST_y - dss.CELL_HEIGHT / 2 };

				//const auto TOP_Y{ dss.TOP_PADDING + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - cell.get_y_coord() - 1) + dss.VERTICAL_PIECE_PADDING };
				//const auto BOTTOM_Y{ CELL_CORNER_SOUTH_WEST_y - dss.VERTICAL_PIECE_PADDING };

				const double SINCOS45{ 0.7071 };

				const auto COORD_DISTANCE_CORNER{ (CANVAS_X_SIZE / 2) * SINCOS45 };

				marker->path_elements.push_back(
					std::make_shared<svg::svg_path_elements::M<double>>(
						CENTER_X,
						CENTER_Y + HALF_CANVAS_Y_SIZE
					)
				);

				// remove unused consts in this function!

				marker->path_elements.push_back(
					std::make_shared<svg::svg_path_elements::C<double>>(
						weighted_sum(CENTER_X, CENTER_X), weighted_sum(CENTER_Y + HALF_CANVAS_Y_SIZE, CENTER_Y),
						weighted_sum(CENTER_X + COORD_DISTANCE_CORNER, CENTER_X), weighted_sum(CENTER_Y + COORD_DISTANCE_CORNER, CENTER_Y),
						CENTER_X + COORD_DISTANCE_CORNER, CENTER_Y + COORD_DISTANCE_CORNER
					)
				);
				marker->path_elements.push_back(
					std::make_shared<svg::svg_path_elements::C<double>>(
						weighted_sum(CENTER_X + COORD_DISTANCE_CORNER, CENTER_X), weighted_sum(CENTER_Y + COORD_DISTANCE_CORNER, CENTER_Y),
						weighted_sum(CENTER_X + HALF_CANVAS_X_SIZE, CENTER_X), weighted_sum(CENTER_Y, CENTER_Y),
						CENTER_X + HALF_CANVAS_X_SIZE, CENTER_Y
					)
				);
				marker->path_elements.push_back(
					std::make_shared<svg::svg_path_elements::C<double>>(
						weighted_sum(CENTER_X + HALF_CANVAS_X_SIZE, CENTER_X), weighted_sum(CENTER_Y, CENTER_Y),
						weighted_sum(CENTER_X + COORD_DISTANCE_CORNER, CENTER_X), weighted_sum(CENTER_Y - COORD_DISTANCE_CORNER, CENTER_Y),
						CENTER_X + COORD_DISTANCE_CORNER, CENTER_Y - COORD_DISTANCE_CORNER
					)
				);
				marker->path_elements.push_back(
					std::make_shared<svg::svg_path_elements::C<double>>(
						weighted_sum(CENTER_X + COORD_DISTANCE_CORNER, CENTER_X), weighted_sum(CENTER_Y - COORD_DISTANCE_CORNER, CENTER_Y),
						weighted_sum(CENTER_X, CENTER_X), weighted_sum(CENTER_Y - HALF_CANVAS_Y_SIZE, CENTER_Y),
						CENTER_X, CENTER_Y - HALF_CANVAS_Y_SIZE
					)
				);
				marker->path_elements.push_back(
					std::make_shared<svg::svg_path_elements::C<double>>(
						weighted_sum(CENTER_X, CENTER_X), weighted_sum(CENTER_Y - HALF_CANVAS_Y_SIZE, CENTER_Y),
						weighted_sum(CENTER_X - COORD_DISTANCE_CORNER, CENTER_X), weighted_sum(CENTER_Y - COORD_DISTANCE_CORNER, CENTER_Y),
						CENTER_X - COORD_DISTANCE_CORNER, CENTER_Y - COORD_DISTANCE_CORNER
					)
				);
				marker->path_elements.push_back(
					std::make_shared<svg::svg_path_elements::C<double>>(
						weighted_sum(CENTER_X - COORD_DISTANCE_CORNER, CENTER_X), weighted_sum(CENTER_Y - COORD_DISTANCE_CORNER, CENTER_Y),
						weighted_sum(CENTER_X - HALF_CANVAS_X_SIZE, CENTER_X), weighted_sum(CENTER_Y, CENTER_Y),
						CENTER_X - HALF_CANVAS_X_SIZE, CENTER_Y
					)
				);
				marker->path_elements.push_back(
					std::make_shared<svg::svg_path_elements::C<double>>(
						weighted_sum(CENTER_X - HALF_CANVAS_X_SIZE, CENTER_X), weighted_sum(CENTER_Y, CENTER_Y),
						weighted_sum(CENTER_X - COORD_DISTANCE_CORNER, CENTER_X), weighted_sum(CENTER_Y + COORD_DISTANCE_CORNER, CENTER_Y),
						CENTER_X - COORD_DISTANCE_CORNER, CENTER_Y + COORD_DISTANCE_CORNER
					)
				);
				marker->path_elements.push_back(
					std::make_shared<svg::svg_path_elements::C<double>>(
						weighted_sum(CENTER_X - COORD_DISTANCE_CORNER, CENTER_X), weighted_sum(CENTER_Y + COORD_DISTANCE_CORNER, CENTER_Y),
						weighted_sum(CENTER_X, CENTER_X), weighted_sum(CENTER_Y + HALF_CANVAS_Y_SIZE, CENTER_Y),
						CENTER_X, CENTER_Y + HALF_CANVAS_Y_SIZE
					)
				);

				marker->path_elements.push_back(std::make_shared<svg::svg_path_elements::Z>());

				return marker;

			}


			virtual ~ball_piece_drawer() {}
		};

		template<class World_Type, class Positions_Of_Pieces_Type_T>
		class tobor_graphics {
		public:

			using world_type = World_Type;

			using positions_of_pieces_type = Positions_Of_Pieces_Type_T;


			using cell_id_type = typename positions_of_pieces_type::cell_id_type;

			using cell_size_int = typename cell_id_type::int_cell_id_type;

			using cell_narrow_int = typename cell_id_type::int_cell_id_type;

			using pieces_quantity_type = typename positions_of_pieces_type::pieces_quantity_type;

			using pq_size_type = typename pieces_quantity_type::int_type;

			static constexpr pq_size_type COUNT_ALL_PIECES{ pieces_quantity_type::COUNT_ALL_PIECES };

			struct coloring {
				using array_type = std::array<std::string, COUNT_ALL_PIECES>;

				array_type colors;

				template<class ... T>
				coloring(T&& ... args) :
					colors{ std::forward<T>(args) ... } {
				}
			};

			enum class piece_shape_selection {
				BALL,
				DUCK
			};

		private:

			inline static std::unique_ptr<svg::svg_generator> draw_tobor_background(const world_type& tobor_world, const drawing_style_sheet& dss) {
				auto svg_background = std::make_unique<svg::svg_path>();
				svg_background->fill() = "lightyellow";
				svg_background->stroke_width() = "0";
				svg_background->stroke() = svg_background->fill();

				const double horizontal_size = dss.LEFT_PADDING + dss.CELL_WIDTH * tobor_world.get_horizontal_size() + dss.RIGHT_PADDING;
				const double vertical_size = dss.TOP_PADDING + dss.CELL_HEIGHT * tobor_world.get_vertical_size() + dss.BOTTOM_PADDING;

				auto start_at_0_0 = std::make_shared<svg::svg_path_elements::M<double>>(0, 0);
				auto go_to_right_bottom_left = std::make_shared<svg::svg_path_elements::l<double>>(horizontal_size, 0, 0, vertical_size, 0 - horizontal_size, 0);
				auto go_back = std::make_shared<svg::svg_path_elements::Z>();

				svg_background->path_elements.push_back(start_at_0_0);
				svg_background->path_elements.push_back(go_to_right_bottom_left);
				svg_background->path_elements.push_back(go_back);

				return svg_background;
			}

			inline static std::unique_ptr<svg::svg_path> get_vertical_grid_element(const world_type& tobor_world, const drawing_style_sheet& dss, std::size_t cell_count_offset) {
				auto grid_element = std::make_unique<svg::svg_path>();

				grid_element->fill() = "dimgrey"; // !80 #0F54DA";
				grid_element->stroke() = grid_element->fill();
				grid_element->stroke_width() = "0";

				auto start_at_upper_left = std::make_shared<svg::svg_path_elements::M<double>>(
					dss.LEFT_PADDING - dss.HALF_GRID_LINE_WIDTH + dss.CELL_WIDTH * cell_count_offset,
					dss.TOP_PADDING - dss.HALF_GRID_LINE_WIDTH
				);
				auto go_to_right_bottom_left = std::make_shared<svg::svg_path_elements::l<double>>(
					dss.HALF_GRID_LINE_WIDTH * 2, 0,
					0, dss.CELL_HEIGHT * tobor_world.get_vertical_size() + dss.HALF_GRID_LINE_WIDTH * 2,
					-dss.HALF_GRID_LINE_WIDTH * 2, 0
				);
				auto go_back = std::make_shared<svg::svg_path_elements::Z>();

				grid_element->path_elements = { start_at_upper_left, go_to_right_bottom_left, go_back };

				return grid_element;
			}

			inline static std::unique_ptr<svg::svg_path> get_horizontal_grid_element(const world_type& tobor_world, const drawing_style_sheet& dss, std::size_t cell_count_offset) {
				auto grid_element = std::make_unique<svg::svg_path>();

				grid_element->fill() = "dimgrey"; // !80 #0F54DA";
				grid_element->stroke() = grid_element->fill();
				grid_element->stroke_width() = "0";

				auto start_at_upper_left = std::make_shared<svg::svg_path_elements::M<double>>(
					dss.LEFT_PADDING - dss.HALF_GRID_LINE_WIDTH,
					dss.TOP_PADDING - dss.HALF_GRID_LINE_WIDTH + dss.CELL_HEIGHT * cell_count_offset
				);
				auto go_to_bottom_right_top = std::make_shared<svg::svg_path_elements::l<double>>(
					0, dss.HALF_GRID_LINE_WIDTH * 2,
					dss.CELL_WIDTH * tobor_world.get_vertical_size() + dss.HALF_GRID_LINE_WIDTH * 2, 0,
					0, -dss.HALF_GRID_LINE_WIDTH * 2
				);
				auto go_back = std::make_shared<svg::svg_path_elements::Z>();

				grid_element->path_elements = { start_at_upper_left, go_to_bottom_right_top, go_back };

				return grid_element;
			}

			inline static std::unique_ptr<svg::svg_generator> draw_tobor_grid(const world_type& tobor_world, const drawing_style_sheet& dss) {
				auto svg_grid = std::make_unique<svg::svg_compound>();

				for (std::size_t i{ 0 }; i <= tobor_world.get_horizontal_size(); ++i) { // draw vertical grid lines
					svg_grid->elements.push_back(get_vertical_grid_element(tobor_world, dss, i));
				}

				for (std::size_t i{ 0 }; i <= tobor_world.get_vertical_size(); ++i) { // draw horizontal grid lines
					svg_grid->elements.push_back(get_horizontal_grid_element(tobor_world, dss, i));
				}

				return svg_grid;
			}

			inline static std::unique_ptr<svg::svg_path> get_horizontal_south_wall(const world_type& tobor_world, const drawing_style_sheet& dss, std::size_t x, std::size_t y) {
				auto horizontal_wall_element = std::make_unique<svg::svg_path>();

				horizontal_wall_element->fill() = "black"; // !80 #0F54DA";
				horizontal_wall_element->stroke() = horizontal_wall_element->fill();
				horizontal_wall_element->stroke_width() = "0";

				auto start_at_upper_left = std::make_shared<svg::svg_path_elements::M<double>>(
					dss.LEFT_PADDING + dss.CELL_WIDTH * x,
					dss.TOP_PADDING - dss.HALF_WALL_LINE_WIDTH + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - y)
				);

				auto left_semicircle = std::make_shared<svg::svg_path_elements::c<double>>(
					-dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH, 0,
					-dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH, 2 * dss.HALF_WALL_LINE_WIDTH,
					0, 2 * dss.HALF_WALL_LINE_WIDTH
				);

				auto go_right = std::make_shared<svg::svg_path_elements::l<double>>(
					dss.CELL_WIDTH, 0
				);

				auto right_semicircle = std::make_shared<svg::svg_path_elements::c<double>>(
					dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH, 0,
					dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH, -2 * dss.HALF_WALL_LINE_WIDTH,
					0, -2 * dss.HALF_WALL_LINE_WIDTH
				);

				auto go_back = std::make_shared<svg::svg_path_elements::Z>();

				horizontal_wall_element->path_elements = { start_at_upper_left, left_semicircle, go_right, right_semicircle, go_back };

				return horizontal_wall_element;
			}

			inline static std::unique_ptr<svg::svg_path> get_vertical_west_wall(const world_type& tobor_world, const drawing_style_sheet& dss, std::size_t x, std::size_t y) {
				auto vertical_wall_element = std::make_unique<svg::svg_path>();

				vertical_wall_element->fill() = "black"; // !80 #0F54DA";
				vertical_wall_element->stroke() = vertical_wall_element->fill();
				vertical_wall_element->stroke_width() = "0";

				auto start_at_lower_left = std::make_shared<svg::svg_path_elements::M<double>>(
					dss.LEFT_PADDING - dss.HALF_WALL_LINE_WIDTH + dss.CELL_WIDTH * x,
					dss.TOP_PADDING + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - y)
				);

				auto bottom_semicircle = std::make_shared<svg::svg_path_elements::c<double>>(
					0, dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH,
					2 * dss.HALF_WALL_LINE_WIDTH, dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH,
					2 * dss.HALF_WALL_LINE_WIDTH, 0
				);

				auto go_north = std::make_shared<svg::svg_path_elements::l<double>>(
					0, -dss.CELL_WIDTH
				);

				auto top_semicircle = std::make_shared<svg::svg_path_elements::c<double>>(
					0, -dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH,
					-2 * dss.HALF_WALL_LINE_WIDTH, -dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH,
					-2 * dss.HALF_WALL_LINE_WIDTH, 0
				);

				auto go_back = std::make_shared<svg::svg_path_elements::Z>();

				vertical_wall_element->path_elements = { start_at_lower_left, bottom_semicircle, go_north, top_semicircle, go_back };

				return vertical_wall_element;
			}

			inline static std::unique_ptr<svg::svg_path> fill_whole_cell(
				const world_type& tobor_world,
				const drawing_style_sheet& dss,
				const cell_id_type& cell_id,
				const std::string& color
			) {
				auto whole_cell = std::make_unique<svg::svg_path>();

				whole_cell->fill() = color;
				whole_cell->stroke() = whole_cell->fill();
				whole_cell->stroke_width() = "0";

				auto start_at_north_west = std::make_shared<svg::svg_path_elements::M<double>>(
					dss.LEFT_PADDING + dss.CELL_WIDTH * cell_id.get_x_coord(tobor_world),
					dss.TOP_PADDING + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - cell_id.get_y_coord(tobor_world) - 1)
				);

				auto go_east = std::make_shared<svg::svg_path_elements::l<double>>(
					dss.CELL_WIDTH, 0
				);

				auto go_south = std::make_shared<svg::svg_path_elements::l<double>>(
					0, dss.CELL_HEIGHT
				);

				auto go_west = std::make_shared<svg::svg_path_elements::l<double>>(
					-dss.CELL_WIDTH, 0
				);

				auto go_back = std::make_shared<svg::svg_path_elements::Z>();

				whole_cell->path_elements = { start_at_north_west, go_east, go_south, go_west, go_back };

				return whole_cell;
			}

			inline static std::unique_ptr<svg::svg_generator> draw_blocked_cells(
				const world_type& tobor_world,
				const drawing_style_sheet& dss
			) {

				auto blocked_cells = std::make_unique<svg::svg_compound>();

				for (cell_size_int raw_id{ 0 }; raw_id < tobor_world.count_cells(); ++raw_id) {

					const cell_id_type cell_id = cell_id_type::create_by_id(raw_id, tobor_world);
					const cell_narrow_int nid{ world_type::narrow(raw_id) };
					const cell_narrow_int ntid{ cell_id.get_transposed_id(tobor_world) };

					if (tobor_world.west_wall_by_id(nid) && tobor_world.east_wall_by_id(nid) && tobor_world.south_wall_by_transposed_id(ntid) && tobor_world.north_wall_by_transposed_id(ntid)) {

						auto block = fill_whole_cell(tobor_world, dss, cell_id, "black");
						blocked_cells->elements.push_back(std::move(block));
					}
				}

				return blocked_cells;
			}

			inline static std::unique_ptr<svg::svg_generator> draw_walls(const world_type& tobor_world, const drawing_style_sheet& dss) {
				auto svg_walls = std::make_unique<svg::svg_compound>();

				// horizontal walls:
				for (cell_size_int x = 0; x < tobor_world.get_horizontal_size(); ++x) {
					for (cell_size_int y = 0; y <= tobor_world.get_vertical_size(); ++y) {
						if (tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(world_type::narrow(x), world_type::narrow(y)))) {
							// ! Note that y exceeds its natural range by 1. But it is okay, since we operate on an infinite repeating 2D landscape
							svg_walls->elements.push_back(get_horizontal_south_wall(tobor_world, dss, x, y));
						}
					}
				}

				// vertical walls:
				for (cell_size_int x = 0; x <= tobor_world.get_horizontal_size(); ++x) {
					for (cell_size_int y = 0; y < tobor_world.get_vertical_size(); ++y) {
						if (tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(world_type::narrow(x), world_type::narrow(y)))) {
							// ! Note that y exceeds its natural range by 1. But it is okay, since we operate on an infinite repeating 2D landscape
							svg_walls->elements.push_back(get_vertical_west_wall(tobor_world, dss, x, y));
						}
					}
				}
				return svg_walls;
			}

		public:


			static_assert(pieces_quantity_type::COUNT_TARGET_PIECES == 1, "Not yet supported: multiple target pieces");

			inline static std::string draw_tobor_world(
				const world_type& tw,
				const positions_of_pieces_type& pop,
				const cell_id_type& target_cell,
				const coloring& c,
				piece_shape_selection shape
			) {
				std::unique_ptr<piece_drawer<world_type>> piece_drawer;
				if (shape == piece_shape_selection::BALL) {
					piece_drawer = std::make_unique<ball_piece_drawer<world_type>>();
				}
				else if (shape == piece_shape_selection::DUCK) {
					piece_drawer = std::make_unique<duck_piece_drawer<world_type>>();
				}
				else {
					throw std::invalid_argument("Unknown shape in SVG draw process.");
				}

				drawing_style_sheet dss;

				auto svg_target = fill_whole_cell(tw, dss, target_cell, c.colors[0]);

				auto svg_pieces = std::make_unique<svg::svg_compound>();

				for (pq_size_type pid = 0; pid < pieces_quantity_type::COUNT_ALL_PIECES; ++pid) {
					svg_pieces->elements.push_back(

						//draw_duck_piece(tw, dss, pop.piece_positions[pid], true, c.colors[pid])

						(*piece_drawer)(tw, dss, pop.raw()[pid], c.colors[pid])

					);

				}

				// The following order is final:
				auto svg_body = std::make_unique<svg::svg_compound>(
					draw_tobor_background(tw, dss),
					draw_blocked_cells(tw, dss),
					std::move(svg_target),
					draw_tobor_grid(tw, dss),
					draw_walls(tw, dss),
					std::move(svg_pieces)
				);
				/* ORDER REQUIREMENTS

					background is behind everything else					// otherwise cannot see anything
					grid is behind walls									// walls should cover grid of cause

					grid is in front of target								// otherwise grid would be covered partially
					walls are in front of target							// since walls partially covered otherwise

					target is in front of blocked							// if by error, target is blocked, we still see target, we also indirectly see it is blocked by seeing walls all around

					pieces are in front of tagert							// since piece covered otherwise when in target cell

					background   <---   blocked   <---   target   <---   grid   <---   walls
														   |
														   |

															<---   pieces

					By now there are three variants to place pieces behind/in front of  ...  grid / walls
				*/



				const std::string svg_root_height = std::to_string(dss.CELL_HEIGHT * tw.get_vertical_size() + dss.TOP_PADDING + dss.BOTTOM_PADDING);
				const std::string svg_root_width = std::to_string(dss.CELL_WIDTH * tw.get_vertical_size() + dss.LEFT_PADDING + dss.RIGHT_PADDING);
				auto svg_root = std::make_unique<svg::svg_environment>(svg_root_height, svg_root_width, std::make_unique<svg::xml_version>(), std::move(svg_body));

				return svg_root->get_svg();
			}


			template<class ... T>
			inline static std::string draw_tobor_world_with_cell_markers(
				const tobor::v1_1::dynamic_rectangle_world<T...>& tw,
				const std::vector<cell_id_type>& markers
				/*, coloring*/
			) {
				ball_piece_drawer<tobor::v1_1::dynamic_rectangle_world<T...>> piece_drawer;

				drawing_style_sheet dss;

				auto svg_cell_markers = std::make_unique<svg::svg_compound>();

				for (const cell_id_type& marker_position : markers) {
					svg_cell_markers->elements.push_back(
						piece_drawer(tw, dss, marker_position, "red")
					);
				}

				auto svg_body = std::make_unique<svg::svg_compound>(
					draw_tobor_background(tw, dss),
					draw_blocked_cells(tw, dss),
					draw_tobor_grid(tw, dss),
					draw_walls(tw, dss),
					std::move(svg_cell_markers)
				);

				const std::string svg_root_height = std::to_string(dss.CELL_HEIGHT * tw.get_vertical_size() + dss.TOP_PADDING + dss.BOTTOM_PADDING);
				const std::string svg_root_width = std::to_string(dss.CELL_WIDTH * tw.get_vertical_size() + dss.LEFT_PADDING + dss.RIGHT_PADDING);
				auto svg_root = std::make_unique<svg::svg_environment>(svg_root_height, svg_root_width, std::make_unique<svg::xml_version>(), std::move(svg_body));

				return svg_root->get_svg();
			}

		};
	}
}