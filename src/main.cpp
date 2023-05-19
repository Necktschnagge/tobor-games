#include "logger.h"
#include "engine.h"

#include <string>

class svg_generator {

	static std::string get_xml_header() {
		return R"xxx(<?xml version="1.0" standalone="no"?>
)xxx";
	}

	static std::string wrap_svg(const std::string& height, const std::string& width, const std::string& nested_content)
		< svg width = "6400" height = "1600" version = "1.1" xmlns = "http://www.w3.org/2000/svg" >



public:
	static std::string create_svg() {
		std::string svg_result;

		return svg_result;
	}

	std::string get_svg() const virtual = 0;


};

class xml_header : public svg_generator {

public:
	std::string get_svg() const virtual override {
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

	const svg_generator& body;
	std::string height;
	std::string width;

public:

	svg_environment(const std::string& height, const std::string& width, const svg_generator& body) :
		body(body),
		height(height),
		width(width)
	{}

	std::string get_svg() const virtual override {
		return wrap_svg(height, width, body.get_svg());
	}

};


void test() {


	body = grid + walls + robots;
	file = header + svg_environment(body);

}


int main()
{
	init_logger();
	return 0;
}

