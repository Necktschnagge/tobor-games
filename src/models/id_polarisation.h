#pragma once

#include "direction.h"

namespace tobor {
	namespace v1_1 {

		/**
		*	@brief A Wrapper for a bool to distinguish between (non-transposed) id used for directions EAST, WEST and transposed id used for directions NORTH, SOUTH
		*	
		*	@details The underlying bool is true if indicating transposed ids (NORTH, SOUTH)
		*/
		class id_polarisation {

			bool is_transposed{ false };

		public:
			constexpr id_polarisation() noexcept : is_transposed{ false } {}
			constexpr id_polarisation(const id_polarisation&) noexcept = default;
			id_polarisation(id_polarisation&&) noexcept = default;

			id_polarisation& operator=(const id_polarisation&) noexcept = default;
			id_polarisation& operator=(id_polarisation&&) noexcept = default;

			inline constexpr id_polarisation(const direction& d) noexcept : is_transposed(d.is_transposed_id_direction()) {}

			inline constexpr operator bool() const noexcept { return is_transposed; };
		};
	}
}
