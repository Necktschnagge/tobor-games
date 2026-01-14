#pragma once

#include "../fsl/interface/stringable.h"

namespace tobor {
	namespace latest {
		namespace svggen {

			class xml_declaration final : public fsl::i::stringable {
				public:
				virtual std::string to_string() const override {
					return R"xxx(<?xml version="1.0" standalone="no"?>
)xxx";
				}
			};

		} // namespace svggen
	}    // namespace latest
} // namespace tobor