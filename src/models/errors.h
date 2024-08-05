#pragma once

#include <stdexcept>

namespace tobor {

	namespace v1_0 {

		/**
		*	@brief An error to be thrown when a number is of wrong remainder class on division by 2.
		*/
		class division_by_2_error : public std::logic_error {
			inline static const char MESSAGE[]{ "Wrong remainder on division by 2" };
		public:
			division_by_2_error() : std::logic_error(MESSAGE) {}
		};


		/**
		*	@brief An error to be thrown when there are too many blocked cells in the center of a game board.
		*/
		class blocked_center_error : public std::logic_error {
			inline static const char MESSAGE[]{ "Blocking too many cells." };
		public:
			blocked_center_error() : std::logic_error(MESSAGE) {}
		};

	}

	namespace v1_1 {
		using division_by_2_error = tobor::v1_0::division_by_2_error;
		using blocked_center_error = tobor::v1_0::blocked_center_error;
	}
}
