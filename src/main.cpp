#include "logger.h"
#include "engine.h"

#include <string>
#include <numeric>

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

	std::unique_ptr<svg_generator> body;
	std::string height;
	std::string width;

public:

	svg_environment(const std::string& height, const std::string& width, std::unique_ptr<svg_generator> body) :
		body(std::move(body)),
		height(height),
		width(width)
	{}

	virtual std::string get_svg() const override {
		return wrap_svg(height, width, body->get_svg());
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


class svg_path : public svg_generator {

public:

	class element {

	public:

		virtual std::string str() const = 0;



	};

	template <class Number>
	class M : public element {

		std::vector<std::pair<Number, Number>> coordinates;

		virtual std::string str() const override {
			return std::accumulate(coordinates.cbegin(), coordinates.cend(), "M",
				[](const std::string& acc, const std::pair<Number, Number>& el) -> std::string {
					return acc + " " + std::to_string(el.first) + " " + std::to_string(el.second);
				}
			);
		}
	};

	using u_ptr = std::unique_ptr<svg_generator>;

	//std::vector<u_ptr> elements;
	//std::string primitive;
	using map = std::map<std::string, std::string>;
	map other_properties;
	std::vector<std::shared_ptr<element>> path_elements;


	svg_path() {}

	//svg_compound(std::initializer_list<std::unique_ptr<svg_compound>> init) : elements(init) {}

	//template<class... T>
	//svg_path(const std::string& primitive) : primitive(primitive) {}

	virtual std::string get_svg() const override {
		auto result = std::string("<path");

		result += std::string(" d=\"") +
			std::accumulate(
				path_elements.cbegin(),
				path_elements.cend(),
				std::string(""),
				[](const std::string& acc, const std::shared_ptr<element>& el) -> std::string {
					return acc + el->str();
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
};

void test() {




	const std::string test_s{ R"xxx(<path d="M 10 70 l 200 100 l 200 50 z"
        fill="red" stroke="blue" stroke-width="3" />
)xxx"};
	const std::string test_s2{ R"xxx(<path d="M 0 0 h 400 v 400 h -400 v -400 z"
        fill="grey" stroke="black" stroke-width="10" />
)xxx"};

	auto example_path_element = std::make_unique<svg_primitive>(test_s);
	auto outside_box = std::make_unique<svg_primitive>(test_s2);

	//<rect width="100%" height="100%" fill="grey" />


	auto grid = std::make_unique<svg_compound>(std::move(example_path_element), std::move(outside_box));
	auto walls = std::make_unique<svg_compound>();
	auto robots = std::make_unique<svg_compound>();

	auto body = std::make_unique<svg_compound>(
		std::move(grid),
		std::move(walls),
		std::move(robots)
	);

	auto svg_env = std::make_unique<svg_environment>("400", "400", std::move(body));

	svg_compound svg_file;
	svg_file.elements.emplace_back(std::make_unique<xml_header>());
	svg_file.elements.emplace_back(std::move(svg_env));

	standard_logger()->info(svg_file.get_svg());
	/*
	*/

}
#if false
#endif


int main()
{
	init_logger();
	test();
	return 0;
}

