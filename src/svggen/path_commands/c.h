#pragma once

#include "../path_command.h"

// #include <fmt/format.h>

namespace tobor {
	namespace latest {
		namespace svggen {
			namespace pathc {

				template <class Number>
				class c : public path_command {

					friend class svg_path;

					std::vector<std::array<Number, 6>> coordinates;

				public:
					c(const Number& control1_x, const Number& control1_y, const Number& control2_x, const Number& control2_y, const Number& destination_x, const Number& destination_y) :
					   coordinates({ { control1_x, control1_y, control2_x, control2_y, destination_x, destination_y } }) {}

					inline void add_coordinates() const noexcept {}

					template <class... _Rest>
					void add_coordinates(const Number& control1_x,
					                     const Number& control1_y,
					                     const Number& control2_x,
					                     const Number& control2_y,
					                     const Number& destination_x,
					                     const Number& destination_y,
					                     _Rest&&... others) {
						coordinates.emplace_back({ control1_x, control1_y, control2_x, control2_y, destination_x, destination_y });
						add_coordinates(std::forward<_Rest>(others)...);
					}

					template <class... _Rest>
					c(const Number& control1_x,
					  const Number& control1_y,
					  const Number& control2_x,
					  const Number& control2_y,
					  const Number& destination_x,
					  const Number& destination_y,
					  _Rest&&... others) :
					   coordinates({ control1_x, control1_y, control2_x, control2_y, destination_x, destination_y }) {
						add_coordinates(std::forward<_Rest>(others)...);
					}

					virtual std::string to_string() const override {
						return std::accumulate(coordinates.cbegin(), coordinates.cend(), std::string("c"), [](const std::string& acc, const std::array<Number, 6>& el) -> std::string {
							std::string result = acc;
							for (std::size_t i = 0; i < 6; ++i) {
								result += " " + std::to_string(el[i]);
							}
							return result;
						});
					}

					virtual std::unique_ptr<path_command> clone() const override { return std::make_unique<c>(*this); }

					virtual ~c() override {}
				};

				template <class Number>
				class C : public path_command {

					friend class svg_path;

					std::vector<std::array<Number, 6>> coordinates;

				public:
					C(const Number& control1_x, const Number& control1_y, const Number& control2_x, const Number& control2_y, const Number& destination_x, const Number& destination_y) :
					   coordinates({ { control1_x, control1_y, control2_x, control2_y, destination_x, destination_y } }) {}

					inline void add_coordinates() const noexcept {}

					template <class... _Rest>
					void add_coordinates(const Number& control1_x,
					                     const Number& control1_y,
					                     const Number& control2_x,
					                     const Number& control2_y,
					                     const Number& destination_x,
					                     const Number& destination_y,
					                     _Rest&&... others) {
						coordinates.emplace_back({ control1_x, control1_y, control2_x, control2_y, destination_x, destination_y });
						add_coordinates(std::forward<_Rest>(others)...);
					}

					template <class... _Rest>
					C(const Number& control1_x,
					  const Number& control1_y,
					  const Number& control2_x,
					  const Number& control2_y,
					  const Number& destination_x,
					  const Number& destination_y,
					  _Rest&&... others) :
					   coordinates({ control1_x, control1_y, control2_x, control2_y, destination_x, destination_y }) {
						add_coordinates(std::forward<_Rest>(others)...);
					}

					virtual std::string to_string() const override {
						return std::accumulate(coordinates.cbegin(), coordinates.cend(), std::string("C"), [](const std::string& acc, const std::array<Number, 6>& el) -> std::string {
							std::string result = acc;
							for (std::size_t i = 0; i < 6; ++i) {
								result += " " + std::to_string(el[i]);
							}
							return result;
						});
					}

					virtual std::unique_ptr<path_command> clone() const override { return std::make_unique<C>(*this); }

					virtual ~C() override {}
				};

			} // namespace pathc
		}    // namespace svggen
	}       // namespace latest
} // namespace tobor
