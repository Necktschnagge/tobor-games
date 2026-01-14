#pragma once

#include "../fsl/interface/cloneable.h"
#include "../fsl/interface/stringable.h"

namespace tobor {
	namespace latest {
		namespace svggen {

			class path_command : public fsl::i::stringable, public fsl::i::unique_cloneable<path_command> {
			public:
				virtual ~path_command() noexcept override {}
			};

		} // namespace svggen
	}    // namespace latest
} // namespace tobor
