#pragma once

#include <string>

namespace fsl {
	namespace i {

		class stringable {
		public:
			virtual std::string to_string() const = 0;
			virtual ~stringable() {}
		};

	} // namespace i
} // namespace fsl
