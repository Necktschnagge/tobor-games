#pragma once

#include <string>

namespace tobor {

	namespace v1_0 {

		/**
		*	@brief Represents one of the four directions NORTH, EAST, SOUTH, WEST, or none of them, called END.
		*/
		class direction {
		public:

			/** underlying integer type */
			using int_type = uint8_t;

			using type = direction; // check if used anywhere!!

		private:

			int_type value;

			constexpr direction(int_type v) : value(v) {}

			static constexpr uint8_t direction_invert_array[17]{ // check if this should be inline static constexpr !!!
					0x10,
					0b0100, // 1 -> 4 // 0 -> 2
					0b1000, // 2 -> 8 // 1 -> 3
					0x10,
					0b0001, // 4 -> 1 // 2 -> 0
					0x10,
					0x10,
					0x10,
					0b0010, // 8 -> 2 // 3 -> 1
					0x10,
					0x10,
					0x10,
					0x10,
					0x10,
					0x10,
					0x10,
					0x10, // END -> END
			};

		public:

			struct encoding {

				// add no formatting block here:
				static constexpr int_type NORTH { 1 << 0 };
				static constexpr int_type EAST  { 1 << 1 };
				static constexpr int_type SOUTH { 1 << 2 };
				static constexpr int_type WEST  { 1 << 3 };
				static constexpr int_type END   { 1 << 4 };
				static constexpr int_type NONE  { END };

				// add no formatting block here:
				static_assert(NORTH != EAST,  "piece_move: NORTH == EAST");
				static_assert(NORTH != SOUTH, "piece_move: NORTH == SOUTH");
				static_assert(NORTH != WEST,  "piece_move: NORTH == WEST");
				static_assert(NORTH != END,   "piece_move: NORTH == END");
				static_assert( EAST != SOUTH, "piece_move:  EAST == SOUTH");
				static_assert( EAST != WEST,  "piece_move:  EAST == WEST");
				static_assert( EAST != END,   "piece_move:  EAST == END");
				static_assert(SOUTH != WEST,  "piece_move: SOUTH == WEST");
				static_assert(SOUTH != END,   "piece_move: SOUTH == END");
				static_assert( WEST != END,   "piece_move:  WEST == END");
				
				static_assert( NORTH < EAST && EAST < SOUTH && SOUTH < WEST && WEST < END,   "piece_move: broken order");
			};

			constexpr direction(const direction&) = default;

			constexpr direction(direction&&) = default;

			inline direction& operator=(const direction&) = default;

			inline direction& operator=(direction&&) = default;

			// add no formatting block here:
			inline static constexpr direction NORTH() { return direction(encoding::NORTH); }
			inline static constexpr direction EAST()  { return direction(encoding::EAST);  }
			inline static constexpr direction SOUTH() { return direction(encoding::SOUTH); }
			inline static constexpr direction WEST()  { return direction(encoding::WEST);  }
			inline static constexpr direction NONE()  { return direction(encoding::NONE);  }

			inline constexpr bool is_id_direction()            const noexcept { return (value & (encoding::EAST  | encoding::WEST )); }
			inline constexpr bool is_transposed_id_direction() const noexcept { return (value & (encoding::NORTH | encoding::SOUTH)); }

			/* use it like an iterator over directions: */
			inline static constexpr direction begin() { return direction(encoding::NORTH); }
			inline static constexpr direction end()   { return direction(encoding::END  ); }

			inline direction& operator++() { value <<= 1; return *this; }
			inline direction operator++(int) { direction c = *this; value <<= 1; return c; }

			inline std::strong_ordering operator<=>(const direction& another) const { return value <=> another.value; }

			/** Conversion to underlying integer type */
			inline constexpr int_type get() const noexcept { return value; }
			inline constexpr operator int_type() const noexcept { return value; }

			/** Returns character N E S or W, but space in case of NONE / END direction. */
			inline constexpr char to_char() const {
				switch (value) {
				case encoding::NORTH:
					return 'N';
				case encoding::EAST:
					return 'E';
				case encoding::SOUTH:
					return 'S';
				case encoding::WEST:
					return 'W';
				default:
					return ' ';
				}
			}

			/** Returns a std::string containing exactly one character matching this->to_char() */
			inline constexpr operator std::string() const {
				char x = to_char();
				return std::string(&x, 1);
			}

			/**
			* @brief Returns the opposite direction.
			* @details North and south are opposites, east is opposite of west and vice versa.
			* Note that NONE / END direction is mapped to NONE / END.
			*/
			inline constexpr direction operator!() const noexcept { return direction(direction_invert_array[value]); }

		};
	}

	namespace v1_1 {

		using direction = tobor::v1_0::direction;
	}
}
