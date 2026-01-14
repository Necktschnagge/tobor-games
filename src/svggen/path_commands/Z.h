#pragma once

#include "../path_command.h"

namespace tobor {
	namespace latest {
		namespace svggen {
			namespace pathc {

				class Z : public latest::svggen::path_command {

				public:
					Z() {}

					virtual std::string to_string() const override { return "Z"; }

					virtual std::unique_ptr<path_command> clone() const override { return std::make_unique<Z>(*this); }

					virtual ~Z() override {}
				};

				// add the relative move! TODO

			} // namespace pathc
		}    // namespace svggen
	}       // namespace latest
} // namespace tobor
