#pragma once


#include <map>
#include <set>
#include <array>
#include <vector>

#include <algorithm>
#include <utility>

#include <stdexcept>
#include <iterator>
#include <string>
#include <execution>
#include <compare>


namespace tobor {

	namespace v1_0 {

		/**
		*	@brief An error to be thrown when a number is of wrong remainder class on division by 2.
		*/
		class division_by_2_error : public std::logic_error {
			inline static const char MESSAGE[]{ "Wrong remainder on division by 2" };
		public:
			division_by_2_error() : std::logic_error(MESSAGE) {}
		};


		/**
		*	@brief An error to be thrown when there are too many blocked cells in the center of a game board.
		*/
		class blocked_center_error : public std::logic_error {
			inline static const char MESSAGE[]{ "Blocking too many cells." };
		public:
			blocked_center_error() : std::logic_error(MESSAGE) {}
		};


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


		/**
		*	@brief One single boolean wall
		*/
		class wall {

			bool _is_wall;

		public:

			using type = wall;

			wall(bool is_wall) : _is_wall(is_wall) {}

			operator bool() const { return _is_wall; }

			operator bool& () { return _is_wall; }

		};


		using wall_vector = std::vector<wall>;


		/**
		*
		*	@brief Represents the game's entire board, stating which cells are separated by walls.
		*	@details Does NOT contain any information about where pieces are located.
		*
		*/
		template <class Int_Type_T = std::size_t>
		class tobor_world {
		public:

			using int_type = Int_Type_T;
			using wall_vector_type = wall_vector;
			using type = tobor_world;

		private:

			int_type x_size;
			int_type y_size;

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

			/* ctors et. al. **************************************************************************************/

			tobor_world() : x_size(0), y_size(0) {}

			tobor_world(const int_type& x_size, const int_type& y_size) : tobor_world() {
				resize(x_size, y_size);
			}

			/**
			*	@brief Sets the size of the world, also creates an empty rectangle with walls only on the outer borders
			*/
			void resize(int_type _x_size, int_type _y_size) noexcept {
				this->x_size = _x_size;
				this->y_size = _y_size;
				{
					const wall_vector_type::size_type SIZE{ static_cast<std::size_t>(x_size) * y_size + 1 };

					h_walls = wall_vector_type(SIZE, false);
					v_walls = wall_vector_type(SIZE, false);
				} // ### check for exception (? heap allocation, should anyway cause the app to terminate, ignore this?)

				for (int_type i = 0; i <= x_size; ++i) { // set north and south walls
					h_walls[y_size * i] = true;
				}
				for (int_type i = 0; i <= y_size; ++i) { // set east and west walls
					v_walls[x_size * i] = true;
				}
			}

			/*
				@brief Fills center fields to make them unreachable.
			*/
			inline void block_center_cells(int_type x_blocked_size, int_type y_blocked_size) {
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

				const int_type x_begin{ static_cast<int_type>((x_size - x_blocked_size) / 2) };
				const int_type x_end{ static_cast<int_type>(x_begin + x_blocked_size) };
				const int_type y_begin{ static_cast<int_type>((y_size - y_blocked_size) / 2) };
				const int_type y_end{ static_cast<int_type>(y_begin + y_blocked_size) };

				for (auto y = y_begin; y != y_end; ++y) {
					for (auto x = x_begin; x != x_end; ++x) {
						const int_type id = coordinates_to_cell_id(x, y);
						const int_type transposed_id = coordinates_to_transposed_cell_id(x, y);

						north_wall_by_transposed_id(transposed_id) = true;
						east_wall_by_id(id) = true;
						south_wall_by_transposed_id(transposed_id) = true;
						west_wall_by_id(id) = true;
						// remark: this is by the way inefficient, but readable
					}
				}
			}

			/* cell id conversion **************************************************************************************/

			inline constexpr int_type coordinates_to_cell_id(int_type x_coord, int_type y_coord) const noexcept {
				return x_size * y_coord + x_coord;
			}

			inline constexpr int_type coordinates_to_transposed_cell_id(int_type x_coord, int_type y_coord) const noexcept {
				return y_size * x_coord + y_coord;
			}

			inline constexpr std::pair<int_type, int_type> cell_id_to_coordinates(int_type id) const noexcept {
				return std::make_pair(id % x_size, id / x_size);
			}

			inline constexpr void cell_id_to_coordinates(int_type id, int_type& x_coord, int_type& y_coord) const noexcept {
				x_coord = id % x_size;
				y_coord = id / x_size;
			}

			inline constexpr std::pair<int_type, int_type> transposed_cell_id_to_coordinates(int_type transposed_id) const noexcept {
				return std::make_pair(transposed_id / y_size, transposed_id % y_size);
			}

			inline constexpr void transposed_cell_id_to_coordinates(int_type transposed_id, int_type& x_coord, int_type& y_coord) const noexcept {
				x_coord = transposed_id / y_size;
				y_coord = transposed_id % y_size;
			}

			inline constexpr int_type id_to_transposed_id(int_type id) const noexcept {
				return y_size * (id % x_size) + id / x_size;
			}

			inline constexpr int_type transposed_id_to_id(int_type transposed_id) const noexcept {
				return x_size * (transposed_id % y_size) + transposed_id / y_size;
			}

			/* wall accessors **************************************************************************************/

			inline wall& south_wall_by_transposed_id(int_type transposed_id) noexcept {
				return h_walls[transposed_id];
			}

			inline const wall& south_wall_by_transposed_id(int_type transposed_id) const noexcept {
				return h_walls[transposed_id];
			}

			inline wall& north_wall_by_transposed_id(int_type transposed_id) noexcept {
				return h_walls[transposed_id + 1];
			}

			inline const wall& north_wall_by_transposed_id(int_type transposed_id) const noexcept {
				return h_walls[transposed_id + 1];
			}

			inline wall& west_wall_by_id(int_type id) noexcept {
				return v_walls[id];
			}

			inline const wall& west_wall_by_id(int_type id) const noexcept {
				return v_walls[id];
			}

			inline wall& east_wall_by_id(int_type id) noexcept {
				return v_walls[id + 1];
			}

			inline const wall& east_wall_by_id(int_type id) const noexcept {
				return v_walls[id + 1];
			}

			/* getter **************************************************************************************/

			/**
			*	@brief Returns the board's number of cells, including totally blocked ones.
			*/
			inline int_type count_cells() const noexcept {
				return x_size * y_size;
			}

			/**
			*	@brief Return true if and only if given cell is blocked.
			*	@details cell_id must be a non-negative integer less than \a count_cells(). Otherwise behaviour is undefined.
			*/
			inline bool blocked(int_type cell_id) const {
				auto transposed_id = id_to_transposed_id(cell_id);
				return west_wall_by_id(cell_id) && east_wall_by_id(cell_id) && south_wall_by_transposed_id(transposed_id) && north_wall_by_transposed_id(transposed_id);
			}

			/**
			*	@brief Returns the board's number of blocked cells
			*/
			inline int_type blocked_cells() const noexcept {
				int_type counter{ 0 };
				for (int_type cell_id = 0; cell_id < count_cells(); ++cell_id) {
					counter += blocked(cell_id);
				}
				return counter;
			}

			/**
			*	@brief Returns the board's number of cells in a row.
			*/
			inline int_type get_horizontal_size() const noexcept {
				return x_size;
			}

			/**
			*	@brief Returns the board's number of cells in a column.
			*/
			inline int_type get_vertical_size() const noexcept {
				return y_size;
			}

			type turn_left_90() const { // only for quadratic
				if (x_size != y_size) {
					throw std::logic_error("Cannot turn for non-quadratic world.");
				}
				auto copy = type(x_size, y_size);

				for (int_type big_id = 0; big_id < x_size; ++big_id) {
					for (int_type little_id = 0; little_id < x_size; ++little_id) {
						copy.v_walls[big_id * x_size + little_id] = h_walls[big_id * x_size + (x_size - little_id)];
						copy.h_walls[big_id * x_size + little_id] = v_walls[((x_size - 1) - big_id) * x_size + little_id];
					}
				}
				return copy;
			}

		};

		using default_world = tobor_world<>;

		/**
		*	@brief Kind of iterator to a cell of a board game. Does only store a cell id.
		*
		*	@details This version calculates all three cell id types when set and stores all of them.
					It is the least memory efficient way but may reduce computation time (not yet tested!).
					Reading different kind of ids does not require a game board object.
		*/
		template<class World_Type_T = default_world>
		class redundant_cell_id {
		public:

			using world_type = World_Type_T;

			using int_type = typename world_type::int_type;

			using type = redundant_cell_id;

			/* static factory member functions */

			inline static redundant_cell_id create_by_coordinates(int_type p_x_coord, int_type p_y_coord, const world_type& world) noexcept {
				redundant_cell_id result;
				result.set_coord(p_x_coord, p_y_coord, world);
				return result;
			}

			inline static redundant_cell_id create_by_id(int_type p_id, const world_type& world) noexcept {
				redundant_cell_id result;
				result.set_id(p_id, world);
				return result;
			}

			inline static redundant_cell_id create_by_transposed_id(int_type p_transposed_id, const world_type& world) noexcept {
				redundant_cell_id result;
				result.set_transposed_id(p_transposed_id, world);
				return result;
			}

		private:

			int_type id;
			int_type transposed_id;
			int_type x_coord;
			int_type y_coord;

		public:

			/* ctors */

			redundant_cell_id() : id(0), transposed_id(0), x_coord(0), y_coord(0) {}

			redundant_cell_id(const redundant_cell_id&) = default;

			redundant_cell_id(redundant_cell_id&&) = default; // needed for static factory member function

			/* operator = */

			inline redundant_cell_id& operator = (const redundant_cell_id&) = default;

			inline redundant_cell_id& operator = (redundant_cell_id&&) = default;

			/* comparison operators */

			inline bool operator < (const redundant_cell_id& other) const noexcept {
				return this->id < other.id;
			}

			inline bool operator == (const redundant_cell_id& other) const noexcept {
				return this->id == other.id;
			}

			/* getter */

			inline int_type get_id() const noexcept { return id; }

			inline int_type get_transposed_id() const noexcept { return transposed_id; }

			inline int_type get_x_coord() const noexcept { return x_coord; }

			inline int_type get_y_coord() const noexcept { return y_coord; }

			/* modifiers */

			inline void set_id(int_type p_id, const world_type& world) noexcept {
				id = p_id;
				world.cell_id_to_coordinates(id, x_coord, y_coord);
				transposed_id = world.coordinates_to_transposed_cell_id(x_coord, y_coord);
			}

			inline void set_transposed_id(int_type p_transposed_id, const world_type& world) noexcept {
				transposed_id = p_transposed_id;
				world.transposed_cell_id_to_coordinates(transposed_id, x_coord, y_coord);
				id = world.coordinates_to_cell_id(x_coord, y_coord);
			}

			inline void set_coord(int_type p_x_coord, int_type p_y_coord, const world_type& world) noexcept {
				x_coord = p_x_coord;
				y_coord = p_y_coord;
				id = world.coordinates_to_cell_id(x_coord, y_coord);
				transposed_id = world.coordinates_to_transposed_cell_id(x_coord, y_coord);
			}

		};

		using default_cell_id = redundant_cell_id<>;

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


		/**
		*	@brief Contains the information where the pieces are located on the game board.
		*/
		template <class Pieces_Quantity_Type = default_pieces_quantity, class Cell_Id_Type_T = default_cell_id, bool SORTED_TARGET_PIECES_V = true, bool SORTED_NON_TARGET_PIECES_V = true>
		class positions_of_pieces {
			// ## alternative implementation using std::vector instead of array, as non-template variant

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

		public:

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

			inline all_pieces_array_type& piece_positions() { return _piece_positions; } // ### do the same in the naked class

			inline const all_pieces_array_type& piece_positions() const { return _piece_positions; } // ### do the same in the naked class

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
			*	@brief Returns the number of pieces ín which both states differ.
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
		*	@brief Equivalent to a pair of a piece_id and a direction where to move it
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
