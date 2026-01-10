#pragma once

#include "xml_declaration.h"

#include <fmt/format.h>

#include <memory>

namespace tobor {
	namespace latest {
		namespace svggen {

			class xml_root_element final : public fsl::i_to_std_string {

				static std::string wrap_svg(const std::string& height, const std::string& width, const std::string& nested_content) {
					return fmt::format("<svg height=\"{}\" width=\"{}\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">\n{}</svg>", height, width, nested_content);
				}

				std::unique_ptr<fsl::i_to_std_string> body;
				std::string                           height;
				std::string                           width;

			public:
				xml_root_element(const std::string& height, const std::string& width, std::unique_ptr<fsl::i_to_std_string> body) :
				   body(std::move(body)),
				   height(height),
				   width(width) {}

				virtual std::string to_std_string() const override { return xml_declaration().to_std_string() + wrap_svg(height, width, body->to_std_string()); }
			};

		} // namespace svggen
	}    // namespace latest
} // namespace tobor
