#pragma once

#include "all_models.h"

#include "logger.h"

#include <string>
#include <numeric>

#include <fstream>

// tobor colors: 
// red		#f4191c
// yellow	#ffbd02
// green	#00a340
// blue		#4285f4

namespace tobor {

	namespace v1_0 {

		namespace svg {

			class svg_generator {
			public:
				virtual std::string get_svg() const = 0;

				virtual ~svg_generator() {}
			};

			class xml_version final : public svg_generator {
			public:
				virtual std::string get_svg() const override {
					return R"xxx(<?xml version="1.0" standalone="no"?>
)xxx";
				}
			};


			/**
			*
			*/
			class svg_environment : public svg_generator {

				static std::string wrap_svg(const std::string& height, const std::string& width, const std::string& nested_content) {
					return std::string(R"---(<svg height=")---")
						+ height
						+ R"---(" width=")---"
						+ width
						+ R"---(" version="1.1" xmlns="http://www.w3.org/2000/svg">
)---"
+ nested_content
+ "</svg>";
				}

				std::unique_ptr<xml_version> header;
				std::unique_ptr<svg_generator> body;
				std::string height;
				std::string width;

			public:

				svg_environment(const std::string& height, const std::string& width, std::unique_ptr<xml_version> header, std::unique_ptr<svg_generator> body) :
					header(std::move(header)),
					body(std::move(body)),
					height(height),
					width(width)
				{}

				virtual std::string get_svg() const override {
					return header->get_svg() + wrap_svg(height, width, body->get_svg());
				}

			};


			class svg_compound : public svg_generator {

			public:

				using u_ptr = std::unique_ptr<svg_generator>;

				std::vector<u_ptr> elements;

				svg_compound() {}

				template<class... T>
				svg_compound(T&&... init) {
					((void)elements.push_back(std::forward<T>(init)), ...);
				}

				virtual std::string get_svg() const override {
					return std::accumulate(
						elements.cbegin(),
						elements.cend(),
						std::string(),
						[](const std::string& acc, const u_ptr& el) {
							return acc + el->get_svg();
						});
				}
			};

			class svg_primitive : public svg_generator {

				std::string primitive;

			public:
				svg_primitive() {}

				svg_primitive(const std::string& primitive) : primitive(primitive) {}

				inline void set(const std::string& primitive_p) {
					this->primitive = primitive_p;
				}

				virtual std::string get_svg() const override {
					return primitive;
				}
			};


			class svg_path_element {

			public:

				virtual std::string str() const = 0;

				virtual std::shared_ptr<svg_path_element> clone() const = 0;

				virtual ~svg_path_element() {}

			};

			namespace svg_path_elements {


				template <class Number>
				class M : public svg_path_element {

					friend class svg_path;

				public:

					std::vector<std::pair<Number, Number>> coordinates;

					M(const Number& x, const Number& y) : coordinates({ std::make_pair(x,y) })
					{
					}

					M() : M(0, 0) {}

					virtual std::string str() const override {
						return std::accumulate(coordinates.cbegin(), coordinates.cend(), std::string("M"),
							[](const std::string& acc, const std::pair<Number, Number>& el) -> std::string {
								return acc + " " + std::to_string(el.first) + " " + std::to_string(el.second);
							}
						);
					}

					virtual std::shared_ptr<svg_path_element> clone() const override {
						return std::make_shared<M>(*this);
					}

					virtual ~M() override {}
				};

				class Z : public svg_path_element {

				public:

					Z() {}

					virtual std::string str() const override {
						return "Z";
					}

					virtual std::shared_ptr<svg_path_element> clone() const override {
						return std::make_shared<Z>(*this);
					}

					virtual ~Z() override {}
				};

				template <class Number>
				class l : public svg_path_element {

					friend class svg_path;

					std::vector<std::pair<Number, Number>> coordinates;

				public:

					l(const Number& x, const Number& y) : coordinates({ std::make_pair(x,y) })
					{
					}

					inline void add_coordinates() const noexcept {}

					template<class... _Rest>
					void add_coordinates(const Number& x, const Number& y, _Rest&& ... others) {
						coordinates.emplace_back(x, y);
						add_coordinates(std::forward<_Rest>(others) ...);
					}


					template<class... _Rest>
					l(const Number& x, const Number& y, _Rest&& ... others) : coordinates({ std::make_pair(x,y) })
					{
						add_coordinates(std::forward<_Rest>(others) ...);
					}

					virtual std::string str() const override {
						return std::accumulate(coordinates.cbegin(), coordinates.cend(), std::string("l"),
							[](const std::string& acc, const std::pair<Number, Number>& el) -> std::string {
								return acc + " " + std::to_string(el.first) + " " + std::to_string(el.second);
							}
						);
					}

					virtual std::shared_ptr<svg_path_element> clone() const override {
						return std::make_shared<l>(*this);
					}

					virtual ~l() override {}
				};

				template <class Number>
				class c : public svg_path_element {

					friend class svg_path;

					std::vector<std::array<Number, 6>> coordinates;

				public:

					c(const Number& control1_x,
						const Number& control1_y,
						const Number& control2_x,
						const Number& control2_y,
						const Number& destination_x,
						const Number& destination_y) :
						coordinates({ { control1_x, control1_y, control2_x ,control2_y , destination_x, destination_y } })
					{
					}

					inline void add_coordinates() const noexcept {}

					template<class... _Rest>
					void add_coordinates(const Number& control1_x,
						const Number& control1_y,
						const Number& control2_x,
						const Number& control2_y,
						const Number& destination_x,
						const Number& destination_y, _Rest&& ... others) {
						coordinates.emplace_back({ control1_x, control1_y, control2_x ,control2_y , destination_x, destination_y });
						add_coordinates(std::forward<_Rest>(others) ...);
					}


					template<class... _Rest>
					c(const Number& control1_x,
						const Number& control1_y,
						const Number& control2_x,
						const Number& control2_y,
						const Number& destination_x,
						const Number& destination_y, _Rest&& ... others) :
						coordinates({ control1_x, control1_y, control2_x ,control2_y , destination_x, destination_y })
					{
						add_coordinates(std::forward<_Rest>(others) ...);
					}

					virtual std::string str() const override {
						return std::accumulate(coordinates.cbegin(), coordinates.cend(), std::string("c"),
							[](const std::string& acc, const std::array<Number, 6>& el) -> std::string {
								std::string result = acc;
								for (std::size_t i = 0; i < 6; ++i) {
									result += " " + std::to_string(el[i]);
								}
								return result;
							}
						);
					}

					virtual std::shared_ptr<svg_path_element> clone() const override {
						return std::make_shared<c>(*this);
					}

					virtual ~c() override {}
				};

				template <class Number>
				class C : public svg_path_element {

					friend class svg_path;

					std::vector<std::array<Number, 6>> coordinates;

				public:

					C(const Number& control1_x,
						const Number& control1_y,
						const Number& control2_x,
						const Number& control2_y,
						const Number& destination_x,
						const Number& destination_y) :
						coordinates({ { control1_x, control1_y, control2_x ,control2_y , destination_x, destination_y } })
					{
					}

					inline void add_coordinates() const noexcept {}

					template<class... _Rest>
					void add_coordinates(const Number& control1_x,
						const Number& control1_y,
						const Number& control2_x,
						const Number& control2_y,
						const Number& destination_x,
						const Number& destination_y, _Rest&& ... others) {
						coordinates.emplace_back({ control1_x, control1_y, control2_x ,control2_y , destination_x, destination_y });
						add_coordinates(std::forward<_Rest>(others) ...);
					}


					template<class... _Rest>
					C(const Number& control1_x,
						const Number& control1_y,
						const Number& control2_x,
						const Number& control2_y,
						const Number& destination_x,
						const Number& destination_y, _Rest&& ... others) :
						coordinates({ control1_x, control1_y, control2_x ,control2_y , destination_x, destination_y })
					{
						add_coordinates(std::forward<_Rest>(others) ...);
					}

					virtual std::string str() const override {
						return std::accumulate(coordinates.cbegin(), coordinates.cend(), std::string("C"),
							[](const std::string& acc, const std::array<Number, 6>& el) -> std::string {
								std::string result = acc;
								for (std::size_t i = 0; i < 6; ++i) {
									result += " " + std::to_string(el[i]);
								}
								return result;
							}
						);
					}

					virtual std::shared_ptr<svg_path_element> clone() const override {
						return std::make_shared<C>(*this);
					}

					virtual ~C() override {}
				};

				template <class Number>
				class a : public svg_path_element {

					friend class svg_path;

				public:
					struct step {
						double rx;
						double ry;
						double x_axis_rotation;
						bool large_arc_flag;
						bool sweep_flag;
						double x;
						double y;

						std::string str() const noexcept {
							return std::to_string(rx) + " " +
								std::to_string(ry) + " " +
								std::to_string(x_axis_rotation) + " " +
								std::to_string(static_cast<int>(large_arc_flag)) + " " +
								std::to_string(static_cast<int>(sweep_flag)) + " " +
								std::to_string(x) + " " +
								std::to_string(y);
						}

					public:
						step(double rx, double ry, double x_axis_rotation, bool large_arc_flag, bool sweep_flag, double x, double y) :
							rx(rx),
							ry(ry),
							x_axis_rotation(x_axis_rotation),
							large_arc_flag(large_arc_flag),
							sweep_flag(sweep_flag),
							x(x),
							y(y)
						{
						}
					};

				private:

					std::vector<step> coordinates;

				public:

					a(const step& s) : coordinates({ s }) {}

					inline void add_coordinates() const noexcept {}

					template<class... _Rest>
					void add_coordinates(const step& step, _Rest&& ... others) {
						coordinates.push_back(step);
						add_coordinates(std::forward<_Rest>(others) ...);
					}


					template<class... _Rest>
					a(const step& s, _Rest&& ... others) :
						coordinates({ s })
					{
						add_coordinates(std::forward<_Rest>(others) ...);
					}

					virtual std::string str() const override {
						return std::accumulate(coordinates.cbegin(), coordinates.cend(), std::string("a"),
							[](const std::string& acc, const step& el) -> std::string {
								return acc + " " + el.str();
							}
						);
					}

					virtual std::shared_ptr<svg_path_element> clone() const override {
						return std::make_shared<a>(*this);
					}

					virtual ~a() override {}
				};

			}


			class svg_path : public svg_generator {

			public:

				using map = std::map<std::string, std::string>;

				map other_properties;

				std::vector<std::shared_ptr<svg_path_element>> path_elements;

				svg_path() {}

				svg_path(const svg_path& other) : other_properties(other.other_properties) {
					std::transform(
						other.path_elements.cbegin(),
						other.path_elements.cend(),
						std::back_inserter(this->path_elements),
						[](const std::shared_ptr<svg_path_element>& el) {
							return el->clone();
						}
					);
				}

				svg_path(svg_path&& other) = delete;

				svg_path& operator = (const svg_path& other) = delete;

				svg_path& operator = (svg_path&& other) = delete;


				virtual std::string get_svg() const override {
					auto result = std::string("<path");

					result += std::string(" d=\"") +
						std::accumulate(
							path_elements.cbegin(),
							path_elements.cend(),
							std::string(""),
							[](const std::string& acc, const std::shared_ptr<svg_path_element>& el) -> std::string {
								return acc.empty() ?
									el->str() :
									acc + " " + el->str();
							})
						+ "\"";
							result += std::accumulate(
								other_properties.cbegin(),
								other_properties.cend(),
								std::string(""),
								[](const std::string& acc, const map::value_type& el) -> std::string {
									return acc + " " + el.first + "=" + "\"" + el.second + "\"";
								});
							result += std::string("/>");
							return result;
				}

				std::string& fill() {
					return other_properties["fill"];
				}
				std::string& stroke() {
					return other_properties["stroke"];
				}
				std::string& stroke_width() {
					return other_properties["stroke-width"];
				}
				std::string& stroke_linecap() {
					return other_properties["stroke-linecap"];
				}
				std::string& stroke_linejoin() {
					return other_properties["stroke-linejoin"];
				}
			};

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

		template<class ... T>
		inline std::unique_ptr<svg::svg_generator> draw_tobor_background(const tobor::v1_0::tobor_world<T...>& tobor_world, const drawing_style_sheet& dss) {
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

		template<class ... T>
		inline std::unique_ptr<svg::svg_path> get_vertical_grid_element(const tobor::v1_0::tobor_world<T...>& tobor_world, const drawing_style_sheet& dss, std::size_t cell_count_offset) {
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

		template<class ... T>
		inline std::unique_ptr<svg::svg_path> get_horizontal_grid_element(const tobor::v1_0::tobor_world<T...>& tobor_world, const drawing_style_sheet& dss, std::size_t cell_count_offset) {
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

		template<class ... T>
		inline std::unique_ptr<svg::svg_generator> draw_tobor_grid(const tobor::v1_0::tobor_world<T...>& tobor_world, const drawing_style_sheet& dss) {
			auto svg_grid = std::make_unique<svg::svg_compound>();

			for (std::size_t i{ 0 }; i <= tobor_world.get_horizontal_size(); ++i) { // draw vertical grid lines
				svg_grid->elements.push_back(get_vertical_grid_element(tobor_world, dss, i));
			}

			for (std::size_t i{ 0 }; i <= tobor_world.get_vertical_size(); ++i) { // draw horizontal grid lines
				svg_grid->elements.push_back(get_horizontal_grid_element(tobor_world, dss, i));
			}

			return svg_grid;
		}

		template<class ... T>
		inline std::unique_ptr<svg::svg_path> get_horizontal_south_wall(const tobor::v1_0::tobor_world<T...>& tobor_world, const drawing_style_sheet& dss, std::size_t x, std::size_t y) {
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

		template<class ...T>
		inline std::unique_ptr<svg::svg_path> get_vertical_west_wall(const tobor::v1_0::tobor_world<T...>& tobor_world, const drawing_style_sheet& dss, std::size_t x, std::size_t y) {
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

		template<class cell_id_type, class ...T>
		inline std::unique_ptr<svg::svg_path> fill_whole_cell(
			const tobor::v1_0::tobor_world<T...>& tobor_world,
			const drawing_style_sheet& dss,
			const cell_id_type& cell_id,
			const std::string& color
		) {
			auto whole_cell = std::make_unique<svg::svg_path>();

			whole_cell->fill() = color;
			whole_cell->stroke() = whole_cell->fill();
			whole_cell->stroke_width() = "0";

			auto start_at_north_west = std::make_shared<svg::svg_path_elements::M<double>>(
				dss.LEFT_PADDING + dss.CELL_WIDTH * cell_id.get_x_coord(),
				dss.TOP_PADDING + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - cell_id.get_y_coord() - 1)
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

		template<class ...T>
		inline std::unique_ptr<svg::svg_generator> draw_blocked_cells(
			const tobor::v1_0::tobor_world<T...>& tobor_world,
			const drawing_style_sheet& dss
		) {
			using cell_id_type = tobor::v1_0::universal_cell_id<tobor::v1_0::tobor_world<T...>>;
			using cell_id_int_type = typename cell_id_type::int_type;

			auto blocked_cells = std::make_unique<svg::svg_compound>();

			for (cell_id_int_type cell_id{ 0 }; cell_id < tobor_world.count_cells(); ++cell_id) {
				const auto universal_cell_id = cell_id_type::create_by_id(cell_id, tobor_world);

				if (
					tobor_world.west_wall_by_id(cell_id) &&
					tobor_world.east_wall_by_id(cell_id) &&
					tobor_world.south_wall_by_transposed_id(universal_cell_id.get_transposed_id()) &&
					tobor_world.north_wall_by_transposed_id(universal_cell_id.get_transposed_id())
					) {

					auto block = fill_whole_cell(
						tobor_world,
						dss,
						universal_cell_id,
						"black"
					);

					blocked_cells->elements.push_back(std::move(block));

				}
			}

			return blocked_cells;
		}

		template<class ...T>
		inline std::unique_ptr<svg::svg_generator> draw_walls(const tobor::v1_0::tobor_world<T...>& tobor_world, const drawing_style_sheet& dss) {
			auto svg_walls = std::make_unique<svg::svg_compound>();

			using world_type = tobor::v1_0::tobor_world<T...>;
			using cell_id_type = tobor::v1_0::universal_cell_id<world_type>;
			using int_type = typename cell_id_type::int_type;

			// horizontal walls:
			for (int_type x = 0; x < tobor_world.get_horizontal_size(); ++x) {
				for (int_type y = 0; y <= tobor_world.get_vertical_size(); ++y) {
					if (
						tobor_world.south_wall_by_transposed_id(tobor::v1_0::universal_cell_id< tobor::v1_0::tobor_world<T...>>::create_by_coordinates(x, y, tobor_world).get_transposed_id())
						) {
						// ! Note that y exceeds its natural range by 1. But it is okay, since we operate on an infinite repeating 2D landscape
						svg_walls->elements.push_back(get_horizontal_south_wall(tobor_world, dss, x, y));
					}
				}
			}

			// vertical walls:
			for (int_type x = 0; x <= tobor_world.get_horizontal_size(); ++x) {
				for (int_type y = 0; y < tobor_world.get_vertical_size(); ++y) {
					if (
						tobor_world.west_wall_by_id(tobor::v1_0::universal_cell_id< tobor::v1_0::tobor_world<T...>>::create_by_coordinates(x, y, tobor_world).get_id())
						) {
						// ! Note that y exceeds its natural range by 1. But it is okay, since we operate on an infinite repeating 2D landscape
						svg_walls->elements.push_back(get_vertical_west_wall(tobor_world, dss, x, y));
					}
				}
			}


			return svg_walls;
		}

		template<class WorldType>
		class piece_drawer {
		public:

			using world_type = WorldType;
			// assert that it is indeed a tobor::v1_0::tobor_world<...> ?

			virtual std::unique_ptr<svg::svg_generator> operator()(
				const world_type& tobor_world,
				const drawing_style_sheet& dss,
				const tobor::v1_0::universal_cell_id<world_type>& cell,
				const std::string& color) = 0;

			virtual ~piece_drawer() {}
		};


		template<class WorldType>
		class duck_piece_drawer : public piece_drawer<WorldType> {
		public:

			using world_type = WorldType;

			using base_type = piece_drawer<world_type>;

		private:

			bool look_right{ false };

		public:
			virtual std::unique_ptr<svg::svg_generator> operator()(
				const world_type& tobor_world,
				const drawing_style_sheet& dss,
				const tobor::v1_0::universal_cell_id<world_type>& cell,
				const std::string& color
				) override {

				(void)look_right;

				auto duck = std::make_unique<svg::svg_path >();

				duck->fill() = color;
				duck->stroke() = "black";
				duck->stroke_width() = std::to_string(dss.PIECE_LINE_WIDTH);

				const auto CELL_CORNER_SOUTH_WEST_x = dss.LEFT_PADDING + dss.CELL_WIDTH * cell.get_x_coord();
				const auto CELL_CORNER_SOUTH_WEST_y = dss.TOP_PADDING + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - cell.get_y_coord());

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

		private:

			double param{ 0.2 };

			inline double weighted_sum(double from, double to) {
				return (1.0 - param) * from + param * to;
			}

		public:
			virtual std::unique_ptr<svg::svg_generator> operator()(
				const world_type& tobor_world,
				const drawing_style_sheet& dss,
				const tobor::v1_0::universal_cell_id<world_type>& cell,
				const std::string& color
				) override {

				auto marker = std::make_unique<svg::svg_path>();

				marker->fill() = color;
				marker->stroke() = "black";
				marker->stroke_width() = std::to_string(dss.PIECE_LINE_WIDTH);

				const auto CELL_CORNER_SOUTH_WEST_x = dss.LEFT_PADDING + dss.CELL_WIDTH * cell.get_x_coord();
				const auto CELL_CORNER_SOUTH_WEST_y = dss.TOP_PADDING + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - cell.get_y_coord());

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

		template<class Positions_Of_Pieces_Type_T>
		class tobor_graphics {
		public:

			using positions_of_pieces_type = Positions_Of_Pieces_Type_T;

			using cell_id_type = typename positions_of_pieces_type::cell_id_type;

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

			static_assert(pieces_quantity_type::COUNT_TARGET_PIECES == 1, "Not yet supported: multiple target pieces");

			template<class ... T>
			inline static std::string draw_tobor_world(
				const tobor::v1_0::tobor_world<T...>& tw,
				const positions_of_pieces_type& pop,
				const cell_id_type& target_cell,
				const coloring& c,
				piece_shape_selection shape
			) {
				std::unique_ptr<piece_drawer<tobor::v1_0::tobor_world<T...>>> piece_drawer;
				if (shape == piece_shape_selection::BALL) {
					piece_drawer = std::make_unique<ball_piece_drawer<tobor::v1_0::tobor_world<T...>>>();
				}
				else if (shape == piece_shape_selection::DUCK) {
					piece_drawer = std::make_unique<duck_piece_drawer<tobor::v1_0::tobor_world<T...>>>();
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

						(*piece_drawer)(tw, dss, pop.piece_positions[pid], c.colors[pid])

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
				const tobor::v1_0::tobor_world<T...>& tw,
				const std::vector<cell_id_type>& markers
				/*, coloring*/
			) {
				ball_piece_drawer< tobor::v1_0::tobor_world<T...>> piece_drawer;

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

