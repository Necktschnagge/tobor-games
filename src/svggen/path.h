#pragma once

#include "path_command.h"

#include "../fsl/interface/stringable.h"

#include <map>
#include <memory>
#include <vector>

namespace tobor {
	namespace latest {
		namespace svggen {

			class path : public fsl::i::stringable {

			public:
				using map = std::map<std::string, std::string>;

				map properties;

				std::vector<std::shared_ptr<path_command>> path_commands;

				path() noexcept {}

				path(const path& other) : properties(other.properties) {
					std::transform(other.path_commands.cbegin(), other.path_commands.cend(), std::back_inserter(this->path_commands), [](const std::shared_ptr<path_command>& el) {
						return el->clone();
					});
				}

				path(path&& other) = delete;

				path& operator=(const path& other) = delete;

				path& operator=(path&& other) = delete;

				virtual std::string to_string() const override {
					auto result = std::string("<path");

					result += std::string(" d=\"") +
					          std::accumulate(path_commands.cbegin(),
					                          path_commands.cend(),
					                          std::string(""),
					                          [](const std::string& acc, const std::shared_ptr<path_command>& el) -> std::string {
						                          return acc.empty() ? el->to_string() : acc + " " + el->to_string();
					                          }) +
					          "\"";
					result += std::accumulate(properties.cbegin(), properties.cend(), std::string(""), [](const std::string& acc, const map::value_type& el) -> std::string {
						return acc + " " + el.first + "=" + "\"" + el.second + "\"";
					});
					result += std::string("/>");
					return result;
				}

				std::string& fill() { return properties["fill"]; }
				std::string& stroke() { return properties["stroke"]; }
				std::string& stroke_width() { return properties["stroke-width"]; }
				std::string& stroke_linecap() { return properties["stroke-linecap"]; }
				std::string& stroke_linejoin() { return properties["stroke-linejoin"]; }
			};

		} // namespace svggen
	}    // namespace latest
} // namespace tobor
