#pragma once

#include "models/errors.h"
#include "models/direction.h"
#include "models/wall.h"
#include "models/legacy_world.h"
#include "models/redundant_cell_id.h"
#include "models/pieces_quantity.h"


#include <map>
#include <set>
#include <array>
#include <vector>

#include <algorithm>
#include <utility>

#include <iterator>
#include <string>
#include <execution>
#include <compare>
#include <functional>


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

		/**
		*	@brief Wrapper for an integer to select one of the pieces on the board.
		*/
		template <class Pieces_Quantity_Type = default_pieces_quantity>
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

		using default_piece_id = piece_id<>;

		/*
		*	@brief Equivalent to a pair of a piece_id and a direction where to move it.
		*
		*	@details Does not define how piece_id is interpreted.
		*/
		template<class Piece_Id_Type = default_piece_id>
		struct piece_move {
		public:

			using piece_id_type = Piece_Id_Type;

			using pieces_quantity_type = typename piece_id_type::pieces_quantity_type;

			piece_id_type pid;
			direction dir;

			piece_move(const piece_id_type& pid, const direction& dir) : pid(pid), dir(dir) {}

			piece_move() : pid(0), dir(direction::NORTH()) {}

			piece_move(const piece_move&) = default;

			piece_move& operator=(const piece_move&) = default;

			piece_move(piece_move&&) = default;

			piece_move& operator=(piece_move&&) = default;

			inline bool operator<(const piece_move& another) const {
				return pid == another.pid ?
					dir < another.dir :
					pid < another.pid;
			}

			inline bool operator==(const piece_move& another) const {
				return pid == another.pid && dir == another.dir;
			}
		};

		using default_piece_move = piece_move<>;

		template<class Piece_Move_Type = default_piece_move>
		class move_path {

		public:
			using piece_move_type = Piece_Move_Type;

			using vector_type = std::vector<piece_move_type>;

			using pieces_quantity_type = typename piece_move_type::pieces_quantity_type;

		private:
			vector_type _move_vector;

		public:

			move_path() {}

			move_path(std::size_t n) : _move_vector(n, piece_move_type()) {}

			move_path(const move_path&) = default;

			move_path& operator=(const move_path&) = default;

			move_path(move_path&&) = default;

			move_path& operator=(move_path&&) = default;

			vector_type& vector() { return _move_vector; }

			const vector_type& vector() const { return _move_vector; }

			inline move_path operator +(const move_path& another) {
				move_path copy;
				copy._move_vector.reserve(_move_vector.size() + another._move_vector.size());
				std::copy(_move_vector.cbegin(), _move_vector.cend(), std::back_inserter(copy._move_vector));
				std::copy(another._move_vector.cbegin(), another._move_vector.cend(), std::back_inserter(copy._move_vector));
				return copy;
			}

			inline bool operator==(const move_path& another) const {
				return _move_vector == another._move_vector;
			}

			inline bool operator<(const move_path& another) const {
				return _move_vector < another._move_vector;
			}

			inline std::vector<move_path> syntactic_interleaving_neighbours() {
				if (_move_vector.size() < 2) {
					return std::vector<move_path>();
				}

				auto result = std::vector<move_path>(_move_vector.size() - 1, *this);
				auto iter = result.begin();
				for (std::size_t i{ 0 }; i + 1 < _move_vector.size(); ++i) {
					if (!(_move_vector[i] == _move_vector[i + 1])) {
						std::swap(iter->_move_vector[i], iter->_move_vector[i + 1]);
						++iter;
					}
				}
				result.erase(iter, result.end());

				return result;
			}

			inline move_path color_sorted_footprint() const {
				auto result = move_path(*this);

				std::stable_sort(
					result.vector().begin(),
					result.vector().end(),
					[](const piece_move_type& left, const piece_move_type& right) { return left.pid < right.pid; }
				);

				return result;
			}

			inline bool is_interleaving_neighbour(const move_path& another) const {
				if (vector().size() != another.vector().size()) {
					return false;
				}

				typename vector_type::size_type i{ 0 };

				while (i + 1 < vector().size()) { // looking for the switched positions i, i+1

					if (!(vector()[i] == another.vector()[i])) {
						// here it must be switched i, i+1 and the rest must be equal to return true...

						return
							vector()[i] == another.vector()[i + 1] &&
							vector()[i + 1] == another.vector()[i] &&
							std::equal(
								vector().cbegin() + i + 2,
								vector().cend(),
								another.vector().cbegin() + i + 2
							);
					}

					++i;
				}
				return false;
			}

			inline static std::vector<std::vector<move_path>> interleaving_partitioning_improved(const std::vector<move_path>& paths) {
				std::vector<std::vector<move_path>> equivalence_classes;

				using pair_type = std::pair<move_path, uint8_t>; // divide this into two vectors(?)

				using flagged_paths_type = std::vector<pair_type>;
				using flagged_paths_iterator = typename flagged_paths_type::iterator;


				//static constexpr uint8_t EXPLORED{ 0b10 };
				static constexpr uint8_t REACHED{ 0b01 };

				flagged_paths_type flagged_paths;
				flagged_paths.reserve(paths.size());
				std::transform(paths.cbegin(), paths.cend(), std::back_inserter(flagged_paths), [](const move_path& mp) { return std::make_pair(mp, 0); });

				std::sort(flagged_paths.begin(), flagged_paths.end()); // lexicographical sorting of paths by piece_id, then direction.

				flagged_paths_iterator remaining_end{ flagged_paths.end() };

				while (flagged_paths.begin() != remaining_end) { // while there are path not yet put into some equivalence class
					std::size_t diff = remaining_end - flagged_paths.begin();
					(void)diff;
					equivalence_classes.emplace_back();
					auto& equiv_class{ equivalence_classes.back() };
					equiv_class.reserve(remaining_end - flagged_paths.begin());

					flagged_paths.front().second = REACHED;
					equiv_class.push_back(flagged_paths.front().first);

					std::set<std::size_t> indices_to_explore;

					indices_to_explore.insert(0);

					while (!indices_to_explore.empty()) {

						std::size_t current_exploration_index = *indices_to_explore.cbegin();
						indices_to_explore.erase(indices_to_explore.cbegin());

						std::vector<move_path> neighbour_candidates = flagged_paths[current_exploration_index].first.syntactic_interleaving_neighbours();

						std::sort(neighbour_candidates.begin(), neighbour_candidates.end()); // lex sorting of move paths.

						flagged_paths_iterator search_begin{ flagged_paths.begin() };

						for (auto& candidate : neighbour_candidates) {

							search_begin = std::lower_bound( // find in sorted vector
								search_begin,
								remaining_end,
								std::make_pair(candidate, std::size_t(0)),
								[](const auto& l, const auto& r) {
									return l.first < r.first;
								}
							);

							if (search_begin == remaining_end) {
								break;
							}

							if (search_begin->first == candidate && !(search_begin->second & REACHED)) {
								// if found candidate and not reached before

								equiv_class.emplace_back(candidate);
								search_begin->second |= REACHED;
								indices_to_explore.insert(search_begin - flagged_paths.begin());
							}

						}
					}

					remaining_end = std::remove_if(
						flagged_paths.begin(),
						remaining_end,
						[](const pair_type& pair) {
							return pair.second & REACHED;
						}
					);
					equiv_class.shrink_to_fit();
				}
				if (paths.size() != flagged_paths.size()) {
					auto x = paths.size() - flagged_paths.size();
					(void)x;
				}

				return equivalence_classes;
			}

			inline static std::vector<std::vector<move_path>> interleaving_partitioning(const std::vector<move_path>& paths) {

				static constexpr bool USE_IMPROVEMENT{ true };

				if constexpr (USE_IMPROVEMENT) {
					return interleaving_partitioning_improved(paths);
				}
				else {
					std::vector<std::vector<move_path>> equivalence_classes;

					for (const auto& p : paths) {

						std::vector<std::size_t> indices; // all indices of matching equivalence classes
						for (std::size_t i{ 0 }; i < equivalence_classes.size(); ++i) {
							auto& ec{ equivalence_classes[i] };
							for (const auto& el : ec) {
								if (el.is_interleaving_neighbour(p)) {
									indices.push_back(i);
									break;
								}
							}
						}

						if (indices.empty()) {
							equivalence_classes.emplace_back();
							equivalence_classes.back().push_back(p);
						}
						else {
							equivalence_classes[indices[0]].emplace_back(p);
							for (std::size_t j = indices.size() - 1; j != 0; --j) {
								std::copy(
									equivalence_classes[indices[j]].cbegin(),
									equivalence_classes[indices[j]].cend(),
									std::back_inserter(equivalence_classes[indices[0]])
								);
								equivalence_classes.erase(equivalence_classes.begin() + indices[j]);
							}
						}
					}
					return equivalence_classes;
				}
			}

			std::size_t changes() const {
				std::size_t counter{ 0 };
				for (std::size_t i = 0; i + 1 < _move_vector.size(); ++i) {
					counter += !(_move_vector[i].pid == _move_vector[i + 1].pid);
				}
				return counter;
			}

			inline static bool antiprettiness_relation(const move_path& l, const move_path& r) {
				return l.changes() < r.changes();
			}

		};
	}
}

namespace std {

	template <class Pieces_Quantity_Type, class Cell_Id_Type_T, bool SORTED_TARGET_PIECES_V, bool SORTED_NON_TARGET_PIECES_V>
	inline void swap(
		tobor::v1_0::positions_of_pieces<Pieces_Quantity_Type, Cell_Id_Type_T, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>& a,
		tobor::v1_0::positions_of_pieces<Pieces_Quantity_Type, Cell_Id_Type_T, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>& b
	) {
		std::swap(a._piece_positions, b._piece_positions);
	}
}
