#pragma once

#include "models/errors.h"
#include "models/direction.h"
#include "models/wall.h"
#include "models/legacy_world.h"
#include "models/redundant_cell_id.h"
#include "models/pieces_quantity.h"
#include "models/positions_of_pieces.h"
#include "models/piece_id.h"


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


		using default_legacy_world = legacy_world<std::size_t>;

		template<uint8_t COUNT_TARGET_PIECES_V, uint8_t COUNT_NON_TARGET_PIECES_V>
		using uint8_t_pieces_quantity = pieces_quantity<uint8_t, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		using default_pieces_quantity = pieces_quantity<uint8_t, 1, 3>;

		using default_piece_id = piece_id<default_pieces_quantity>;

		using default_cell_id = redundant_cell_id<default_legacy_world>;

		using default_positions_of_pieces = positions_of_pieces<default_pieces_quantity, default_cell_id, false, true>;


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
