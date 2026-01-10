#pragma once

#include <string>

namespace fsl {

	class i_to_std_string {
		public:
		virtual std::string to_std_string() const = 0;
		virtual ~i_to_std_string() {}
	};

} // namespace fsl
