#pragma once

#include <map>

namespace tobor {
	namespace latest {
		struct tess_solver {

			using number = uint64_t;

			struct profile_count {
				std::map<number, number> counters;
			};

			profile_count advance(const profile_count& current) {}

			void foo() {

				// clang-format off
				std::array<number> tiles{
				1+2+4,
				1+2+16,
				8+1+2,
				1+8+16,
				1+8+64,
				2+16+8
				};
				// clang-format on
			}

			number calc(int width) {

				profile_count initial;

				initial.counters[0] = 1;

				for (int i = 0; i < width; ++i) {
					initial = advance(initial);
				}
			}
		};

	} // namespace latest
} // namespace tobor
