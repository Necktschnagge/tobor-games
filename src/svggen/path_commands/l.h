#pragma once

#include "../path_command.h"

namespace tobor {
	namespace latest {
		namespace svggen {
			namespace pathc {

				template <class Number>
				class l : public path_command {

					friend class svg_path;

					std::vector<std::pair<Number, Number>> coordinates;

				public:
					l(const Number& x, const Number& y) : coordinates({ std::make_pair(x, y) }) {}

					inline void add_coordinates() const noexcept {}

					template <class... _Rest>
					void add_coordinates(const Number& x, const Number& y, _Rest&&... others) {
						coordinates.emplace_back(x, y);
						add_coordinates(std::forward<_Rest>(others)...);
					}

					template <class... _Rest>
					l(const Number& x, const Number& y, _Rest&&... others) : coordinates({ std::make_pair(x, y) }) {
						add_coordinates(std::forward<_Rest>(others)...);
					}

					virtual std::string to_string() const override {
						return std::accumulate(coordinates.cbegin(), coordinates.cend(), std::string("l"), [](const std::string& acc, const std::pair<Number, Number>& el) -> std::string {
							return acc + " " + std::to_string(el.first) + " " + std::to_string(el.second);
						});
					}

					virtual std::unique_ptr<path_command> clone() const override { return std::make_unique<l>(*this); }

					virtual ~l() override {}
				};

				// add the absolute / relative move! TODO

			} // namespace pathc
		}    // namespace svggen
	}       // namespace latest
} // namespace tobor
