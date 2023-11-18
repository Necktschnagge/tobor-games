#pragma once


#include <map>
#include <array>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iterator>


namespace tobor {

	namespace v1_0 {

		class division_by_2_error : public std::logic_error { // OK
			inline static const char MESSAGE[]{ "Wrong remainder on division by 2" };
		public:
			division_by_2_error() : std::logic_error(MESSAGE) {}
		};

		class blocked_center_error : public std::logic_error { // OK
			inline static const char MESSAGE[]{ "Blocking too many fields." };
		public:
			blocked_center_error() : std::logic_error(MESSAGE) {}
		};

		class direction {
		public:

			using int_type = uint8_t;
			using type = direction;

		private:

			int_type value;

			direction(int_type v) : value(v) {}

		public:
			struct encoding {

				static constexpr int_type NORTH{ 1 << 0 };
				static constexpr int_type EAST{ 1 << 1 };
				static constexpr int_type SOUTH{ 1 << 2 };
				static constexpr int_type WEST{ 1 << 3 };
				static constexpr int_type END{ 1 << 4 };

				static_assert(NORTH != EAST, "piece_move: NORTH == EAST");
				static_assert(NORTH != SOUTH, "piece_move: NORTH == SOUTH");
				static_assert(NORTH != WEST, "piece_move: NORTH == WEST");
				static_assert(EAST != SOUTH, "piece_move: EAST == SOUTH");
				static_assert(EAST != WEST, "piece_move: EAST == WEST");
				static_assert(SOUTH != WEST, "piece_move: SOUTH == WEST");
			};

			inline static direction NORTH() { return encoding::NORTH; }
			inline static direction EAST() { return encoding::EAST; }
			inline static direction SOUTH() { return encoding::SOUTH; }
			inline static direction WEST() { return encoding::WEST; }


			/* usage like an iterator over directions: */
			inline static direction begin() { return encoding::NORTH; }
			inline static direction end() { return encoding::END; }

			inline direction& operator++() { value <<= 1; return *this; }
			inline direction operator++(int) { direction c = *this; value <<= 1; return c; }


			/* comparison operators */
			inline bool operator<(const direction& another) { return this->value < another.value; }
			inline bool operator==(const direction& another) { return this->value == another.value; }


			/* access via conversion to underlying type */
			inline operator int_type() const { return value; }

		};

		/**
		*	@brief One single boolean wall
		*/
		class wall_type { // OK

			bool is_wall;

		public:

			using type = wall_type;

			wall_type(bool p_is_wall) : is_wall(p_is_wall) {}

			operator const bool& () const {
				return is_wall;
			}

			operator bool& () {
				return is_wall;
			}

		};


		using wall_vector = std::vector<wall_type>; // OK


		/**
		*
		*	@brief Represents the game's entire board, stating which fields are separated by walls.
		*	@details Does NOT contain any information about where pieces are located.
		*
		*/
		template <class Int_Type_T = std::size_t>
		class tobor_world { // OK
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
					h_walls = wall_vector_type(x_size * y_size + 1, false);
					v_walls = wall_vector_type(x_size * y_size + 1, false);
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

				const std::size_t x_begin{ (x_size - x_blocked_size) / 2 };
				const std::size_t x_end{ x_begin + x_blocked_size };
				const std::size_t y_begin{ (y_size - y_blocked_size) / 2 };
				const std::size_t y_end{ y_begin + y_blocked_size };

				for (auto y = y_begin; y != y_end; ++y) {
					for (auto x = x_begin; x != x_end; ++x) {
						const std::size_t id = coordinates_to_cell_id(x, y);
						const std::size_t transposed_id = coordinates_to_transposed_cell_id(x, y);

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

			inline wall_type& south_wall_by_transposed_id(int_type transposed_id) noexcept {
				return h_walls[transposed_id];
			}

			inline const wall_type& south_wall_by_transposed_id(int_type transposed_id) const noexcept {
				return h_walls[transposed_id];
			}

			inline wall_type& north_wall_by_transposed_id(int_type transposed_id) noexcept {
				return h_walls[transposed_id + 1];
			}

			inline const wall_type& north_wall_by_transposed_id(int_type transposed_id) const noexcept {
				return h_walls[transposed_id + 1];
			}

			inline wall_type& west_wall_by_id(int_type id) noexcept {
				return v_walls[id];
			}

			inline const wall_type& west_wall_by_id(int_type id) const noexcept {
				return v_walls[id];
			}

			inline wall_type& east_wall_by_id(int_type id) noexcept {
				return v_walls[id + 1];
			}

			inline const wall_type& east_wall_by_id(int_type id) const noexcept {
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

		};

		using default_world = tobor_world<>;

		/**
		*	@brief Kind of iterator to a cell of a board game. Does only store a cell id.
		*
		*	@details This version calculates all three cell id types when set and stores all of them.
					It is the least memory efficient way but may reduce computation time (not yet tested!).
		*/
		template<class World_Type_T = default_world>
		class universal_cell_id { // OK, add some memory-efficient variant!
		public:

			using world_type = World_Type_T;

			using int_type = typename world_type::int_type;

			using type = universal_cell_id;

			/* static factory member functions */

			inline static universal_cell_id create_by_coordinates(int_type p_x_coord, int_type p_y_coord, const world_type& world) noexcept {
				universal_cell_id result;
				result.set_coord(p_x_coord, p_y_coord, world);
				return result;
			}

			inline static universal_cell_id create_by_id(int_type p_id, const world_type& world) noexcept {
				universal_cell_id result;
				result.set_id(p_id, world);
				return result;
			}

			inline static universal_cell_id create_by_transposed_id(int_type p_transposed_id, const world_type& world) noexcept {
				universal_cell_id result;
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

			universal_cell_id() : id(0), transposed_id(0), x_coord(0), y_coord(0) {}

			universal_cell_id(const universal_cell_id&) = default;

			universal_cell_id(universal_cell_id&&) = default; // needed for static factory member function

			/* operator = */

			inline universal_cell_id& operator = (const universal_cell_id&) = default;

			inline universal_cell_id& operator = (universal_cell_id&&) = default;

			/* comparison operators */

			inline bool operator < (const universal_cell_id& other) const noexcept {
				return this->id < other.id;
			}

			inline bool operator == (const universal_cell_id& other) const noexcept {
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

		using default_cell_id = universal_cell_id<>;

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
		using uint8_t_pieces_quantity = pieces_quantity< uint8_t, COUNT_TARGET_PIECES_V, COUNT_NON_TARGET_PIECES_V>;

		using default_pieces_quantity = pieces_quantity<uint8_t, 1, 3>;


		/**
		*	@brief Contains the information where the pieces are located on the game board.
		*
		*	@details It only distinguishes the target piece from non target pieces.
		*			Non target pieces cannot be distiguished. They are kept sorted acending by their cell ids.
		*/
		template <class Pieces_Quantity_Type = default_pieces_quantity, class Cell_Id_Type_T = default_cell_id, bool SORTED_TARGET_PIECES_V = true, bool SORTED_NON_TARGET_PIECES_V = true>
		class positions_of_pieces { // OK
			// ## alternative implementation using std::vector instead of array, as non-template variant

			template <class Move_One_Piece_Calculator, class State_Graph_Node>
			friend class partial_state_graph;

			template<class Position_Of_Pieces_T, class Quick_Move_Cache_T, class Piece_Move_Type>
			friend class move_one_piece_calculator;

		public:

			using pieces_quantity_type = Pieces_Quantity_Type;

			using cell_id_type = Cell_Id_Type_T;

			using world_type = typename cell_id_type::world_type;

			// cell_id_type::int_type for cell ids

			static constexpr typename pieces_quantity_type::int_type COUNT_TARGET_PIECES{ Pieces_Quantity_Type::COUNT_TARGET_PIECES };

			static constexpr typename pieces_quantity_type::int_type COUNT_NON_TARGET_PIECES{ Pieces_Quantity_Type::COUNT_NON_TARGET_PIECES };

			static constexpr typename pieces_quantity_type::int_type COUNT_ALL_PIECES{ Pieces_Quantity_Type::COUNT_ALL_PIECES };

			static constexpr bool SORTED_TARGET_PIECES{ SORTED_TARGET_PIECES_V };

			static constexpr bool SORTED_NON_TARGET_PIECES{ SORTED_NON_TARGET_PIECES_V };

			using target_pieces_array_type = std::array<cell_id_type, COUNT_TARGET_PIECES>;

			using non_target_pieces_array_type = std::array<cell_id_type, COUNT_NON_TARGET_PIECES>;

			using all_pieces_array_type = std::array<cell_id_type, COUNT_ALL_PIECES>;

			//using coloring_type_uint64 = std::array<uint64_t, COUNT_ALL_PIECES>;

		private:

			/**
			*	@brief Cell ids of the target piece(s) and non-target piece.
			*	@details Both sections {TARGET_PIECES : NON_TARGET_PIECES} need to be ordered by < all the time if specified so by template arguments.
			*
			*/
			all_pieces_array_type piece_positions;

		public:

			/**
			*	@brief Creates an object when cell positions of the pieces are given.
			*	@param p_non_target_pieces Does not need to be sorted when passed to this constructor.
			*/
			positions_of_pieces(const target_pieces_array_type& target_pieces, const non_target_pieces_array_type& non_target_pieces) {
				std::copy_n(target_pieces.begin(), COUNT_TARGET_PIECES, piece_positions.begin());
				std::copy_n(non_target_pieces.begin(), COUNT_NON_TARGET_PIECES, piece_positions.begin() + COUNT_TARGET_PIECES);
				sort_pieces();
			}

			positions_of_pieces(const positions_of_pieces&) = default;

			positions_of_pieces(positions_of_pieces&&) = default;

			bool operator< (const positions_of_pieces& another) const noexcept {
				return piece_positions < another.piece_positions;
			}

			bool operator== (const positions_of_pieces& another) const noexcept {
				return piece_positions == another.piece_positions;
			}

			inline auto target_pieces_begin() -> typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator{
				return piece_positions.begin();
			};

			inline auto target_pieces_end() -> typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator{
				return piece_positions.begin() + COUNT_TARGET_PIECES;
			};

			inline auto non_target_pieces_begin() -> typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator{
				return target_pieces_end();
			};

			inline auto non_target_pieces_end() -> typename std::array<cell_id_type, COUNT_ALL_PIECES>::iterator{
				return piece_positions.end();
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
		};

		using default_positions_of_pieces = positions_of_pieces<default_pieces_quantity, default_cell_id, false, true>;


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

			inline bool operator < (const piece_id& another) { return value < another.value; }

			inline bool operator == (const piece_id& another) { return value == another.value; }

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
		};

		using default_piece_move = piece_move<>;


		template<class Position_Of_Pieces_T = default_positions_of_pieces>
		class state_path {
		public:

			using positions_of_pieces_type = Position_Of_Pieces_T;

			using vector_type = std::vector<positions_of_pieces_type>;

		private:

			vector_type state_vector;

		public:

			vector_type& vector() { return state_vector; }

			inline void make_canonical() {

				typename vector_type::size_type count_duplicates{ 0 };
				typename vector_type::size_type i = 0;

				while (i + count_duplicates + 1 < state_vector.size()) {
					if (state_vector[i] == state_vector[i + count_duplicates + 1]) {
						++count_duplicates;
					}
					else {
						if (count_duplicates)
							state_vector[i + 1] = state_vector[i + count_duplicates + 1];
						++i;
					}
				}

				// now i + count_duplicates + 1 == state_vector.size()
				state_vector.erase(state_vector.begin() + i + 1, state_vector.end());
			}

			inline state_path operator +(const state_path& another) {
				state_path copy{ *this };
				std::copy(another.state_vector.cbegin(), another.state_vector.cend(), std::back_inserter(copy.state_vector));
				return copy;
			}

		};
	}
}
