#pragma once

#include "../fsl/interface/stringable.h"

#include <memory>
#include <vector>

namespace tobor {
	namespace latest {
		namespace svggen {

			class compound : public fsl::i::stringable {
			public:
				using unique_ptr = std::unique_ptr<fsl::i::stringable>;

				std::vector<unique_ptr> elements;

			public:
				compound() noexcept = default;

				template <class... T>
				compound(T&&... init) {
					((void) elements.push_back(std::forward<T>(init)), ...);
				}

				virtual std::string to_string() const override {
					return std::accumulate(elements.cbegin(), elements.cend(), std::string(), [](const std::string& acc, const unique_ptr& el) {
						return acc + el->to_string();
					});
				}
			};

		} // namespace svggen
	}    // namespace latest
} // namespace tobor
