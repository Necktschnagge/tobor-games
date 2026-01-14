#pragma once

#include "../path_command.h"

//#include <fmt/format.h>

namespace tobor {
	namespace latest {
		namespace svggen {
			namespace pathc {

				struct a_step {
					double rx;
					double ry;
					double x_axis_rotation;
					bool   large_arc_flag;
					bool   sweep_flag;
					double x;
					double y;

					std::string str() const noexcept {
						return std::format("{} {} {} {} {} {} {}",
						                   std::to_string(rx),
						                   std::to_string(ry),
						                   std::to_string(x_axis_rotation),
						                   std::to_string(static_cast<int>(large_arc_flag)),
						                   std::to_string(static_cast<int>(sweep_flag)),
						                   std::to_string(x),
						                   std::to_string(y));
					}

				public:
					a_step(double rx, double ry, double x_axis_rotation, bool large_arc_flag, bool sweep_flag, double x, double y) :
					   rx(rx),
					   ry(ry),
					   x_axis_rotation(x_axis_rotation),
					   large_arc_flag(large_arc_flag),
					   sweep_flag(sweep_flag),
					   x(x),
					   y(y) {}
				};

				// Number unused!!!
				template <class Number>
				class a : public path_command {

					friend class svg_path;

				private:
					std::vector<a_step> coordinates;

				public:
					a(const a_step& s) : coordinates({ s }) {}

					inline void add_coordinates() const noexcept {}

					template <class... _Rest>
					void add_coordinates(const a_step& step, _Rest&&... others) {
						coordinates.push_back(step);
						add_coordinates(std::forward<_Rest>(others)...);
					}

					template <class... _Rest>
					a(const a_step& s, _Rest&&... others) : coordinates({ s }) {
						add_coordinates(std::forward<_Rest>(others)...);
					}

					virtual std::string to_string() const override {
						return std::accumulate(coordinates.cbegin(), coordinates.cend(), std::string("a"), [](const std::string& acc, const a_step& el) -> std::string {
							return acc + " " + el.str();
						});
					}

					virtual std::unique_ptr<path_command> clone() const override { return std::make_unique<a>(*this); }

					virtual ~a() override {}
				};

				// add the relative move! TODO

			} // namespace pathc
		}    // namespace svggen
	}       // namespace latest
} // namespace tobor
