#include "custom_traits.h"

namespace {
	struct example {

		static void test() {
			(void)mapped_ptr; // get rid of the unused warning
		}
	};
}
