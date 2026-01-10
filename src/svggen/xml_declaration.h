#pragma once

#include "../fsl/i_to_std_string.h"

namespace tobor {
	namespace latest {
		namespace svggen {

			class xml_declaration final : public fsl::i_to_std_string {
				public:
				virtual std::string to_std_string() const override {
					return R"xxx(<?xml version="1.0" standalone="no"?>
)xxx";
				}
			};

		} // namespace svggen
	}    // namespace latest
} // namespace tobor