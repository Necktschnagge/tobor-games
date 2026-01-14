#pragma once

namespace fsl {
	namespace c {

		class stringval : public fsl::i::stringable {

			std::string _string;

		public:
			stringval() noexcept = default;

			stringval(const std::string& string) : _string(string) {}

			inline void set(const std::string& string) { this->_string = string; }

			virtual std::string to_string() const override { return _string; }
		};

	} // namespace c
} // namespace fsl
