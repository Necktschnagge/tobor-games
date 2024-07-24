#pragma once

#include "models_1_0.h"

#include <compare>

namespace tobor {
	namespace v1_1 {

		using division_by_2_error = tobor::v1_0::division_by_2_error;

		using blocked_center_error = tobor::v1_0::blocked_center_error;

		using direction = tobor::v1_0::direction;

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

		using wall = tobor::v1_0::wall;

		using wall_vector = std::vector<wall>;

		/**
		*
		*	@brief Represents the game's entire board, stating which cells are separated by walls.
		*	@details Does NOT contain any information about where pieces are located.
		*
		*/
		template <class Int_Size_Type_T, class Int_Cell_Id_Type_T>
		class dynamic_rectangle_world {
		public:

			using int_size_type = Int_Size_Type_T;
			using int_cell_id_type = Int_Cell_Id_Type_T;

			using wall_vector_type = wall_vector;
			using type = dynamic_rectangle_world;

		private:

			int_size_type x_size;
			int_size_type y_size;

			/*
			* Contains all horizontal walls existing on the board.
			* order: start at (0,0) -> (0, y_size - 1), go line by line x=0 .. x_size - 1
			* At index i of this vector h_walls[i] you find the horizontal wall below the cell with transposed_id i.
			*/
			wall_vector_type h_walls;

			/*
			* Contains all vertical walls existing on the board.
			* order: start at (0,0) -> (x_size - 1, 0), go line by line y=0 .. y_size - 1
			* At index i of this vector v_walls[i] you find the vertical wall on the left of the cell with id i.
			*/
			wall_vector_type v_walls;

		public:

			static constexpr int_cell_id_type narrow(const int_size_type& x) {
				return static_cast<int_cell_id_type>(x);
			}

			static constexpr int_size_type wide(const int_cell_id_type& x) {
				return static_cast<int_size_type>(x);
			}

			/* ctors et. al. **************************************************************************************/

			dynamic_rectangle_world() : x_size(0), y_size(0) {}

			dynamic_rectangle_world(const int_size_type& x_size, const int_size_type& y_size) : dynamic_rectangle_world() {
				resize(x_size, y_size);
			}

			/**
			*	@brief Sets the size of the world, also creates an empty rectangle with walls only on the outer borders
			*/
			void resize(int_size_type _x_size, int_size_type _y_size) noexcept {
				this->x_size = _x_size;
				this->y_size = _y_size;
				{
					const wall_vector_type::size_type SIZE{ static_cast<std::size_t>(x_size) * y_size + 1 };

					h_walls = wall_vector_type(SIZE, false);
					v_walls = wall_vector_type(SIZE, false);
				}

				for (int_size_type i = 0; i <= x_size; ++i) { // set north and south walls
					h_walls[y_size * i] = true;
				}
				for (int_size_type i = 0; i <= y_size; ++i) { // set east and west walls
					v_walls[x_size * i] = true;
				}
			}

			/*
				@brief Fills center cells to make them unreachable.
			*/
			inline void block_center_cells(int_size_type x_blocked_size, int_size_type y_blocked_size) {
				// check for symmetry of blocked area:
				if ((x_size - x_blocked_size) % 2)
					throw division_by_2_error();
				if ((y_size - y_blocked_size) % 2)
					throw division_by_2_error();

				// check for non-blocked cells in every direction
				if (x_blocked_size >= x_size)
					throw blocked_center_error();
				if (y_blocked_size >= y_size)
					throw blocked_center_error();

				const int_size_type x_begin{ static_cast<int_size_type>((x_size - x_blocked_size) / 2) };
				const int_size_type x_end{ static_cast<int_size_type>(x_begin + x_blocked_size) };
				const int_size_type y_begin{ static_cast<int_size_type>((y_size - y_blocked_size) / 2) };
				const int_size_type y_end{ static_cast<int_size_type>(y_begin + y_blocked_size) };

				for (auto y = y_begin; y != y_end; ++y) {
					for (auto x = x_begin; x != x_end; ++x) {
						const int_cell_id_type x_narrow{ narrow(x) };
						const int_cell_id_type y_narrow{ narrow(y) };

						const int_cell_id_type id = coordinates_to_cell_id(x_narrow, y_narrow);
						const int_cell_id_type transposed_id = coordinates_to_transposed_cell_id(x_narrow, y_narrow);

						north_wall_by_transposed_id(transposed_id) = true;
						east_wall_by_id(id) = true;
						south_wall_by_transposed_id(transposed_id) = true;
						west_wall_by_id(id) = true;
						// remark: this is by the way inefficient, but readable
					}
				}
			}

			/* cell id conversion **************************************************************************************/

			inline constexpr int_cell_id_type coordinates_to_cell_id(int_cell_id_type x_coord, int_cell_id_type y_coord) const noexcept {
				return narrow(x_size * y_coord + x_coord);
			}

			inline constexpr int_cell_id_type coordinates_to_transposed_cell_id(int_cell_id_type x_coord, int_cell_id_type y_coord) const noexcept {
				return narrow(y_size * x_coord + y_coord);
			}

			inline constexpr std::pair<int_cell_id_type, int_cell_id_type> cell_id_to_coordinates(int_cell_id_type id) const noexcept {
				return std::make_pair(id % x_size, id / x_size);
			}

			inline constexpr void cell_id_to_coordinates(int_cell_id_type id, int_cell_id_type& x_coord, int_cell_id_type& y_coord) const noexcept {
				x_coord = id % x_size;
				y_coord = id / x_size;
			}

			inline constexpr std::pair<int_cell_id_type, int_cell_id_type> transposed_cell_id_to_coordinates(int_cell_id_type transposed_id) const noexcept {
				return std::make_pair(transposed_id / y_size, transposed_id % y_size);
			}

			inline constexpr void transposed_cell_id_to_coordinates(int_cell_id_type transposed_id, int_cell_id_type& x_coord, int_cell_id_type& y_coord) const noexcept {
				x_coord = transposed_id / y_size;
				y_coord = transposed_id % y_size;
			}

			inline constexpr int_cell_id_type cell_id_to_transposed_cell_id(int_cell_id_type id) const noexcept {
				return narrow(y_size * (id % x_size) + id / x_size);
			}

			inline constexpr int_cell_id_type transposed_cell_id_to_cell_id(int_cell_id_type transposed_id) const noexcept {
				return narrow(x_size * (transposed_id % y_size) + transposed_id / y_size);
			}

			/* wall accessors **************************************************************************************/

			inline wall& south_wall_by_transposed_id(int_cell_id_type transposed_id) noexcept {
				return h_walls[transposed_id];
			}

			inline const wall& south_wall_by_transposed_id(int_cell_id_type transposed_id) const noexcept {
				return h_walls[transposed_id];
			}

			inline wall& north_wall_by_transposed_id(int_cell_id_type transposed_id) noexcept {
				return h_walls[wide(transposed_id) + 1];
			}

			inline const wall& north_wall_by_transposed_id(int_cell_id_type transposed_id) const noexcept {
				return h_walls[wide(transposed_id) + 1];
			}

			inline wall& west_wall_by_id(int_cell_id_type id) noexcept {
				return v_walls[id];
			}

			inline const wall& west_wall_by_id(int_cell_id_type id) const noexcept {
				return v_walls[id];
			}

			inline wall& east_wall_by_id(int_cell_id_type id) noexcept {
				return v_walls[wide(id) + 1];
			}

			inline const wall& east_wall_by_id(int_cell_id_type id) const noexcept {
				return v_walls[wide(id) + 1];
			}

			/* getter **************************************************************************************/

			/**
			*	@brief Returns the board's number of cells, including totally blocked ones.
			*/
			inline int_size_type count_cells() const noexcept {
				return x_size * y_size;
			}

			/**
			*	@brief Return true if and only if given cell is blocked.
			*	@details cell_id must be a non-negative integer less than \a count_cells(). Otherwise behaviour is undefined.
			*/
			inline bool blocked(int_cell_id_type cell_id) const {
				auto transposed_id = cell_id_to_transposed_cell_id(cell_id);
				return west_wall_by_id(cell_id) && east_wall_by_id(cell_id) && south_wall_by_transposed_id(transposed_id) && north_wall_by_transposed_id(transposed_id);
			}

			/**
			*	@brief Returns the board's number of blocked cells
			*/
			inline int_size_type blocked_cells() const noexcept {
				int_size_type counter{ 0 };
				for (int_size_type cell_id = 0; cell_id < count_cells(); ++cell_id) {
					counter += blocked(narrow(cell_id));
				}
				return counter;
			}

			/**
			*	@brief Returns the board's number of cells in a row.
			*/
			inline int_size_type get_horizontal_size() const noexcept {
				return x_size;
			}

			/**
			*	@brief Returns the board's number of cells in a column.
			*/
			inline int_size_type get_vertical_size() const noexcept {
				return y_size;
			}

			type turn_left_90() const { // only for quadratic
				if (x_size != y_size) {
					throw std::logic_error("Cannot turn for non-quadratic board.");
				}
				auto copy = type(x_size, y_size);

				for (int_size_type big_id = 0; big_id < x_size; ++big_id) {
					for (int_size_type little_id = 0; little_id < x_size; ++little_id) {
						copy.v_walls[big_id * x_size + little_id] = h_walls[big_id * x_size + (x_size - little_id)];
						copy.h_walls[big_id * x_size + little_id] = v_walls[((x_size - 1) - big_id) * x_size + little_id];
					}
				}
				return copy;
			}

		};

		using default_dynamic_rectangle_world = dynamic_rectangle_world<std::size_t, std::size_t>;

		/**
		*	@brief Stores just a cell id.
		*/
		template<class World_T>
		class min_size_cell_id {
		public:

			using world_type = World_T;

			using int_size_type = typename world_type::int_size_type; // narrow_int
			using int_cell_id_type = typename world_type::int_cell_id_type; // size_int

			using type = min_size_cell_id<world_type>;

			/* static factory member functions */

			inline static type create_by_coordinates(int_cell_id_type p_x_coord, int_cell_id_type p_y_coord, const world_type& world) noexcept {
				return type(world.coordinates_to_cell_id(p_x_coord, p_y_coord));
			}

			inline static type create_by_id(int_cell_id_type p_id) noexcept {
				return type(p_id);
			}

			inline static type create_by_id(int_cell_id_type p_id, const world_type&) noexcept {
				return create_by_id(p_id);
			}

			inline static type create_by_transposed_id(int_cell_id_type p_transposed_id, const world_type& world) noexcept {
				return type(world.transposed_cell_id_to_cell_id(p_transposed_id));
			}

			inline static type create_by_raw_id(const id_polarisation& p, int_cell_id_type p_raw_id, const world_type& world) noexcept {
				if (p) return create_by_transposed_id(p_raw_id, world);
				else return create_by_id(p_raw_id);
			}

		private:

			int_cell_id_type _id;

		public:

			/* ctors */

			min_size_cell_id() : _id(0) {}

			min_size_cell_id(int_cell_id_type id) : _id(id) {}

			min_size_cell_id(const min_size_cell_id&) = default;

			min_size_cell_id(min_size_cell_id&&) = default; // needed for static factory member function

			/* operator = */

			inline min_size_cell_id& operator = (const min_size_cell_id&) = default;

			inline min_size_cell_id& operator = (min_size_cell_id&&) = default;

			/* comparison operators */

			inline std::strong_ordering operator<=>(const min_size_cell_id& another) const = default;

			/* getter */

			inline int_cell_id_type get_id() const noexcept { return _id; }

			inline int_cell_id_type get_id(const world_type&) const noexcept { return _id; }

			inline int_cell_id_type get_transposed_id(const world_type& world) const noexcept { return world.cell_id_to_transposed_cell_id(_id); }

			inline int_cell_id_type get_x_coord(const world_type& world) const noexcept { return world.cell_id_to_coordinates(_id).first; }

			inline int_cell_id_type get_y_coord(const world_type& world) const noexcept { return world.cell_id_to_coordinates(_id).second; }

			inline int_cell_id_type get_raw_id(const id_polarisation& p, const world_type& world) const noexcept {
				if (p) return get_transposed_id(world);
				else return get_id();
			}

			/* modifiers */

			inline void set_id(int_cell_id_type p_id) noexcept {
				_id = p_id;
			}

			inline void set_id(int_cell_id_type p_id, const world_type&) noexcept {
				return set_id(p_id);
			}

			inline void set_transposed_id(int_cell_id_type p_transposed_id, const world_type& world) noexcept {
				_id = world.transposed_cell_id_to_cell_id(p_transposed_id);
			}

			inline void set_coord(int_cell_id_type p_x_coord, int_cell_id_type p_y_coord, const world_type& world) noexcept {
				_id = world.coordinates_to_cell_id(p_x_coord, p_y_coord);
			}

		};

		using default_min_size_cell_id = min_size_cell_id<default_dynamic_rectangle_world>;

		/**
		*	@brief Stores the number of target pieces and non-target pieces statically.
		*/
		template<class Int_Type_T, Int_Type_T COUNT_TARGET_PIECES_V, Int_Type_T COUNT_NON_TARGET_PIECES_V>
		using pieces_quantity = tobor::v1_0::pieces_quantity<Int_Type_T, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		template<uint8_t COUNT_TARGET_PIECES_V, uint8_t COUNT_NON_TARGET_PIECES_V>
		using uint8_t_pieces_quantity = pieces_quantity<uint8_t, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		using default_pieces_quantity = pieces_quantity<uint8_t, 1, 3>;


		/**
		*	@brief Contains the information where the pieces are located on the game board.
		*/
		template <class Pieces_Quantity_T, class Cell_Id_Type_T, bool SORTED_TARGET_PIECES_V, bool SORTED_NON_TARGET_PIECES_V>
		using positions_of_pieces = tobor::v1_0::positions_of_pieces<Pieces_Quantity_T, Cell_Id_Type_T, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>;

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
