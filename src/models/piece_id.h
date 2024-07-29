#pragma once


namespace tobor {

	namespace v1_0 {

		/**
		*	@brief Wrapper for an integer to select one of the pieces on the board.
		*/
		template <class Pieces_Quantity_Type>
		struct piece_id {

		public:

			using pieces_quantity_type = Pieces_Quantity_Type;

			using int_type = typename pieces_quantity_type::int_type;

			int_type value;

			piece_id(int_type v) : value(v) {}

			piece_id() : value(0) {}

			inline static piece_id begin() { return piece_id(0); }

			inline static piece_id end() { return piece_id(pieces_quantity_type::COUNT_ALL_PIECES); }

			inline bool operator < (const piece_id& another) const { return value < another.value; }

			inline bool operator == (const piece_id& another) const { return value == another.value; }

			inline piece_id& operator++() { ++value; return *this; }

			inline piece_id operator++(int) { const piece_id copy{ *this }; ++value; return copy; }

		};
	}
}
