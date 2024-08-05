#pragma once

#include <vector>

namespace tobor {
	namespace v1_0 {

		/**
		*	@brief One single boolean wall
		*/
		class wall {

			bool _is_wall;

		public:

			using type = wall;

			wall(bool is_wall) : _is_wall(is_wall) {}

			operator bool() const { return _is_wall; }

			operator bool& () { return _is_wall; }

		};

		using wall_vector = std::vector<wall>;
	}

	namespace v1_1 {
		using wall = tobor::v1_0::wall;
		using wall_vector = std::vector<wall>;
	}
}
