#pragma once

#include "direction.h"

namespace tobor {
	namespace v1_1 {

		/**
		*	@brief A Wrapper for a bool to distinguish between (non-transposed) id - direction (EAST - WEST) and transposed id - direction (NORTH - SOUTH)
		*/
		class id_polarisation {

			bool is_transposed{ false };

		public:
			id_polarisation() {}
			id_polarisation(const id_polarisation&) = default;
			id_polarisation(id_polarisation&&) = default;

			id_polarisation& operator=(const id_polarisation&) = default;
			id_polarisation& operator=(id_polarisation&&) = default;

			inline id_polarisation(const direction& d) : is_transposed(d.is_transposed_id_direction()) {}

			inline operator bool() const noexcept { return is_transposed; };
		};
	}
}