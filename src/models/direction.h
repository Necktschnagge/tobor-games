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

			using type = direction;

		private:

			int_type value;

			direction(int_type v) : value(v) {}

			static constexpr uint8_t direction_invert_array[17]{
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

			direction(const direction&) = default;

			direction(direction&&) = default;

			direction& operator=(const direction&) = default;

			direction& operator=(direction&&) = default;

			struct encoding {

				static constexpr int_type NORTH{ 1 << 0 };
				static constexpr int_type EAST{ 1 << 1 };
				static constexpr int_type SOUTH{ 1 << 2 };
				static constexpr int_type WEST{ 1 << 3 };
				static constexpr int_type END{ 1 << 4 };
				static constexpr int_type NONE{ END };

				static_assert(NORTH != EAST, "piece_move: NORTH == EAST");
				static_assert(NORTH != SOUTH, "piece_move: NORTH == SOUTH");
				static_assert(NORTH != WEST, "piece_move: NORTH == WEST");
				static_assert(NORTH != END, "piece_move: NORTH == END");
				static_assert(EAST != SOUTH, "piece_move: EAST == SOUTH");
				static_assert(EAST != WEST, "piece_move: EAST == WEST");
				static_assert(EAST != END, "piece_move: EAST == END");
				static_assert(SOUTH != WEST, "piece_move: SOUTH == WEST");
				static_assert(SOUTH != END, "piece_move: SOUTH == END");
				static_assert(WEST != END, "piece_move: WEST == END");
			};

			inline static direction NORTH() { return encoding::NORTH; }
			inline static direction EAST() { return encoding::EAST; }
			inline static direction SOUTH() { return encoding::SOUTH; }
			inline static direction WEST() { return encoding::WEST; }
			inline static direction NONE() { return encoding::NONE; }

			inline bool is_id_direction() const noexcept { return (value & (encoding::EAST | encoding::WEST)); }
			inline bool is_transposed_id_direction() const noexcept { return (value & (encoding::NORTH | encoding::SOUTH)); }

			/* use it like an iterator over directions: */
			inline static direction begin() { return encoding::NORTH; }
			inline static direction end() { return encoding::END; }

			inline direction& operator++() { value <<= 1; return *this; }
			inline direction operator++(int) { direction c = *this; value <<= 1; return c; }

			inline std::strong_ordering operator<=>(const direction& another) const { return value <=> another.value; }

			/** Conversion to underlying integer type */
			inline int_type get() const noexcept { return value; }
			inline operator int_type() const noexcept { return value; }

			inline char to_char() const {
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

			inline operator std::string() const {
				char x = to_char();
				return std::string(&x, 1);
			}

			/** Returns the opposite direction. */
			inline direction operator!() const noexcept { return direction(direction_invert_array[value]); }

		};
	}

	namespace v1_1 {

		using direction = tobor::v1_0::direction;
	}
}