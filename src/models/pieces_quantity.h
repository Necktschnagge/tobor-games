#pragma once


namespace tobor {
	namespace v1_0 {

		/**
		*	@brief Stores the number of target pieces and non-target pieces statically.
		*/
		template<class Int_Type_T, Int_Type_T COUNT_TARGET_PIECES_V, Int_Type_T COUNT_NON_TARGET_PIECES_V>
		struct pieces_quantity {
			using int_type = Int_Type_T;
			static constexpr int_type COUNT_TARGET_PIECES{ COUNT_TARGET_PIECES_V };
			static constexpr int_type COUNT_NON_TARGET_PIECES{ COUNT_NON_TARGET_PIECES_V };
			static constexpr int_type COUNT_ALL_PIECES{ COUNT_TARGET_PIECES + COUNT_NON_TARGET_PIECES };

			static_assert(std::is_unsigned<int_type>::value, "pieces_quantity integer type must be unsigned.");
			static_assert(std::is_signed<int_type>::value == false, "size integer type is unsigned so that overflow does not have undefined behavior.");

			static_assert(COUNT_TARGET_PIECES >= 1, "positions_of_pieces: condition: at least one target piece");
			static_assert(COUNT_ALL_PIECES >= COUNT_TARGET_PIECES, "positions_of_pieces: condition: no sum overflow");
			static_assert(COUNT_ALL_PIECES > COUNT_NON_TARGET_PIECES, "positions_of_pieces: condition: no sum overflow");
		};

		template<uint8_t COUNT_TARGET_PIECES_V, uint8_t COUNT_NON_TARGET_PIECES_V>
		using uint8_t_pieces_quantity = pieces_quantity<uint8_t, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		using default_pieces_quantity = pieces_quantity<uint8_t, 1, 3>;

	}

	namespace v1_1 {
		
		/**
		*	@brief Stores the number of target pieces and non-target pieces statically.
		*/
		template<class Int_Type_T, Int_Type_T COUNT_TARGET_PIECES_V, Int_Type_T COUNT_NON_TARGET_PIECES_V>
		using pieces_quantity = tobor::v1_0::pieces_quantity<Int_Type_T, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		template<uint8_t COUNT_TARGET_PIECES_V, uint8_t COUNT_NON_TARGET_PIECES_V>
		using uint8_t_pieces_quantity = pieces_quantity<uint8_t, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		using default_pieces_quantity = pieces_quantity<uint8_t, 1, 3>;

	}
}