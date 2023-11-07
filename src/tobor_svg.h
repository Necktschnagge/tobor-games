#pragma once

#include "models.h"

#include "logger.h"

#include <string>
#include <numeric>

#include <fstream>

// tobor colors: 
// red		#f4191c
// yellow	#ffbd02
// green	#00a340
// blue		#4285f4

class svg_generator {

	static std::string get_xml_header() {
		return R"xxx(<?xml version="1.0" standalone="no"?>
)xxx";
	}

	static std::string wrap_svg(const std::string& height, const std::string& width, const std::string& nested_content) {
		(void)height;
		(void)width;
		(void)nested_content;
		return std::string();
	}
	//< svg width = "6400" height = "1600" version = "1.1" xmlns = "http://www.w3.org/2000/svg" >



public:
	static std::string create_svg() {
		std::string svg_result;

		return svg_result;
	}


	virtual std::string get_svg() const = 0;

	virtual ~svg_generator() {
	}

};

class xml_header : public svg_generator {

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

	std::unique_ptr<xml_header> header;
	std::unique_ptr<svg_generator> body;
	std::string height;
	std::string width;

public:

	svg_environment(const std::string& height, const std::string& width, std::unique_ptr<xml_header> header, std::unique_ptr<svg_generator> body) :
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

	//svg_compound(std::initializer_list<std::unique_ptr<svg_compound>> init) : elements(init) {}

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

public:

	using u_ptr = std::unique_ptr<svg_generator>;

	//std::vector<u_ptr> elements;
	std::string primitive;

	svg_primitive() {}

	//svg_compound(std::initializer_list<std::unique_ptr<svg_compound>> init) : elements(init) {}

	//template<class... T>
	svg_primitive(const std::string& primitive) : primitive(primitive) {}

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
			return std::make_shared< M>(*this);
		}

		virtual ~M() {}
	};

	class Z : public svg_path_element {

	public:

		Z() {}

		virtual std::string str() const override {
			return "Z";
		}

		virtual std::shared_ptr<svg_path_element> clone() const override {
			return std::make_shared< Z>(*this);
		}

		virtual ~Z() {}
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

		virtual ~l() {}
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

		virtual ~c() {}
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

		a(const step& s) :
			coordinates({ s })
		{
		}

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

		virtual ~a() {}
	};

}


class svg_path : public svg_generator {

public:


	//using u_ptr = std::unique_ptr<svg_generator>;

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
					return acc + " " + el->str();
				})
			+ "\"";
				result += std::accumulate(
					other_properties.cbegin(),
					other_properties.cend(),
					std::string(""),
					[](std::string& acc, const map::value_type& el) -> std::string {
						return acc + " " + el.first + "=" + "\"" + el.second + "\"";
					});
				result += std::string("/>");
				return result;
	}

	auto fill() -> decltype(other_properties["fill"]) {
		return other_properties["fill"];
	}
	auto stroke() -> decltype(other_properties["stroke"]) {
		return other_properties["stroke"];
	}
	auto stroke_width() -> decltype(other_properties["stroke-width"]) {
		return other_properties["stroke-width"];
	}
	auto stroke_linecap() -> decltype(other_properties["stroke-linecap"]) {
		return other_properties["stroke-linecap"];
	}
	auto stroke_linejoin() -> decltype(other_properties["stroke-linejoin"]) {
		return other_properties["stroke-linejoin"];
	}
};

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

std::unique_ptr<svg_generator> draw_tobor_background(const tobor::v1_0::tobor_world& tobor_world, const drawing_style_sheet& dss) {
	auto svg_background = std::make_unique<svg_path>();
	svg_background->fill() = "lightyellow";
	svg_background->stroke_width() = "0";
	svg_background->stroke() = svg_background->fill();

	const double horizontal_size = dss.LEFT_PADDING + dss.CELL_WIDTH * tobor_world.get_horizontal_size() + dss.RIGHT_PADDING;
	const double vertical_size = dss.TOP_PADDING + dss.CELL_HEIGHT * tobor_world.get_vertical_size() + dss.BOTTOM_PADDING;

	auto start_at_0_0 = std::make_shared<svg_path_elements::M<double>>(0, 0);
	auto go_to_right_bottom_left = std::make_shared<svg_path_elements::l<double>>(horizontal_size, 0, 0, vertical_size, 0 - horizontal_size, 0);
	auto go_back = std::make_shared<svg_path_elements::Z>();

	svg_background->path_elements.push_back(start_at_0_0);
	svg_background->path_elements.push_back(go_to_right_bottom_left);
	svg_background->path_elements.push_back(go_back);

	return svg_background;
}

std::unique_ptr<svg_path> get_vertical_grid_element(const tobor::v1_0::tobor_world& tobor_world, const drawing_style_sheet& dss, std::size_t cell_count_offset) {
	auto grid_element = std::make_unique<svg_path>();

	grid_element->fill() = "dimgrey"; // !80 #0F54DA";
	grid_element->stroke() = grid_element->fill();
	grid_element->stroke_width() = "0";

	auto start_at_upper_left = std::make_shared<svg_path_elements::M<double>>(
		dss.LEFT_PADDING - dss.HALF_GRID_LINE_WIDTH + dss.CELL_WIDTH * cell_count_offset,
		dss.TOP_PADDING - dss.HALF_GRID_LINE_WIDTH
	);
	auto go_to_right_bottom_left = std::make_shared<svg_path_elements::l<double>>(
		dss.HALF_GRID_LINE_WIDTH * 2, 0,
		0, dss.CELL_HEIGHT * tobor_world.get_vertical_size() + dss.HALF_GRID_LINE_WIDTH * 2,
		-dss.HALF_GRID_LINE_WIDTH * 2, 0
	);
	auto go_back = std::make_shared<svg_path_elements::Z>();

	grid_element->path_elements = { start_at_upper_left, go_to_right_bottom_left, go_back };

	return grid_element;
}

std::unique_ptr<svg_path> get_horizontal_grid_element(const tobor::v1_0::tobor_world& tobor_world, const drawing_style_sheet& dss, std::size_t cell_count_offset) {
	auto grid_element = std::make_unique<svg_path>();

	grid_element->fill() = "dimgrey"; // !80 #0F54DA";
	grid_element->stroke() = grid_element->fill();
	grid_element->stroke_width() = "0";

	auto start_at_upper_left = std::make_shared<svg_path_elements::M<double>>(
		dss.LEFT_PADDING - dss.HALF_GRID_LINE_WIDTH,
		dss.TOP_PADDING - dss.HALF_GRID_LINE_WIDTH + dss.CELL_HEIGHT * cell_count_offset
	);
	auto go_to_bottom_right_top = std::make_shared<svg_path_elements::l<double>>(
		0, dss.HALF_GRID_LINE_WIDTH * 2,
		dss.CELL_WIDTH * tobor_world.get_vertical_size() + dss.HALF_GRID_LINE_WIDTH * 2, 0,
		0, -dss.HALF_GRID_LINE_WIDTH * 2
	);
	auto go_back = std::make_shared<svg_path_elements::Z>();

	grid_element->path_elements = { start_at_upper_left, go_to_bottom_right_top, go_back };

	return grid_element;
}

std::unique_ptr<svg_generator> draw_tobor_grid(const tobor::v1_0::tobor_world& tobor_world, const drawing_style_sheet& dss) {
	auto svg_grid = std::make_unique<svg_compound>();

	for (std::size_t i{ 0 }; i <= tobor_world.get_horizontal_size(); ++i) { // draw vertical grid lines
		svg_grid->elements.push_back(get_vertical_grid_element(tobor_world, dss, i));
	}

	for (std::size_t i{ 0 }; i <= tobor_world.get_vertical_size(); ++i) { // draw horizontal grid lines
		svg_grid->elements.push_back(get_horizontal_grid_element(tobor_world, dss, i));
	}

	return svg_grid;
}

std::unique_ptr<svg_path> get_horizontal_south_wall(const tobor::v1_0::tobor_world& tobor_world, const drawing_style_sheet& dss, std::size_t x, std::size_t y) {
	auto horizontal_wall_element = std::make_unique<svg_path>();

	horizontal_wall_element->fill() = "black"; // !80 #0F54DA";
	horizontal_wall_element->stroke() = horizontal_wall_element->fill();
	horizontal_wall_element->stroke_width() = "0";

	auto start_at_upper_left = std::make_shared<svg_path_elements::M<double>>(
		dss.LEFT_PADDING + dss.CELL_WIDTH * x,
		dss.TOP_PADDING - dss.HALF_WALL_LINE_WIDTH + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - y)
	);

	auto left_semicircle = std::make_shared<svg_path_elements::c<double>>(
		-dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH, 0,
		-dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH, 2 * dss.HALF_WALL_LINE_WIDTH,
		0, 2 * dss.HALF_WALL_LINE_WIDTH
	);

	auto go_right = std::make_shared<svg_path_elements::l<double>>(
		dss.CELL_WIDTH, 0
	);

	auto right_semicircle = std::make_shared<svg_path_elements::c<double>>(
		dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH, 0,
		dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH, -2 * dss.HALF_WALL_LINE_WIDTH,
		0, -2 * dss.HALF_WALL_LINE_WIDTH
	);

	auto go_back = std::make_shared<svg_path_elements::Z>();

	horizontal_wall_element->path_elements = { start_at_upper_left, left_semicircle, go_right, right_semicircle, go_back };

	return horizontal_wall_element;
}

std::unique_ptr<svg_path> get_vertical_west_wall(const tobor::v1_0::tobor_world& tobor_world, const drawing_style_sheet& dss, std::size_t x, std::size_t y) {
	auto vertical_wall_element = std::make_unique<svg_path>();

	vertical_wall_element->fill() = "black"; // !80 #0F54DA";
	vertical_wall_element->stroke() = vertical_wall_element->fill();
	vertical_wall_element->stroke_width() = "0";

	auto start_at_lower_left = std::make_shared<svg_path_elements::M<double>>(
		dss.LEFT_PADDING - dss.HALF_WALL_LINE_WIDTH + dss.CELL_WIDTH * x,
		dss.TOP_PADDING + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - y)
	);

	auto bottom_semicircle = std::make_shared<svg_path_elements::c<double>>(
		0, dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH,
		2 * dss.HALF_WALL_LINE_WIDTH, dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH,
		2 * dss.HALF_WALL_LINE_WIDTH, 0
	);

	auto go_north = std::make_shared<svg_path_elements::l<double>>(
		0, -dss.CELL_WIDTH
	);

	auto top_semicircle = std::make_shared<svg_path_elements::c<double>>(
		0, -dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH,
		-2 * dss.HALF_WALL_LINE_WIDTH, -dss.HALF_WALL_LINE_WIDTH * 2 * dss.WALL_CORNER_STRETCH,
		-2 * dss.HALF_WALL_LINE_WIDTH, 0
	);

	auto go_back = std::make_shared<svg_path_elements::Z>();

	vertical_wall_element->path_elements = { start_at_lower_left, bottom_semicircle, go_north, top_semicircle, go_back };

	return vertical_wall_element;
}

std::unique_ptr<svg_generator> draw_blocked_cells(const tobor::v1_0::tobor_world& tobor_world, const drawing_style_sheet& dss) {
	auto blocked_cells = std::make_unique<svg_compound>();

	for (std::size_t cell_id{ 0 }; cell_id < tobor_world.count_cells(); ++cell_id) {
		const auto universal_cell_id = tobor::v1_0::universal_cell_id::create_by_id(cell_id, tobor_world);
		//const auto cell_transposed_id = tobor_world.id_to_transposed_id(cell_id);
		if (
			tobor_world.west_wall_by_id(cell_id) &&
			tobor_world.east_wall_by_id(cell_id) &&
			tobor_world.south_wall_by_transposed_id(universal_cell_id.get_transposed_id()) &&
			tobor_world.north_wall_by_transposed_id(universal_cell_id.get_transposed_id())
			) {

			auto block = std::make_unique<svg_path>();

			block->fill() = "black";
			block->stroke() = block->fill();
			block->stroke_width() = "0";

			auto start_at_north_west = std::make_shared<svg_path_elements::M<double>>(
				dss.LEFT_PADDING + dss.CELL_WIDTH * universal_cell_id.get_x_coord(),
				dss.TOP_PADDING + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - universal_cell_id.get_y_coord() - 1)
			);

			auto go_east = std::make_shared<svg_path_elements::l<double>>(
				dss.CELL_WIDTH, 0
			);

			auto go_south = std::make_shared<svg_path_elements::l<double>>(
				0, dss.CELL_HEIGHT
			);

			auto go_west = std::make_shared<svg_path_elements::l<double>>(
				-dss.CELL_WIDTH, 0
			);

			auto go_back = std::make_shared<svg_path_elements::Z>();

			block->path_elements = { start_at_north_west, go_east, go_south, go_west, go_back };

			blocked_cells->elements.push_back(std::move(block));

		}
	}

	return blocked_cells;
}

std::unique_ptr<svg_generator> draw_walls(const tobor::v1_0::tobor_world& tobor_world, const drawing_style_sheet& dss) {
	auto svg_walls = std::make_unique<svg_compound>();

	// horizontal walls:
	for (std::size_t x = 0; x < tobor_world.get_horizontal_size(); ++x) {
		for (std::size_t y = 0; y <= tobor_world.get_vertical_size(); ++y) {
			if (
				tobor_world.south_wall_by_transposed_id(tobor::v1_0::universal_cell_id::create_by_coordinates(x, y, tobor_world).get_transposed_id())
				) {
				// ! Note that y exceeds its natural range by 1. But it is okay, since we operate on an infinite repeating 2D landscape
				svg_walls->elements.push_back(get_horizontal_south_wall(tobor_world, dss, x, y));
			}
		}
	}

	// vertical walls:
	for (std::size_t x = 0; x <= tobor_world.get_horizontal_size(); ++x) {
		for (std::size_t y = 0; y < tobor_world.get_vertical_size(); ++y) {
			if (
				tobor_world.west_wall_by_id(tobor::v1_0::universal_cell_id::create_by_coordinates(x, y, tobor_world).get_id())
				) {
				// ! Note that y exceeds its natural range by 1. But it is okay, since we operate on an infinite repeating 2D landscape
				svg_walls->elements.push_back(get_vertical_west_wall(tobor_world, dss, x, y));
			}
		}
	}


	return svg_walls;
}

std::unique_ptr<svg_generator> draw_duck_piece(const tobor::v1_0::tobor_world& tobor_world, const drawing_style_sheet& dss, const tobor::v1_0::universal_cell_id& cell, bool look_right, const std::string& color) {

	(void)look_right;

	auto duck = std::make_unique<svg_path>();

	duck->fill() = color;
	duck->stroke() = "black";
	duck->stroke_width() = std::to_string(dss.PIECE_LINE_WIDTH);

	const auto CELL_CORNER_SOUTH_WEST_x = dss.LEFT_PADDING + dss.CELL_WIDTH * cell.get_x_coord();
	const auto CELL_CORNER_SOUTH_WEST_y = dss.TOP_PADDING + dss.CELL_HEIGHT * (tobor_world.get_vertical_size() - cell.get_y_coord());

	const auto CANVAS_X_SIZE = dss.CELL_WIDTH - 2 * dss.HORIZONTAL_PIECE_PADDING;
	const auto CANVAS_Y_SIZE = dss.CELL_HEIGHT - 2 * dss.VERTICAL_PIECE_PADDING;

	const auto MINIMUM_CANVAS_SIZE = std::min(CANVAS_X_SIZE, CANVAS_Y_SIZE);

	const auto FOOT_THICKNESS_RIGHT = CANVAS_Y_SIZE * dss.PIECE_FOOT_THICKNESS_FACTOR;

	auto start_at_south_west = std::make_shared<svg_path_elements::M<double>>(
		CELL_CORNER_SOUTH_WEST_x + dss.HORIZONTAL_PIECE_PADDING,
		CELL_CORNER_SOUTH_WEST_y - dss.VERTICAL_PIECE_PADDING
	);

	auto go_bottom_east = std::make_shared<svg_path_elements::l<double>>(
		dss.CELL_WIDTH - 2 * dss.HORIZONTAL_PIECE_PADDING, 0
	);

	auto go_bottom_right_corner_north = std::make_shared<svg_path_elements::l<double>>(
		0, -FOOT_THICKNESS_RIGHT
	);

	auto go_back_neck_bottom_up = std::make_shared<svg_path_elements::c<double>>(
		-0.5 * CANVAS_X_SIZE, 0,
		-0.5 * CANVAS_X_SIZE, 0,
		-0.4 * CANVAS_X_SIZE, -(0.45 * CANVAS_Y_SIZE - FOOT_THICKNESS_RIGHT)
	);

	auto go_head = std::make_shared<svg_path_elements::a<double>>(
		svg_path_elements::a<double>::step(dss.PIECE_HEAD_X_RADIUS_FACTOR * MINIMUM_CANVAS_SIZE, dss.PIECE_HEAD_Y_RADIUS_FACTOR * MINIMUM_CANVAS_SIZE, dss.PIECE_HEAD_AXIS_ROTATION, true, false, -0.2 * MINIMUM_CANVAS_SIZE, -0.25 * MINIMUM_CANVAS_SIZE)
	);

	auto go_nort_west_for_beak = std::make_shared<svg_path_elements::l<double>>(
		-0.33 * MINIMUM_CANVAS_SIZE, -0.15 * MINIMUM_CANVAS_SIZE
	);

	auto go_south_east_for_beak_down = std::make_shared<svg_path_elements::l<double>>(
		0.32 * MINIMUM_CANVAS_SIZE, 0.25 * MINIMUM_CANVAS_SIZE
	);

	auto go_back = std::make_shared<svg_path_elements::Z>();

	duck->path_elements = { start_at_south_west, go_bottom_east, go_bottom_right_corner_north, go_back_neck_bottom_up, go_head , go_nort_west_for_beak, go_south_east_for_beak_down, go_back };

	auto outer_eye = std::make_unique<svg_path>();

	outer_eye->fill() = "white";
	outer_eye->stroke() = "black";
	outer_eye->stroke_width() = std::to_string(dss.PIECE_LINE_WIDTH);


	auto start_eye = std::make_shared<svg_path_elements::M<double>>(
		CELL_CORNER_SOUTH_WEST_x + dss.HORIZONTAL_PIECE_PADDING + 0.66 * MINIMUM_CANVAS_SIZE,
		CELL_CORNER_SOUTH_WEST_y - (dss.VERTICAL_PIECE_PADDING + 0.77 * MINIMUM_CANVAS_SIZE)
	);

	outer_eye->path_elements.push_back(start_eye);

	outer_eye->path_elements.push_back(
		std::make_shared<svg_path_elements::a<double>>(
			svg_path_elements::a<double>::step(
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


	auto comp = std::make_unique<svg_compound>(std::move(duck), std::move(outer_eye));
	
	return comp;

}


std::string draw_tobor_world(const tobor::v1_0::tobor_world& tobor_world) {

	drawing_style_sheet dss;

	auto svg_target = std::make_unique<svg_compound>();


	auto svg_pieces = std::make_unique<svg_compound>();
	svg_pieces->elements.push_back(draw_duck_piece(tobor_world, dss, tobor::v1_0::universal_cell_id::create_by_coordinates(3, 4, tobor_world), true, "green"));


	// The following order is final:
	auto svg_body = std::make_unique<svg_compound>(
		draw_tobor_background(tobor_world, dss),
		draw_blocked_cells(tobor_world, dss),
		std::move(svg_target),
		draw_tobor_grid(tobor_world, dss),
		draw_walls(tobor_world, dss),
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

		By now there are three variants to place pieces behind/before  ...  grid / walls
	*/



	const std::string svg_root_height = std::to_string(dss.CELL_HEIGHT * tobor_world.get_vertical_size() + dss.TOP_PADDING + dss.BOTTOM_PADDING);
	const std::string svg_root_width = std::to_string(dss.CELL_WIDTH * tobor_world.get_vertical_size() + dss.LEFT_PADDING + dss.RIGHT_PADDING);
	auto svg_root = std::make_unique<svg_environment>(svg_root_height, svg_root_width, std::make_unique<xml_header>(), std::move(svg_body));




	standard_logger()->info(svg_root->get_svg());
	auto fs = std::ofstream(R"(C:\Users\F-NET-ADMIN\Desktop\preview.svg)");

	fs << svg_root->get_svg();

	return svg_root->get_svg();

}

