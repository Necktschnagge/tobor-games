#pragma once

#include "models_1_0.h"

namespace tobor {
	namespace v1_1 {

		using division_by_2_error = tobor::v1_0::division_by_2_error;

		using blocked_center_error = tobor::v1_0::blocked_center_error;

		using direction = tobor::v1_0::direction;

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

		using wall_type = tobor::v1_0::wall_type;

		using wall_vector = std::vector<wall_type>;

		/**
		*
		*	@brief Represents the game's entire board, stating which fields are separated by walls.
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
				} // ### check for exception (? heap allocation, should anyway cause the app to terminate, ignore this?)

				for (int_size_type i = 0; i <= x_size; ++i) { // set north and south walls
					h_walls[y_size * i] = true;
				}
				for (int_size_type i = 0; i <= y_size; ++i) { // set east and west walls
					v_walls[x_size * i] = true;
				}
			}

			/*
				@brief Fills center fields to make them unreachable.
			*/
			inline void block_center_cells(int_size_type x_blocked_size, int_size_type y_blocked_size) {
				// check for symmetry of blocked area:
				if ((x_size - x_blocked_size) % 2)
					throw division_by_2_error();
				if ((y_size - y_blocked_size) % 2)
					throw division_by_2_error();

				// check for non-blocked fields in every direction
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
			// #### beginning from here check overflow for allowed inputs by some static asserts!!!
			inline constexpr int_cell_id_type coordinates_to_cell_id(int_cell_id_type x_coord, int_cell_id_type y_coord) const noexcept {
				return static_cast<int_cell_id_type>(x_size * y_coord + x_coord);
			}

			inline constexpr int_cell_id_type coordinates_to_transposed_cell_id(int_cell_id_type x_coord, int_cell_id_type y_coord) const noexcept {
				return static_cast<int_cell_id_type>(y_size * x_coord + y_coord);
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
				return static_cast<int_cell_id_type>(y_size * (id % x_size) + id / x_size);
			}

			inline constexpr int_cell_id_type transposed_cell_id_to_cell_id(int_cell_id_type transposed_id) const noexcept {
				return static_cast<int_cell_id_type>(x_size * (transposed_id % y_size) + transposed_id / y_size);
			}

			/* wall accessors **************************************************************************************/

			inline wall_type& south_wall_by_transposed_id(int_cell_id_type transposed_id) noexcept {
				return h_walls[transposed_id];
			}

			inline const wall_type& south_wall_by_transposed_id(int_cell_id_type transposed_id) const noexcept {
				return h_walls[transposed_id];
			}

			inline wall_type& north_wall_by_transposed_id(int_cell_id_type transposed_id) noexcept {
				return h_walls[transposed_id + 1]; // ### need casting to size_type
			}

			inline const wall_type& north_wall_by_transposed_id(int_cell_id_type transposed_id) const noexcept {
				return h_walls[transposed_id + 1]; // ### need casting to size_type
			}

			inline wall_type& west_wall_by_id(int_cell_id_type id) noexcept {
				return v_walls[id];
			}

			inline const wall_type& west_wall_by_id(int_cell_id_type id) const noexcept {
				return v_walls[id];
			}

			inline wall_type& east_wall_by_id(int_cell_id_type id) noexcept {
				return v_walls[id + 1]; // ### need casting to size_type
			}

			inline const wall_type& east_wall_by_id(int_cell_id_type id) const noexcept {
				return v_walls[id + 1]; // ### need casting to size_type
			}

			// ### consider: should calling these functions above be allowed for greater values ?, e.g. south wall of cell 256? -> no .... sure?

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
					counter += blocked(narrow(cell_id)); //## need to cast here, note loop must use greater type since for breaks on overflow of smaller type.
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
					throw std::logic_error("Cannot turn for non-quadratic world.");
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

		template<class World_Type_T>
		class min_size_cell_id {
		public:

			using world_type = World_Type_T;

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

			int_cell_id_type id;

		public:

			/* ctors */

			min_size_cell_id() : id(0) {}

			min_size_cell_id(int_cell_id_type id) : id(id) {}

			min_size_cell_id(const min_size_cell_id&) = default;

			min_size_cell_id(min_size_cell_id&&) = default; // needed for static factory member function

			/* operator = */

			inline min_size_cell_id& operator = (const min_size_cell_id&) = default;

			inline min_size_cell_id& operator = (min_size_cell_id&&) = default;

			/* comparison operators */

			inline bool operator < (const min_size_cell_id& other) const noexcept {
				return this->id < other.id;
			}

			inline bool operator == (const min_size_cell_id& other) const noexcept {
				return this->id == other.id;
			}

			/* getter */

			inline int_cell_id_type get_id() const noexcept { return id; }

			inline int_cell_id_type get_id(const world_type&) const noexcept { return id; }

			inline int_cell_id_type get_transposed_id(const world_type& world) const noexcept { return world.cell_id_to_transposed_cell_id(id); }

			inline int_cell_id_type get_x_coord(const world_type& world) const noexcept { return world.cell_id_to_coordinates(id).first; }

			inline int_cell_id_type get_y_coord(const world_type& world) const noexcept { return world.cell_id_to_coordinates(id).second; }

			inline int_cell_id_type get_raw_id(const id_polarisation& p, const world_type& world) const noexcept {
				if (p) return get_transposed_id(world);
				else return get_id();
			}

			/* modifiers */

			inline void set_id(int_cell_id_type p_id) noexcept {
				id = p_id;
			}

			inline void set_id(int_cell_id_type p_id, const world_type&) noexcept {
				return set_id(p_id);
			}

			inline void set_transposed_id(int_cell_id_type p_transposed_id, const world_type& world) noexcept {
				id = world.transposed_cell_id_to_cell_id(p_transposed_id);
			}

			inline void set_coord(int_cell_id_type p_x_coord, int_cell_id_type p_y_coord, const world_type& world) noexcept {
				id = world.coordinates_to_cell_id(p_x_coord, p_y_coord);
			}

		};

		using default_min_size_cell_id = min_size_cell_id<default_dynamic_rectangle_world>;

		template<class Int_Type_T, Int_Type_T COUNT_TARGET_PIECES_V, Int_Type_T COUNT_NON_TARGET_PIECES_V>
		using pieces_quantity = tobor::v1_0::pieces_quantity<Int_Type_T, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		template<uint8_t COUNT_TARGET_PIECES_V, uint8_t COUNT_NON_TARGET_PIECES_V>
		using uint8_t_pieces_quantity = pieces_quantity<uint8_t, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		using default_pieces_quantity = pieces_quantity<uint8_t, 1, 3>;


		/**
		*	@brief Contains the information where the pieces are located on the game board.
		*
		*	@details It only distinguishes the target piece from non target pieces.
		*			Non target pieces cannot be distiguished. They are kept sorted acending by their cell ids.
		*/
		template <class Pieces_Quantity_Type, class Cell_Id_Type_T, bool SORTED_TARGET_PIECES_V, bool SORTED_NON_TARGET_PIECES_V>
		using positions_of_pieces = tobor::v1_0::positions_of_pieces<Pieces_Quantity_Type, Cell_Id_Type_T, SORTED_TARGET_PIECES_V, SORTED_NON_TARGET_PIECES_V>;

		using default_positions_of_pieces = positions_of_pieces<default_pieces_quantity, default_min_size_cell_id, false, true>;

		/**
		*	@brief Contains the information where the pieces are located on the game board.
		*
		*	@details It only distinguishes the target piece from non target pieces.
		*			Non target pieces cannot be distiguished. They are kept sorted acending by their cell ids.
		*/
		template <class Pieces_Quantity_Type, class Cell_Id_Type_T, bool SORTED_TARGET_PIECES_V, bool SORTED_NON_TARGET_PIECES_V>
		class augmented_positions_of_pieces {

		public:

			using pieces_quantity_type = Pieces_Quantity_Type;

			using cell_id_type = Cell_Id_Type_T;

			using world_type = typename cell_id_type::world_type;

			// cell_id_type::int_type for cell ids


			// here it should not use a dependent name without alias-defining it in the class itself!
			// like piece_id_int_type, we already defined class piece_id as a wrapper.

			static constexpr typename pieces_quantity_type::int_type COUNT_TARGET_PIECES{ Pieces_Quantity_Type::COUNT_TARGET_PIECES };

			static constexpr typename pieces_quantity_type::int_type COUNT_NON_TARGET_PIECES{ Pieces_Quantity_Type::COUNT_NON_TARGET_PIECES };

			static constexpr typename pieces_quantity_type::int_type COUNT_ALL_PIECES{ Pieces_Quantity_Type::COUNT_ALL_PIECES };

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
			all_pieces_array_type piece_positions;

			permutation_type permutation;

			inline static void reset_perm(permutation_type& perm) {
				for (std::size_t i = 0; i < COUNT_ALL_PIECES; ++i)
					perm[i] = i;
			}

			template<class Aggregation_Type>
			inline static void apply_perm(const permutation_type& p, Aggregation_Type& target) {
				auto update = Aggregation_Type(target);
				for (std::size_t i{ 0 }; i < p.size(); ++i) {
					update[i] = target[p[i]];
				}
				target = update;
			}
		public:

			inline const permutation_type& get_permutation() const { return permutation; } // ### rename members!

			inline void reset_permutation() {
				reset_perm(permutation);
			}

			template<class Iter>
			inline augmented_positions_of_pieces(Iter1 target_pieces_begin) {
				std::copy_n(target_pieces_begin, COUNT_ALL_PIECES, piece_positions.begin());
				reset_permutation();
				sort_pieces();
			}

			template<class Iter1, class Iter2>
			inline augmented_positions_of_pieces(Iter1 target_pieces_begin, Iter2 non_target_pieces_begin) {
				std::copy_n(target_pieces_begin, COUNT_TARGET_PIECES, piece_positions.begin());
				std::copy_n(non_target_pieces_begin, COUNT_NON_TARGET_PIECES, piece_positions.begin() + COUNT_TARGET_PIECES);
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

			augmented_positions_of_pieces(augmented_positions_of_pieces&&) = default; //## need to make the other valid

			inline augmented_positions_of_pieces& operator = (augmented_positions_of_pieces&&) = default; //## need to make the other valid

			inline const all_pieces_array_type& raw() const { return piece_positions; }

			bool operator< (const augmented_positions_of_pieces& another) const noexcept {
				return piece_positions < another.piece_positions;
			}

			bool operator== (const augmented_positions_of_pieces& another) const noexcept {
				return piece_positions == another.piece_positions;
			}

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::const_iterator target_pieces_cbegin() const {
				return piece_positions.cbegin();
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator target_pieces_begin() {
				return piece_positions.begin();
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::const_iterator target_pieces_cend() const {
				return piece_positions.cbegin() + COUNT_TARGET_PIECES;
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator target_pieces_end() {
				return piece_positions.begin() + COUNT_TARGET_PIECES;
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::const_iterator non_target_pieces_cbegin() const {
				return target_pieces_cend();
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator non_target_pieces_begin() {
				return target_pieces_end();
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::const_iterator non_target_pieces_cend() const {
				return piece_positions.cend();
			};

			inline typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator non_target_pieces_end() {
				return piece_positions.end();
			};

			inline void sort_pieces() {
				if constexpr ((!SORTED_TARGET_PIECES || COUNT_TARGET_PIECES <= 1) && (!SORTED_NON_TARGET_PIECES || COUNT_NON_TARGET_PIECES <= 1)) {
					return;
				}
				permutation_type p_new;
				reset_perm(p_new);
				if constexpr (SORTED_TARGET_PIECES && !(COUNT_TARGET_PIECES <= 1)) {
					//std::sort(target_pieces_begin(), target_pieces_end());
					std::sort(p_new.begin(), p_new.begin() + COUNT_TARGET_PIECES, [](const std::size_t& l, const std::size_t& r) {
						return piece_positions[l] < piece_positions[r];
						});
				}
				if constexpr (SORTED_NON_TARGET_PIECES && !(COUNT_NON_TARGET_PIECES <= 1)) {
					std::sort(p_new.begin() + COUNT_TARGET_PIECES, p_new.begin() + COUNT_ALL_PIECES, [](const std::size_t& l, const std::size_t& r) {
						return piece_positions[l] < piece_positions[r];
						});
				}
				apply_perm(p_new, piece_positions);
				apply_perm(p_new, permutation);
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


		template <class Pieces_Quantity_Type>
		using piece_id = tobor::v1_0::piece_id<Pieces_Quantity_Type>;

		using default_piece_id = piece_id<default_pieces_quantity>;

		/*
		*	@brief Equivalent to a pair of a piece_id and a direction where to move it
		*
		*	@details Does not define how piece_id is interpreted.
		*/
		template<class Piece_Id_Type>
		using piece_move = tobor::v1_0::piece_move<Piece_Id_Type>;

		using default_piece_move = piece_move<default_piece_id>;

#if false
		template<class Position_Of_Pieces_T>
		using state_path = tobor::v1_0::state_path<Position_Of_Pieces_T>;
		// has to be reviewed again! ###

		template<class Piece_Move_Type>
		using move_path = tobor::v1_0::move_path<Piece_Move_Type>;
		// has to be reviewed again! ###
#endif

	}
}
