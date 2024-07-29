#pragma once

#include "models_1_0.h"

#include "models/id_polarisation.h"
#include "models/pieces_quantity.h"
#include "models/dynamic_rectangle_world.h"
#include "models/min_size_cell_id.h"

#include <compare>

namespace tobor {
	namespace v1_1 {

		using default_min_size_cell_id = min_size_cell_id<default_dynamic_rectangle_world>;

		template<uint8_t COUNT_TARGET_PIECES_V, uint8_t COUNT_NON_TARGET_PIECES_V>
		using uint8_t_pieces_quantity = pieces_quantity<uint8_t, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		using default_pieces_quantity = pieces_quantity<uint8_t, 1, 3>;

		using default_positions_of_pieces = positions_of_pieces<default_pieces_quantity, default_min_size_cell_id, false, true>;

		/**
		*	@brief Contains the information where the pieces are located on the game board.
		*
		*	@details Whether or not a section is sorted, it keeps track of the permutation which was applied in order to sort the pieces.
		*/
		template <class Pieces_Quantity_T, class Cell_Id_Type_T, bool SORTED_TARGET_PIECES_V, bool SORTED_NON_TARGET_PIECES_V>
		class augmented_positions_of_pieces {

			template <class INNER_Pieces_Quantity_Type, class INNER_Cell_Id_Type_T, bool INNER_SORTED_TARGET_PIECES_V, bool INNER_SORTED_NON_TARGET_PIECES_V>
			friend void ::std::swap(
				augmented_positions_of_pieces<INNER_Pieces_Quantity_Type, INNER_Cell_Id_Type_T, INNER_SORTED_TARGET_PIECES_V, INNER_SORTED_NON_TARGET_PIECES_V>&,
				augmented_positions_of_pieces<INNER_Pieces_Quantity_Type, INNER_Cell_Id_Type_T, INNER_SORTED_TARGET_PIECES_V, INNER_SORTED_NON_TARGET_PIECES_V>&
			);

		public:

			using pieces_quantity_type = Pieces_Quantity_T;

			using cell_id_type = Cell_Id_Type_T;

			using world_type = typename cell_id_type::world_type;

			using piece_id_int_type = typename pieces_quantity_type::int_type;



			static constexpr piece_id_int_type COUNT_TARGET_PIECES{ Pieces_Quantity_T::COUNT_TARGET_PIECES };

			static constexpr piece_id_int_type COUNT_NON_TARGET_PIECES{ Pieces_Quantity_T::COUNT_NON_TARGET_PIECES };

			static constexpr piece_id_int_type COUNT_ALL_PIECES{ Pieces_Quantity_T::COUNT_ALL_PIECES };

			static constexpr bool SORTED_TARGET_PIECES{ SORTED_TARGET_PIECES_V };

			static constexpr bool SORTED_NON_TARGET_PIECES{ SORTED_NON_TARGET_PIECES_V };



			using target_pieces_array_type = std::array<cell_id_type, COUNT_TARGET_PIECES>;

			using non_target_pieces_array_type = std::array<cell_id_type, COUNT_NON_TARGET_PIECES>;

			using all_pieces_array_type = std::array<cell_id_type, COUNT_ALL_PIECES>;

			using permutation_type = std::array<std::size_t, COUNT_ALL_PIECES>;

			using naked_type = positions_of_pieces<pieces_quantity_type, cell_id_type, SORTED_TARGET_PIECES, SORTED_NON_TARGET_PIECES>;

		private:

			/**
			*	@brief Cell ids of the target piece(s) and non-target piece.
			*	@details Both sections {TARGET_PIECES : NON_TARGET_PIECES} need to be ordered by < all the time if specified so by template arguments.
			*
			*/
			all_pieces_array_type _piece_positions;

			permutation_type _permutation;

			inline static void reset_perm(permutation_type& perm) {
				for (std::size_t i = 0; i < COUNT_ALL_PIECES; ++i)
					perm[i] = i;
			}

			template<class Aggregation_Type1, class Aggregation_Type2>
			inline static void apply_perm(const Aggregation_Type1& p, Aggregation_Type2& target) {
				auto update = Aggregation_Type2(target);
				for (std::size_t i{ 0 }; i < p.size(); ++i) {
					update[i] = target[p[i]];
				}
				target = update;
			}
		public:

			inline all_pieces_array_type& piece_positions() { return _piece_positions; }

			inline const all_pieces_array_type& piece_positions() const { return _piece_positions; }

			inline const permutation_type& permutation() const { return _permutation; }

			inline augmented_positions_of_pieces& reset_permutation() {
				reset_perm(_permutation);
				return *this;
			}

			template<class Iter>
			inline augmented_positions_of_pieces(Iter target_pieces_begin) {
				std::copy_n(target_pieces_begin, COUNT_ALL_PIECES, _piece_positions.begin());
				reset_permutation();
				sort_pieces();
			}

			template<class Iter1, class Iter2>
			inline augmented_positions_of_pieces(Iter1 target_pieces_begin, Iter2 non_target_pieces_begin) {
				std::copy_n(target_pieces_begin, COUNT_TARGET_PIECES, _piece_positions.begin());
				std::copy_n(non_target_pieces_begin, COUNT_NON_TARGET_PIECES, _piece_positions.begin() + COUNT_TARGET_PIECES);
				reset_permutation();
				sort_pieces();
			}

			/**
			*	@brief Creates an object when cell positions of the pieces are given.
			*	@param p_non_target_pieces Does not need to be sorted when passed to this constructor.
			*/
			augmented_positions_of_pieces(const target_pieces_array_type& target_pieces, const non_target_pieces_array_type& non_target_pieces) : augmented_positions_of_pieces(target_pieces.cbegin(), non_target_pieces.cbegin())
			{}

			augmented_positions_of_pieces(const naked_type& pop) : augmented_positions_of_pieces(pop.target_pieces_cbegin())
			{}

			augmented_positions_of_pieces(const augmented_positions_of_pieces&) = default;

			inline augmented_positions_of_pieces& operator = (const augmented_positions_of_pieces&) = default;

			augmented_positions_of_pieces(augmented_positions_of_pieces&&) = default;

			inline augmented_positions_of_pieces& operator = (augmented_positions_of_pieces&&) = default;

			bool operator< (const augmented_positions_of_pieces& another) const noexcept {
				return _piece_positions < another._piece_positions;
			}

			bool operator== (const augmented_positions_of_pieces& another) const noexcept {
				return _piece_positions == another._piece_positions;
			}

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::const_iterator target_pieces_cbegin() const {
				return _piece_positions.cbegin();
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator target_pieces_begin() {
				return _piece_positions.begin();
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::const_iterator target_pieces_cend() const {
				return _piece_positions.cbegin() + COUNT_TARGET_PIECES;
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator target_pieces_end() {
				return _piece_positions.begin() + COUNT_TARGET_PIECES;
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::const_iterator non_target_pieces_cbegin() const {
				return target_pieces_cend();
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator non_target_pieces_begin() {
				return target_pieces_end();
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::const_iterator non_target_pieces_cend() const {
				return _piece_positions.cend();
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator non_target_pieces_end() {
				return _piece_positions.end();
			};


			inline void feedback_helper(const permutation_type&) {}

			template<class U, class ... T>
			inline void feedback_helper(const permutation_type& p, U& x, T& ... xs) {
				// need inverse of permutation.
				for (typename pieces_quantity_type::int_type i{ 0 }; i < p.size(); ++i) {
					if (p[i] == x) {
						x = i;
						break;
					}
				}
				feedback_helper(p, xs...);
			}


			template<class ... T>
			inline void sort_pieces(T&... piece_ids) {
				if constexpr ((!SORTED_TARGET_PIECES || COUNT_TARGET_PIECES <= 1) && (!SORTED_NON_TARGET_PIECES || COUNT_NON_TARGET_PIECES <= 1)) {
					((void)piece_ids, ...);
					return;
				}
				else {

					permutation_type p_new;
					reset_perm(p_new);
					if constexpr (SORTED_TARGET_PIECES && !(COUNT_TARGET_PIECES <= 1)) {
						//std::sort(target_pieces_begin(), target_pieces_end());
						std::sort(p_new.begin(), p_new.begin() + COUNT_TARGET_PIECES, [&](const std::size_t& l, const std::size_t& r) {
							return _piece_positions[l] < _piece_positions[r];
							});
					}
					if constexpr (SORTED_NON_TARGET_PIECES && !(COUNT_NON_TARGET_PIECES <= 1)) {
						std::sort(p_new.begin() + COUNT_TARGET_PIECES, p_new.begin() + COUNT_ALL_PIECES, [&](const std::size_t& l, const std::size_t& r) {
							return _piece_positions[l] < _piece_positions[r];
							});
					}
					apply_perm(p_new, _piece_positions);
					apply_perm(p_new, _permutation);

					feedback_helper(p_new, piece_ids...);
				}
			}

			template<class AggregationType>
			inline augmented_positions_of_pieces& apply_permutation(const AggregationType& permutation) {
				apply_perm(permutation, _permutation);
				return *this;
			}

			inline bool is_final(const cell_id_type& target_cell) const {
				for (auto iter = target_pieces_cbegin(); iter != target_pieces_cend(); ++iter) {
					if (*iter == target_cell)
						return true;
				}
				return false;
			}

			inline std::size_t count_changed_pieces(const augmented_positions_of_pieces& another) const {
				std::size_t counter{ 0 };
				if constexpr (SORTED_TARGET_PIECES) {
					auto iter = target_pieces_cbegin();
					auto jter = another.target_pieces_cbegin();
					while (iter != target_pieces_cend() && jter != another.target_pieces_cend()) {
						if (*iter == *jter) {
							++iter;
							++jter;
						}
						else if (*iter < *jter) {
							++iter;
							++counter;
						}
						else
							++jter;
					}
					counter += (target_pieces_cend() - iter);
				}
				else {
					for (
						auto iter = target_pieces_cbegin(), jter = another.target_pieces_cbegin();
						iter != target_pieces_cend();
						++iter, ++jter
						)
					{
						if (*iter != *jter)
							++counter;
					}
				}
				if constexpr (SORTED_NON_TARGET_PIECES) {
					auto iter = non_target_pieces_cbegin();
					auto jter = another.non_target_pieces_cbegin();
					while (iter != non_target_pieces_cend() && jter != another.non_target_pieces_cend()) {
						if (*iter == *jter) {
							++iter;
							++jter;
						}
						else if (*iter < *jter) {
							++iter;
							++counter;
						}
						else
							++jter;
					}
					counter += (non_target_pieces_cend() - iter);
				}
				else {
					for (
						auto iter = non_target_pieces_cbegin(), jter = another.non_target_pieces_cbegin();
						iter != non_target_pieces_cend();
						++iter, ++jter
						)
					{
						if (*iter != *jter)
							++counter;
					}
				}
				return counter;
			}

			inline naked_type naked() const {
				return naked_type(target_pieces_cbegin());
			}
		};

		using default_augmented_positions_of_pieces = augmented_positions_of_pieces< default_pieces_quantity, default_min_size_cell_id, false, false>;


		template <class Pieces_Quantity_T>
		using piece_id = tobor::v1_0::piece_id<Pieces_Quantity_T>;

		using default_piece_id = piece_id<default_pieces_quantity>;

		/*
		*	@brief Equivalent to a pair of a piece_id and a direction where to move it.
		*
		*	@details Does not define how piece_id is interpreted.
		*/
		template<class Piece_Id_Type>
		using piece_move = tobor::v1_0::piece_move<Piece_Id_Type>;

		using default_piece_move = piece_move<default_piece_id>;

	}
}

namespace std {
	template <class Pieces_Quantity_T, class Cell_Id_Type, bool SORTED_TARGET_PIECES_V, bool SORTED_NON_TARGET_PIECES_V>
	inline void swap(
		tobor::v1_1::augmented_positions_of_pieces<Pieces_Quantity_T, Cell_Id_Type, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>& a,
		tobor::v1_1::augmented_positions_of_pieces<Pieces_Quantity_T, Cell_Id_Type, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>& b
	) {
		std::swap(a._piece_positions, b._piece_positions);
		std::swap(a._permutation, b._permutation);
	}

}
