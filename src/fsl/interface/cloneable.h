#pragma once

#include <memory>
#include <type_traits>

namespace fsl {
	namespace i {

		template <class T>
		class cloneable {
		public:
			virtual [[nodiscard]] std::decay_t<T>* clone() const = 0;
			virtual ~cloneable() noexcept                        = default;
		};

		template <class T>
		class unique_cloneable {
		public:
			virtual [[nodiscard]] std::unique_ptr<std::decay_t<T>> clone() const = 0;
			virtual ~unique_cloneable() noexcept                                 = default;
		};

	} // namespace i
} // namespace fsl
