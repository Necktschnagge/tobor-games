#pragma once

#include "../path_command.h"

namespace tobor {
	namespace latest {
		namespace svggen {
			namespace pathc {

				template <class Number>
				class M : public path_command {
					// https://svgwg.org/svg2-draft/paths.html#PathDataMovetoCommands

				public:
					std::vector<std::pair<Number, Number>> coordinates;

					M(const Number& x, const Number& y) : coordinates{ std::make_pair(x, y) } {}

					M() : M(0, 0) {}

					virtual std::string to_string() const override {
						return std::accumulate(coordinates.cbegin(), coordinates.cend(), std::string("M"), [](const std::string& acc, const std::pair<Number, Number>& el) -> std::string {
							return acc + " " + std::to_string(el.first) + " " + std::to_string(el.second);
						});
					}

					virtual std::unique_ptr<path_command> clone() const override { return std::make_unique<M>(*this); }

					virtual ~M() override {}
				};

				// add the relative move! TODO

			} // namespace pathc
		}    // namespace svggen
	}       // namespace latest
} // namespace tobor
