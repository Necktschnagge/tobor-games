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

void test() {

	auto grid = std::make_unique<svg_compound>();
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
	return 0;
}

