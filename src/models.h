#pragma once


#include <map>
#include <array>
#include <algorithm>


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


		/**
		*	@brief One single boolean wall
		*/
		class wall_type { // OK
			
			bool is_wall;
		
		public:

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
		class tobor_world { // OK
		public:

			using world_type = tobor_world;

		private:

			std::size_t x_size;
			std::size_t y_size;

			/*
			* Contains all horizontal walls existing on the board.
			* order: start at (0,0) -> (0, y_size - 1), go line by line x=0 .. x_size - 1
			* At index i of this vector h_walls[i] you find the horizontal wall below the cell with transposed_id i.
			*/
			wall_vector h_walls;

			/*
			* Contains all vertical walls existing on the board.
			* order: start at (0,0) -> (x_size - 1, 0), go line by line y=0 .. y_size - 1
			* At index i of this vector v_walls[i] you find the vertical wall on the left of the cell with id i.
			*/
			wall_vector v_walls;

		public:

			/* ctors et. al. **************************************************************************************/

			tobor_world() : x_size(0), y_size(0) {}

			tobor_world(const std::size_t x_size, const std::size_t y_size) : tobor_world() {
				resize(x_size, y_size);
			}

			/**
			*	@brief Sets the size of the world, also creates an empty rectangle with walls only on the outer borders
			*/
			void resize(std::size_t _x_size, std::size_t _y_size) noexcept {
				this->x_size = _x_size;
				this->y_size = _y_size;
				{
					h_walls = std::vector<wall_type>(x_size * y_size + 1, false);
					v_walls = std::vector<wall_type>(x_size * y_size + 1, false);
				} // ### check for exception (? heap allocation, should anyway cause the app to terminate, ignore this?)

				for (std::size_t i = 0; i <= x_size; ++i) { // set north and south walls
					h_walls[y_size * i] = true;
				}
				for (std::size_t i = 0; i <= y_size; ++i) { // set east and west walls
					v_walls[x_size * i] = true;
				}
			}

			/*
				@brief Fills center fields to make them unreachable.
			*/
			inline void block_center_fields(std::size_t x_blocked_size, std::size_t y_blocked_size) {
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

			inline constexpr std::size_t coordinates_to_cell_id(std::size_t x_coord, std::size_t y_coord) const noexcept {
				return x_size * y_coord + x_coord;
			}

			inline constexpr std::size_t coordinates_to_transposed_cell_id(std::size_t x_coord, std::size_t y_coord) const noexcept {
				return y_size * x_coord + y_coord;
			}

			inline constexpr std::pair<std::size_t, std::size_t> cell_id_to_coordinates(std::size_t id) const noexcept {
				return std::make_pair(id % x_size, id / x_size);
			}

			inline constexpr void cell_id_to_coordinates(std::size_t id, std::size_t& x_coord, std::size_t& y_coord) const noexcept {
				x_coord = id % x_size;
				y_coord = id / x_size;
			}

			inline constexpr std::pair<std::size_t, std::size_t> transposed_cell_id_to_coordinates(std::size_t transposed_id) const noexcept {
				return std::make_pair(transposed_id / y_size, transposed_id % y_size);
			}

			inline constexpr void transposed_cell_id_to_coordinates(std::size_t transposed_id, std::size_t& x_coord, std::size_t& y_coord) const noexcept {
				x_coord = transposed_id / y_size;
				y_coord = transposed_id % y_size;
			}

			inline constexpr std::size_t id_to_transposed_id(std::size_t id) const noexcept {
				return y_size * (id % x_size) + id / x_size;
			}

			inline constexpr std::size_t transposed_id_to_id(std::size_t transposed_id) const noexcept {
				return x_size * (transposed_id % y_size) + transposed_id / y_size;
			}

			/* wall accessors **************************************************************************************/

			inline wall_type& south_wall_by_transposed_id(std::size_t transposed_id) noexcept {
				return h_walls[transposed_id];
			}

			inline const wall_type& south_wall_by_transposed_id(std::size_t transposed_id) const noexcept {
				return h_walls[transposed_id];
			}

			inline wall_type& north_wall_by_transposed_id(std::size_t transposed_id) noexcept {
				return h_walls[transposed_id + 1];
			}

			inline const wall_type& north_wall_by_transposed_id(std::size_t transposed_id) const noexcept {
				return h_walls[transposed_id + 1];
			}

			inline wall_type& west_wall_by_id(std::size_t id) noexcept {
				return v_walls[id];
			}

			inline const wall_type& west_wall_by_id(std::size_t id) const noexcept {
				return v_walls[id];
			}

			inline wall_type& east_wall_by_id(std::size_t id) noexcept {
				return v_walls[id + 1];
			}

			inline const wall_type& east_wall_by_id(std::size_t id) const noexcept {
				return v_walls[id + 1];
			}

			/* getter **************************************************************************************/

			/**
			*	@brief Returns the board's number of cells, including totally blocked ones.
			*/
			std::size_t count_cells() const noexcept {
				return x_size * y_size;
			}

		};


		/**
		*	@brief Kind of iterator to a cell of a board game. Does only store a cell id.
		* 
		*	@details This version calculates all three cell id types when set and stores all of them.
					It is the least memory efficient way but may reduce computation time (not yet tested!).
		*/
		class universal_cell_id { // OK, add some memory-efficient variant!
		public:

			/* static factory member functions */

			inline static universal_cell_id create_by_coordinates(std::size_t p_x_coord, std::size_t p_y_coord, const tobor_world& world) noexcept {
				universal_cell_id result;
				result.set_coord(p_x_coord, p_y_coord, world);
				return result;
			}

			inline static universal_cell_id create_by_id(std::size_t p_id, const tobor_world& world) noexcept {
				universal_cell_id result;
				result.set_id(p_id, world);
				return result;
			}

			inline static universal_cell_id create_by_transposed_id(std::size_t p_transposed_id, const tobor_world& world) noexcept {
				universal_cell_id result;
				result.set_transposed_id(p_transposed_id, world);
				return result;
			}

		private:

			std::size_t id;
			std::size_t transposed_id;
			std::size_t x_coord;
			std::size_t y_coord;

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

			inline std::size_t get_id() const noexcept { return id; }

			inline std::size_t get_transposed_id() const noexcept { return transposed_id; }

			inline std::size_t get_x_coord() const noexcept { return x_coord; }

			inline std::size_t get_y_coord() const noexcept { return y_coord; }

			/* modifiers */

			inline void set_id(std::size_t p_id, const tobor_world& world) noexcept {
				id = p_id;
				world.cell_id_to_coordinates(id, x_coord, y_coord);
				transposed_id = world.coordinates_to_transposed_cell_id(x_coord, y_coord);
			}

			inline void set_transposed_id(std::size_t p_transposed_id, const tobor_world& world) noexcept {
				transposed_id = p_transposed_id;
				world.transposed_cell_id_to_coordinates(transposed_id, x_coord, y_coord);
				id = world.coordinates_to_cell_id(x_coord, y_coord);
			}

			inline void set_coord(std::size_t p_x_coord, std::size_t p_y_coord, const tobor_world& world) noexcept {
				x_coord = p_x_coord;
				y_coord = p_y_coord;
				id = world.coordinates_to_cell_id(x_coord, y_coord);
				transposed_id = world.coordinates_to_transposed_cell_id(x_coord, y_coord);
			}

		};


		/**
		*	@brief Contains the information where the pieces are located on the game board.
		*
		*	@details It only distinguishes the target piece from non target pieces.
		*			Non target pieces cannot be distiguished. They are kept sorted acending by their cell ids.
		*/
		template <std::size_t COUNT_NON_TARGET_PIECES>
		class positions_of_pieces { // OK
		// ## alternative implementation using std::vector instead of array, as non-template variant

			using cell_id = universal_cell_id;

			// just for fun
			[[deprecated]]
			inline void sort_non_target_pieces(std::size_t p_index_of_the_only_changed_piece) {
				constexpr std::size_t INDEX_BEGIN{ 0 };
				constexpr std::size_t INDEX_END{ COUNT_NON_TARGET_PIECES };

				if (p_index_of_the_only_changed_piece != INDEX_BEGIN) { // not first element
					if (non_target_pieces[p_index_of_the_only_changed_piece] < non_target_pieces[p_index_of_the_only_changed_piece - 1]) { // look in range [INDEX_BEGIN, p_index_of_the_only_changed_piece) for need of swap
						std::size_t shift_begin{ INDEX_BEGIN };
						//const std::size_t shift_end{ p_index_of_the_only_changed_piece + 1 };

						while (non_target_pieces[shift_begin] < non_target_pieces[p_index_of_the_only_changed_piece]) {
							++shift_begin;
						}

						//A B C D G J M E P R S T // move E to the left...
						// shift in determined range
						{
							cell_id swap = non_target_pieces[p_index_of_the_only_changed_piece];
							for (std::size_t i = p_index_of_the_only_changed_piece; i != shift_begin; --i) {
								non_target_pieces[i] = non_target_pieces[i - 1];
							}
							non_target_pieces[shift_begin] = swap;
						}

						return;
					}
				}
				if (p_index_of_the_only_changed_piece + 1 < INDEX_END) { // not last element
					if (non_target_pieces[p_index_of_the_only_changed_piece + 1] < non_target_pieces[p_index_of_the_only_changed_piece]) {
						//const std::size_t shift_begin{ p_index_of_the_only_changed_piece };
						std::size_t shift_rbegin{ INDEX_END - 1 };

						while (non_target_pieces[p_index_of_the_only_changed_piece] < non_target_pieces[shift_rbegin]) {
							--shift_rbegin;
						}

						// shift in determined range
						{
							cell_id swap = non_target_pieces[p_index_of_the_only_changed_piece];
							for (std::size_t i = p_index_of_the_only_changed_piece; i != shift_rbegin; ++i) {
								non_target_pieces[i] = non_target_pieces[i + 1];
							}
							non_target_pieces[shift_rbegin] = swap;
						}
						return;
					}
				}

			}

		public:

			/**
			*	@brief Cell id of the target piece, i.e. the one which should be moved to the target cell.
			*/
			cell_id target_piece;

			/**
			*	@brief Cell id of the non-target piece, i.e. the ones which can be used to buld obstacles. Need to be ordered by < all the time.
			*/
			std::array<cell_id, COUNT_NON_TARGET_PIECES> non_target_pieces;

			/**
			*	@brief Creates an object when cell positions of the pieces are given.
			*	@param p_non_target_pieces Does not need to be sorted when passed to this constructor.
			*/
			positions_of_pieces(const cell_id& p_target_piece, std::array<cell_id, COUNT_NON_TARGET_PIECES>&& p_non_target_pieces) :
				target_piece(p_target_piece),
				non_target_pieces(std::move(p_non_target_pieces))
			{
				sort_non_target_pieces();
			}

			positions_of_pieces(const positions_of_pieces&) = default;

			positions_of_pieces(positions_of_pieces&&) = default;

			bool operator< (const positions_of_pieces& another) const noexcept {
				return (target_piece < another.target_piece) || (
					(target_piece == another.target_piece) && (non_target_pieces < another.non_target_pieces)
					);
			}

			bool operator== (const positions_of_pieces& another) const noexcept {
				return target_piece == another.target_piece && non_target_pieces == another.non_target_pieces;
			}

			inline void sort_non_target_pieces() {
				std::sort(non_target_pieces.begin(), non_target_pieces.end());
			}

		};


	}

}
