#pragma once

#include "pieces_quantity.h"
#include "redundant_cell_id.h"

#include <array>
#include <functional>
#include <algorithm>



namespace tobor {

	namespace v1_0 {

		/**
		*	@brief Contains the information where the pieces are located on the game board.
		*/
		template <class Pieces_Quantity_Type = default_pieces_quantity, class Cell_Id_Type_T = default_cell_id, bool SORTED_TARGET_PIECES_V = true, bool SORTED_NON_TARGET_PIECES_V = true>
		class positions_of_pieces {
		public:

			template <class INNER_Pieces_Quantity_Type, class INNER_Cell_Id_Type, bool INNER_SORTED_TARGET_PIECES_V, bool INNER_SORTED_NON_TARGET_PIECES_V>
			friend void ::std::swap(
				positions_of_pieces<INNER_Pieces_Quantity_Type, INNER_Cell_Id_Type, INNER_SORTED_TARGET_PIECES_V, INNER_SORTED_NON_TARGET_PIECES_V>&,
				positions_of_pieces<INNER_Pieces_Quantity_Type, INNER_Cell_Id_Type, INNER_SORTED_TARGET_PIECES_V, INNER_SORTED_NON_TARGET_PIECES_V>&
			);

			using pieces_quantity_type = Pieces_Quantity_Type;

			using cell_id_type = Cell_Id_Type_T;

			using world_type = typename cell_id_type::world_type;

			using pieces_quantity_int_type = typename pieces_quantity_type::int_type;

			static constexpr pieces_quantity_int_type COUNT_TARGET_PIECES{ Pieces_Quantity_Type::COUNT_TARGET_PIECES };

			static constexpr pieces_quantity_int_type COUNT_NON_TARGET_PIECES{ Pieces_Quantity_Type::COUNT_NON_TARGET_PIECES };

			static constexpr pieces_quantity_int_type COUNT_ALL_PIECES{ Pieces_Quantity_Type::COUNT_ALL_PIECES };

			static constexpr bool SORTED_TARGET_PIECES{ SORTED_TARGET_PIECES_V };

			static constexpr bool SORTED_NON_TARGET_PIECES{ SORTED_NON_TARGET_PIECES_V };

			using target_pieces_array_type = std::array<cell_id_type, COUNT_TARGET_PIECES>;

			using non_target_pieces_array_type = std::array<cell_id_type, COUNT_NON_TARGET_PIECES>;

			using all_pieces_array_type = std::array<cell_id_type, COUNT_ALL_PIECES>;

		private:

			/**
			*	@brief Cell ids of the target piece(s) and non-target piece.
			*	@details Both sections {TARGET_PIECES : NON_TARGET_PIECES} need to be ordered by < all the time if specified so by template arguments.
			*
			*/
			all_pieces_array_type _piece_positions;


			template <class T>
			using bucket = std::array<std::vector<T>, 256>; // ### make 256 template in future version.

			template<class T>
			class bucket_iterator {

				typename bucket<T>::iterator _outer_iter;
				const typename bucket<T>::iterator _outer_iter_end;
				typename bucket<T>::value_type::iterator _inner_iter;

				inline void advance_until_valid() {
					if (_outer_iter == _outer_iter_end)
						return;

					while (_inner_iter == _outer_iter->end()) {
						++_outer_iter;
						if (_outer_iter == _outer_iter_end) {
							return; // reached end iterator
						}
						// jump to beginning of next bucket:
						_inner_iter = _outer_iter->begin();
					}
				}

				bucket_iterator(const typename bucket<T>::iterator& outer_iter, const typename bucket<T>::iterator& outer_iter_end, const typename bucket<T>::value_type::iterator& inner_iter) : _outer_iter(outer_iter), _outer_iter_end(outer_iter_end), _inner_iter(inner_iter) {
					advance_until_valid();
				}

			public:

				inline static bucket_iterator begin_of(bucket<T>& b) {
					return bucket_iterator(b.begin(), b.end(), b[0].begin());
				}

				inline static bucket_iterator end_of(bucket<T>& b) {
					return bucket_iterator(b.end(), b.end(), b[0].begin());
				}

				inline T& operator*() {
					return *_inner_iter;
				}

				inline bucket_iterator operator++() {
					// guaranteed that this is no end iterator.
					// guaranteed that inner is no end iterator;
					++_inner_iter;
					advance_until_valid();
					return *this;
				}

				inline bool is_end() const {
					return _outer_iter == _outer_iter_end;
				}

				inline bool operator==(const bucket_iterator another) {
					return (_outer_iter == another._outer_iter) && (
						is_end() || another.is_end() || _inner_iter == another._inner_iter
						);
				}

			};

			template<class Iterator_T>
			inline static void sort_in_buckets(std::array<std::vector<positions_of_pieces>, 256>& buckets, const Iterator_T& begin, const Iterator_T& end, const std::function<uint8_t(const positions_of_pieces&)>& get_bucket) {

				for (auto iter = begin; iter != end; ++iter) {
					const uint8_t value = get_bucket(*iter);
					if (buckets[value].empty() || !(buckets[value].back() == *iter)) {
						buckets[value].push_back(*iter);
					}
				}
			}

		public:


			template<class Collection_T>
			inline static void collection_sort_unique(Collection_T& c) {
				using int_cell_id_type = typename cell_id_type::int_cell_id_type;

				bucket<positions_of_pieces> old_buckets;
				bucket<positions_of_pieces> new_buckets;

				for (auto iter = std::cbegin(c); iter != std::cend(c); ++iter) {
					old_buckets[0].push_back(*iter);
				}
				c.clear();

				for (std::size_t i{ 0 }; i < COUNT_ALL_PIECES; ++i) {
					for (std::size_t j{ 0 }; j < sizeof(int_cell_id_type); ++j) {

						for (auto& element : new_buckets) {
							element.clear();
						}

						std::function<uint8_t(const positions_of_pieces&)> f = [&](const positions_of_pieces& p) -> uint8_t {
							const int_cell_id_type raw_id = p._piece_positions[static_cast<pieces_quantity_int_type>(COUNT_ALL_PIECES - i - 1)].get_id();
							return (raw_id >> (j * 8)) & 0xFF;
							};

						sort_in_buckets(new_buckets, bucket_iterator<positions_of_pieces>::begin_of(old_buckets), bucket_iterator<positions_of_pieces>::end_of(old_buckets), f);

						std::swap(old_buckets, new_buckets);

					}
				}

				for (auto iter = bucket_iterator<positions_of_pieces>::begin_of(old_buckets); iter != bucket_iterator<positions_of_pieces>::end_of(old_buckets); ++iter) {
					c.push_back(*iter);
				}

				// check is sorted here

				return;

			}

			template<class Iter>
			inline positions_of_pieces(Iter target_pieces_begin) {
				std::copy_n(target_pieces_begin, COUNT_ALL_PIECES, _piece_positions.begin());
				sort_pieces();
			}

			template<class Iter1, class Iter2>
			inline positions_of_pieces(Iter1 target_pieces_begin, Iter2 non_target_pieces_begin) {
				std::copy_n(target_pieces_begin, COUNT_TARGET_PIECES, _piece_positions.begin());
				std::copy_n(non_target_pieces_begin, COUNT_NON_TARGET_PIECES, _piece_positions.begin() + COUNT_TARGET_PIECES);
				sort_pieces();
			}

			/**
			*	@brief Creates an object when cell positions of the pieces are given.
			*	@param p_non_target_pieces Does not need to be sorted when passed to this constructor.
			*/
			positions_of_pieces(const target_pieces_array_type& target_pieces, const non_target_pieces_array_type& non_target_pieces) : positions_of_pieces(target_pieces.cbegin(), non_target_pieces.cbegin())
			{}

			positions_of_pieces(const positions_of_pieces&) = default;

			inline positions_of_pieces& operator = (const positions_of_pieces&) = default;

			positions_of_pieces(positions_of_pieces&&) = default;

			inline positions_of_pieces& operator = (positions_of_pieces&&) = default;

			inline all_pieces_array_type& piece_positions() { return _piece_positions; }

			inline const all_pieces_array_type& piece_positions() const { return _piece_positions; }

			bool operator< (const positions_of_pieces& another) const noexcept {
				return _piece_positions < another._piece_positions;
			}

			bool operator== (const positions_of_pieces& another) const noexcept {
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

			inline void sort_pieces() {
				if constexpr (SORTED_TARGET_PIECES && !(COUNT_TARGET_PIECES <= 1)) {
					std::sort(target_pieces_begin(), target_pieces_end());
				}
				if constexpr (SORTED_NON_TARGET_PIECES && !(COUNT_NON_TARGET_PIECES <= 1)) {
					std::sort(non_target_pieces_begin(), non_target_pieces_end());
				}
				// may be optimized for fixed array sizes
			}

			inline bool is_final(const cell_id_type& target_cell) const {
				for (auto iter = target_pieces_cbegin(); iter != target_pieces_cend(); ++iter) {
					if (*iter == target_cell)
						return true;
				}
				return false;
			}

			/**
			*	@brief Returns the number of pieces in which both states differ.
			*/
			inline std::size_t count_changed_pieces(const positions_of_pieces& another) const {
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
					// note: we only count the elements of *this which have no matching counterpart. We do not count for \p another.
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
		};

		using default_positions_of_pieces = positions_of_pieces<default_pieces_quantity, default_cell_id, false, true>;

	}
	namespace v1_1 {

		/**
		*	@brief Contains the information where the pieces are located on the game board.
		*/
		template <class Pieces_Quantity_T, class Cell_Id_Type_T, bool SORTED_TARGET_PIECES_V, bool SORTED_NON_TARGET_PIECES_V>
		using positions_of_pieces = tobor::v1_0::positions_of_pieces<Pieces_Quantity_T, Cell_Id_Type_T, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>;

	}
}