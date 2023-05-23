

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

		class wall_type { // OK
		public:
			bool is_wall;

			wall_type(bool p_is_wall) : is_wall(p_is_wall) {}

			operator const bool& () const {
				return is_wall;
			}

			operator bool& () {
				return is_wall;
			}
		};

		using wall_vector = std::vector<wall_type>; // OK


		class tobor_world {
		public:
			using world_type = tobor_world;

		private:

			std::size_t x_size;
			std::size_t y_size;

			/*
			* order: start at (0,0) -> (0, y_max) , go line by line x=0 .. x_size - 1
			*/
			wall_vector fixed_hwalls;

			/*
			* order: start at (0,0) -> (x_size - 1, 0) , go line by line y=0 .. y_size - 1
			*/
			wall_vector fixed_vwalls;

		public:
			inline constexpr std::size_t field_id_of(std::size_t x_coord, std::size_t y_coord) const noexcept {
				return x_size * y_coord + x_coord;
			}

			inline constexpr std::size_t transposed_field_id_of(std::size_t x_coord, std::size_t y_coord) const noexcept {
				return y_size * x_coord + y_coord;
			}

			inline constexpr std::pair<std::size_t, std::size_t> coordinates_of_field_id(std::size_t id) const noexcept {
				return std::make_pair(id % x_size, id / x_size);
			}

			inline constexpr void coordinates_of_field_id(std::size_t id, std::size_t& x_coord, std::size_t& y_coord) const noexcept {
				x_coord = id % x_size;
				y_coord = id / x_size;
			}

			inline constexpr std::pair<std::size_t, std::size_t> coordinates_of_transposed_field_id(std::size_t transposed_id) const noexcept {
				return std::make_pair(transposed_id % y_size, transposed_id / y_size);
			}

			inline constexpr void coordinates_of_transposed_field_id(std::size_t transposed_id, std::size_t& x_coord, std::size_t& y_coord) const noexcept {
				x_coord = transposed_id % y_size;
				y_coord = transposed_id / y_size;
			}

			inline constexpr std::size_t transpose_id(std::size_t id) const noexcept {
				return y_size * (id % x_size) + id / x_size;
			}

			inline constexpr std::size_t detranspose_id(std::size_t transposed_id) const noexcept {
				return x_size * (transposed_id % y_size) + transposed_id / y_size;
			}

			inline wall_type& south_wall_by_transposed_id(std::size_t transposed_id) noexcept {
				return fixed_hwalls[transposed_id];
			}

			inline const wall_type& south_wall_by_transposed_id(std::size_t transposed_id) const noexcept {
				return fixed_hwalls[transposed_id];
			}

			inline wall_type& north_wall_by_transposed_id(std::size_t transposed_id) noexcept {
				return fixed_hwalls[transposed_id + 1];
			}

			inline const wall_type& north_wall_by_transposed_id(std::size_t transposed_id) const noexcept {
				return fixed_hwalls[transposed_id + 1];
			}

			inline wall_type& west_wall_by_id(std::size_t id) noexcept {
				return fixed_vwalls[id];
			}

			inline const wall_type& west_wall_by_id(std::size_t id) const noexcept {
				return fixed_vwalls[id];
			}

			inline wall_type& east_wall_by_id(std::size_t id) noexcept {
				return fixed_vwalls[id + 1];
			}

			inline const wall_type& east_wall_by_id(std::size_t id) const noexcept {
				return fixed_vwalls[id + 1];
			}

			std::size_t count_fields() const noexcept {
				return x_size * y_size;
			}

			tobor_world() : x_size(0), y_size(0) {}

			tobor_world(const std::size_t x_size, const std::size_t y_size) : tobor_world() {
				resize(x_size, y_size);
			}

			/*
				@brief Sets the size of the world, also creates an empty rectangle with walls only on the outer borders
			*/
			void resize(std::size_t _x_size, std::size_t _y_size) noexcept {
				this->x_size = _x_size;
				this->y_size = _y_size;
				{
					fixed_hwalls = std::vector<wall_type>(x_size * y_size + 1, false);
					fixed_vwalls = std::vector<wall_type>(x_size * y_size + 1, false);
				} // ### check for exception

				for (std::size_t i = 0; i <= x_size; ++i) { // set north and south walls
					fixed_hwalls[y_size * i] = true;
				}
				for (std::size_t i = 0; i <= y_size; ++i) { // set east and west walls
					fixed_vwalls[x_size * i] = true;
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
				if (x_blocked_size == x_size)
					throw blocked_center_error();
				if (y_blocked_size == y_size)
					throw blocked_center_error();
				const std::size_t x_begin{ (x_size - x_blocked_size) / 2 };
				const std::size_t x_end{ x_begin + x_blocked_size };
				const std::size_t y_begin{ (y_size - y_blocked_size) / 2 };
				const std::size_t y_end{ y_begin + y_blocked_size };

				for (auto y = y_begin; y != y_end; ++y) {
					for (auto x = x_begin; x != x_end; ++x) {
						const std::size_t id = field_id_of(x, y);
						const std::size_t transposed_id = transposed_field_id_of(x, y);

						north_wall_by_transposed_id(transposed_id) = true;
						east_wall_by_id(id) = true;
						south_wall_by_transposed_id(transposed_id) = true;
						west_wall_by_id(id) = true;
					}
				}
			}

		};

		class universal_field_id { // OK, add some memory-efficient variant!

			std::size_t id;
			std::size_t transposed_id;
			std::size_t x_coord;
			std::size_t y_coord;

		public:

			universal_field_id() : id(0), transposed_id(0), x_coord(0), y_coord(0) {}

			inline universal_field_id& operator = (const universal_field_id&) = default;

			inline bool operator < (const universal_field_id& other) const noexcept {
				return this->id < other.id;
			}

			inline bool operator == (const universal_field_id& other) const noexcept {
				return this->id == other.id;
			}

			inline std::size_t get_id() const noexcept { return id; }

			inline std::size_t get_transposed_id() const noexcept { return transposed_id; }

			inline std::size_t get_x_coord() const noexcept { return x_coord; }

			inline std::size_t get_y_coord() const noexcept { return y_coord; }

			inline void set_id(std::size_t p_id, const tobor_world& world) noexcept {
				id = p_id;
				world.coordinates_of_field_id(id, x_coord, y_coord);
				transposed_id = world.transposed_field_id_of(x_coord, y_coord);
			}

			inline void set_transposed_id(std::size_t p_transposed_id, const tobor_world& world) noexcept {
				transposed_id = p_transposed_id;
				world.coordinates_of_transposed_field_id(transposed_id, x_coord, y_coord);
				id = world.field_id_of(x_coord, y_coord);
			}

			inline void set_coord(std::size_t p_x_coord, std::size_t p_y_coord, const tobor_world& world) noexcept {
				x_coord = p_x_coord;
				y_coord = p_y_coord;
				id = world.field_id_of(x_coord, y_coord);
				transposed_id = world.transposed_field_id_of(x_coord, y_coord);
			}

			inline static universal_field_id create_by_coord(std::size_t p_x_coord, std::size_t p_y_coord, const tobor_world& world) noexcept {
				universal_field_id id;
				id.set_coord(p_x_coord, p_y_coord, world);
				return id;
			}

			inline static universal_field_id create_by_id(std::size_t p_id, const tobor_world& world) noexcept {
				universal_field_id id;
				id.set_id(p_id, world);
				return id;
			}

			inline static universal_field_id create_by_transposed_id(std::size_t p_transposed_id, const tobor_world& world) noexcept {
				universal_field_id id;
				id.set_transposed_id(p_transposed_id, world);
				return id;
			}

		};

		template <std::size_t COUNT_NON_TARGET_ROBOTS> // ## alternative implementation using std::vector instead of array, as non-template variant
		class robots_position_state {
			using Field_Id_Type = universal_field_id;

			inline void sort_robots() {
				std::sort(other_robots_sorted.begin(), other_robots_sorted.end());
			}

			// just for fun
			[[deprecated]]
			inline void sort_robots(std::size_t p_index_of_the_only_changed_robot) {
				constexpr std::size_t INDEX_BEGIN{ 0 };
				constexpr std::size_t INDEX_END{ COUNT_NON_TARGET_ROBOTS };

				if (p_index_of_the_only_changed_robot != INDEX_BEGIN) { // not first element
					if (other_robots_sorted[p_index_of_the_only_changed_robot] < other_robots_sorted[p_index_of_the_only_changed_robot - 1]) { // look in range [INDEX_BEGIN, p_index_of_the_only_changed_robot) for need of swap
						std::size_t shift_begin{ INDEX_BEGIN };
						//const std::size_t shift_end{ p_index_of_the_only_changed_robot + 1 };

						while (other_robots_sorted[shift_begin] < other_robots_sorted[p_index_of_the_only_changed_robot]) {
							++shift_begin;
						}

						//A B C D G J M E P R S T // move E to the left...
						// shift in determined range
						{
							Field_Id_Type swap = other_robots_sorted[p_index_of_the_only_changed_robot];
							for (std::size_t i = p_index_of_the_only_changed_robot; i != shift_begin; --i) {
								other_robots_sorted[i] = other_robots_sorted[i - 1];
							}
							other_robots_sorted[shift_begin] = swap;
						}

						return;
					}
				}
				if (p_index_of_the_only_changed_robot + 1 < INDEX_END) { // not last element
					if (other_robots_sorted[p_index_of_the_only_changed_robot + 1] < other_robots_sorted[p_index_of_the_only_changed_robot]) {
						//const std::size_t shift_begin{ p_index_of_the_only_changed_robot };
						std::size_t shift_rbegin{ INDEX_END - 1 };

						while (other_robots_sorted[p_index_of_the_only_changed_robot] < other_robots_sorted[shift_rbegin]) {
							--shift_rbegin;
						}

						// shift in determined range
						{
							Field_Id_Type swap = other_robots_sorted[p_index_of_the_only_changed_robot];
							for (std::size_t i = p_index_of_the_only_changed_robot; i != shift_rbegin; ++i) {
								other_robots_sorted[i] = other_robots_sorted[i + 1];
							}
							other_robots_sorted[shift_rbegin] = swap;
						}
						return;
					}
				}

			}

		public:
			Field_Id_Type target_robot;
			std::array<Field_Id_Type, COUNT_NON_TARGET_ROBOTS> other_robots_sorted;
#if false
			robots_position_state(const Field_Id_Type& p_target_robot, const Field_Id_Type& p_other_robot_0, const Field_Id_Type& p_other_robot_1, const Field_Id_Type& p_other_robot_2) :
				target_robot(p_target_robot),
				other_robots_sorted(p_other_robot_0, p_other_robot_1, p_other_robot_2)
			{
				sort_robots();
			}
#endif
			//template<class Iterator>
			robots_position_state(const Field_Id_Type& p_target_robot, std::array<Field_Id_Type, COUNT_NON_TARGET_ROBOTS>&& p_other_robots) :
				target_robot(p_target_robot),
				other_robots_sorted(std::move(p_other_robots))
			{
				sort_robots();
			}

			bool operator< (const robots_position_state& another) const noexcept {
				return (target_robot < another.target_robot) || (
					(target_robot == another.target_robot) && (other_robots_sorted < another.other_robots_sorted)
					);
			}

			bool operator== (const robots_position_state& another) const noexcept {
				return target_robot == another.target_robot && other_robots_sorted == another.other_robots_sorted;
			}
		};


		class quick_move_entry {
			using Field_Id_Type = universal_field_id;
			using World_Type = tobor_world;

		public:
			Field_Id_Type next_north; // ### cannot be const, because of ctor, should be only const-accessable!
			Field_Id_Type next_east;
			Field_Id_Type next_south;
			Field_Id_Type next_west;

			quick_move_entry(const Field_Id_Type& start_field, const World_Type& world) {
				// ### inefficient: try to use some recursive call on next east of left neighbour cell, if next_east is not equal to start field
				// east, west - vwalls - id
				std::size_t next_west_id = start_field.get_id();
				while (!world.west_wall_by_id(next_west_id)) {
					--next_west_id;
				}
				next_west.set_id(next_west_id, world);

				std::size_t next_east_id = start_field.get_id();
				while (!world.east_wall_by_id(next_east_id)) {
					++next_east_id;
				}
				next_east.set_id(next_east_id, world);

				// north south - hwalls - transposed_id
				std::size_t next_south_transposed_id = start_field.get_transposed_id();
				while (!world.south_wall_by_transposed_id(next_south_transposed_id)) {
					--next_south_transposed_id;
				}
				next_south.set_transposed_id(next_south_transposed_id, world);

				std::size_t next_north_transposed_id = start_field.get_transposed_id();
				while (!world.north_wall_by_transposed_id(next_north_transposed_id)) {
					++next_north_transposed_id;
				}
				next_north.set_transposed_id(next_north_transposed_id, world);

			}
		};

		class quick_move_table {
		public:
			using Field_Id_Type = universal_field_id;
			using World_Type = tobor_world;

			// maps:   id |-> quick_move_entry of field with given id
			std::vector<quick_move_entry> cells;

			quick_move_table() {}

			quick_move_table(const World_Type& world) {
				cells.reserve(world.count_fields());
				for (std::size_t id = 0; id < world.count_fields(); ++id) {
					Field_Id_Type field;
					field.set_id(id, world);

					cells.emplace_back(quick_move_entry(field, world));
				}
			}
		};

		template<std::size_t COUNT_NON_TARGET_ROBOTS>
		class tobor_world_analyzer {
		public:
			using world_type = tobor_world;

		private:
			const world_type& my_world;

			quick_move_table table;

		public:
			tobor_world_analyzer(const world_type& my_world) : my_world(my_world) {}

			inline void create_quick_move_table() {
				table = quick_move_table(my_world);
			}

			inline std::pair<universal_field_id, bool> get_next_field_on_west_move(const universal_field_id& start_field, const robots_position_state<COUNT_NON_TARGET_ROBOTS>& state) {
				const std::size_t& x_coord_start{ start_field.get_x_coord() }; // ## use id instead, transposed_id respectively for other directions: less comparison operations in code, maybe not at runtime...
				const std::size_t& y_coord{ start_field.get_y_coord() };
				const universal_field_id& next_without_obstacle{ table.cells[start_field.get_id()].next_west };
				std::size_t x_coord_last{ next_without_obstacle.get_x_coord() };
				universal_field_id next_west;
				if (x_coord_start == x_coord_last) {
					/* no move possible */
					return std::make_pair(next_west, false);
				}
				// looking for an obstacle...
				if (state.target_robot.get_y_coord() == y_coord) { // if we compare ids we will not have to check if in correct line.
					if (state.target_robot.get_x_coord() < x_coord_start && state.target_robot.get_x_coord() >= x_coord_last) {
						x_coord_last = state.target_robot.get_x_coord() + 1;
					}
				}
				for (auto& robot : state.other_robots_sorted) {
					if (robot.get_y_coord() == y_coord) {
						if (robot.get_x_coord() < x_coord_start && robot.get_x_coord() >= x_coord_last) {
							x_coord_last = robot.get_x_coord() + 1;
						}
					}
				}
				if (x_coord_start == x_coord_last) {
					/* no move possible */
					return std::make_pair(next_west, false);
				}
				next_west.set_coord(x_coord_last, y_coord, my_world);
				return std::make_pair(next_west, true);
			}

			inline std::pair<universal_field_id, bool> get_next_field_on_east_move(const universal_field_id& start_field, const robots_position_state< COUNT_NON_TARGET_ROBOTS>& state) {
				const std::size_t& x_coord_start{ start_field.get_x_coord() };
				const std::size_t& y_coord{ start_field.get_y_coord() };
				const universal_field_id& next_without_obstacle{ table.cells[start_field.get_id()].next_east };
				std::size_t x_coord_last{ next_without_obstacle.get_x_coord() };
				universal_field_id next_east;
				if (x_coord_start == x_coord_last) {
					/* no move possible */
					return std::make_pair(next_east, false);
				}
				// looking for an obstacle...
				if (state.target_robot.get_y_coord() == y_coord) {
					if (state.target_robot.get_x_coord() > x_coord_start && state.target_robot.get_x_coord() <= x_coord_last) {
						x_coord_last = state.target_robot.get_x_coord() - 1;
					}
				}
				for (auto& robot : state.other_robots_sorted) {
					if (robot.get_y_coord() == y_coord) {
						if (robot.get_x_coord() > x_coord_start && robot.get_x_coord() <= x_coord_last) {
							x_coord_last = robot.get_x_coord() - 1;
						}
					}
				}
				if (x_coord_start == x_coord_last) {
					/* no move possible */
					return std::make_pair(next_east, false);
				}
				next_east.set_coord(x_coord_last, y_coord, my_world);
				return std::make_pair(next_east, true);
			}

			inline std::pair<universal_field_id, bool> get_next_field_on_south_move(const universal_field_id& start_field, const robots_position_state< COUNT_NON_TARGET_ROBOTS>& state) {
				const std::size_t& x_coord{ start_field.get_x_coord() };
				const std::size_t& y_coord_start{ start_field.get_y_coord() };
				const universal_field_id& next_without_obstacle{ table.cells[start_field.get_id()].next_south };
				std::size_t y_coord_last{ next_without_obstacle.get_y_coord() };
				universal_field_id next_south;
				if (y_coord_start == y_coord_last) {
					/* no move possible */
					return std::make_pair(next_south, false);
				}
				// looking for an obstacle...
				if (state.target_robot.get_x_coord() == x_coord) {
					if (state.target_robot.get_y_coord() < y_coord_start && state.target_robot.get_y_coord() >= y_coord_last) {
						y_coord_last = state.target_robot.get_y_coord() + 1;
					}
				}
				for (auto& robot : state.other_robots_sorted) {
					if (robot.get_x_coord() == x_coord) {
						if (robot.get_y_coord() < y_coord_start && robot.get_y_coord() >= y_coord_last) {
							y_coord_last = robot.get_y_coord() + 1;
						}
					}
				}
				if (y_coord_start == y_coord_last) {
					/* no move possible */
					return std::make_pair(next_south, false);
				}
				next_south.set_coord(x_coord, y_coord_last, my_world);
				return std::make_pair(next_south, true);
			}

			inline std::pair<universal_field_id, bool> get_next_field_on_north_move(const universal_field_id& start_field, const robots_position_state<  COUNT_NON_TARGET_ROBOTS>& state) {
				const std::size_t& x_coord{ start_field.get_x_coord() };
				const std::size_t& y_coord_start{ start_field.get_y_coord() };
				const universal_field_id& next_without_obstacle{ table.cells[start_field.get_id()].next_north };
				std::size_t y_coord_last{ next_without_obstacle.get_y_coord() };
				universal_field_id next_north;
				if (y_coord_start == y_coord_last) {
					/* no move possible */
					return std::make_pair(next_north, false);
				}
				// looking for an obstacle...
				if (state.target_robot.get_x_coord() == x_coord) {
					if (state.target_robot.get_y_coord() > y_coord_start && state.target_robot.get_y_coord() <= y_coord_last) {
						y_coord_last = state.target_robot.get_y_coord() + 1;
					}
				}
				for (auto& robot : state.other_robots_sorted) {
					if (robot.get_x_coord() == x_coord) {
						if (robot.get_y_coord() > y_coord_start && robot.get_y_coord() <= y_coord_last) {
							y_coord_last = robot.get_y_coord() + 1;
						}
					}
				}
				if (y_coord_start == y_coord_last) {
					/* no move possible */
					return std::make_pair(next_north, false);
				}
				next_north.set_coord(x_coord, y_coord_last, my_world);
				return std::make_pair(next_north, true);
			}

		};

		// 
		template <std::size_t COUNT_NON_TARGET_ROBOTS>
		class partial_solution_record {
		public:
			robots_position_state<COUNT_NON_TARGET_ROBOTS> state;
			std::vector<std::shared_ptr<partial_solution_record>> predecessors;
			std::size_t steps;
		};

		/*template<class Field_Id_Type, std::size_t COUNT_NON_TARGET_ROBOTS>
		using partial_solutions_container =
			std::vector< // partitioning by target robot position id
			std::vector< // partitioning by additional robot x_coord
			std::map<
			robots_position_state<Field_Id_Type, COUNT_NON_TARGET_ROBOTS>,
			std::tuple<partial_solution_record<Field_Id_Type, COUNT_NON_TARGET_ROBOTS>, std::mutex>
			>
			>
			>;*/ // for multi threaded

		class robot_move {
		public:
			using robot_id_type = uint8_t;
			using robot_direction_type = uint8_t;

			static constexpr robot_direction_type NORTH{ 1 << 0 };
			static constexpr robot_direction_type EAST{ 1 << 1 };
			static constexpr robot_direction_type SOUTH{ 1 << 2 };
			static constexpr robot_direction_type WEST{ 1 << 3 };

			robot_id_type robot_id;
			robot_direction_type direction;

			robot_move(const robot_id_type& _robot_id, const robot_direction_type& _direction) : robot_id(_robot_id), direction(_direction) {}
		};

		template <std::size_t COUNT_NON_TARGET_ROBOTS>
		class partial_solution_connections {
		public:
			using Field_Id_Type = universal_field_id;
			using State_Type = robots_position_state<COUNT_NON_TARGET_ROBOTS>;


			using partial_solutions_map_type = std::map<State_Type, partial_solution_connections>;
			using map_iterator_type = typename partial_solutions_map_type::iterator;

			static constexpr std::size_t MAX{ std::numeric_limits<std::size_t>::max() };

			std::vector<std::tuple<map_iterator_type, robot_move>> predecessors;
			std::size_t steps{ MAX };
			std::size_t count_successors{ 0 }; // is leaf iff == 0
		};

		class move_candidate {
		public:
			robot_move move;
			std::pair<universal_field_id, bool> next_field_paired_enable;

			move_candidate(const robot_move& m, const std::pair<universal_field_id, bool>& n) : move(m), next_field_paired_enable(n) {}
		};

		template<std::size_t COUNT_NON_TARGET_ROBOTS = 3>
		inline void get_all_optimal_solutions(
			tobor_world_analyzer<COUNT_NON_TARGET_ROBOTS>& world_analyzer,
			const universal_field_id& p_target_field,
			const universal_field_id& p_target_robot,
			std::array<universal_field_id, COUNT_NON_TARGET_ROBOTS>&& p_other_robots
		) {

			using state_type = robots_position_state<COUNT_NON_TARGET_ROBOTS>;
			using connect_type = partial_solution_connections<COUNT_NON_TARGET_ROBOTS>;
			using partial_solutions_map_type = typename connect_type::partial_solutions_map_type;
			using map_iterator = typename connect_type::map_iterator_type;

			const auto initial_state = state_type(p_target_robot, std::move(p_other_robots));


			partial_solutions_map_type solutions_map;
			std::vector<map_iterator> to_be_explored;
			std::size_t index_next_exploration{ 0 };
			std::size_t optimal_solution_size{ std::numeric_limits<std::size_t>::max() };

			solutions_map[initial_state].steps = 0; //insert initial_state
			// predecessors are empty
			// is_leaf ist always true until explored


			to_be_explored.push_back(solutions_map.begin());

			world_analyzer.create_quick_move_table();

			while (index_next_exploration < to_be_explored.size()) {
				const auto& current_iterator{ to_be_explored[index_next_exploration] };

				//### if currrent to explore has optimal step number....
						// do not explore sub cases... since they bread sub-optimal solutions... (we can immediately stop exploring at all due to order in fifo chain)

				std::vector<move_candidate> candidates_for_successor_states;

				// get next fields in our world with respect to current state
				candidates_for_successor_states.emplace_back(
					robot_move(COUNT_NON_TARGET_ROBOTS, robot_move::WEST),
					world_analyzer.get_next_field_on_west_move(current_iterator->first.target_robot, current_iterator->first)
				);
				candidates_for_successor_states.emplace_back(
					robot_move(COUNT_NON_TARGET_ROBOTS, robot_move::EAST),
					world_analyzer.get_next_field_on_east_move(current_iterator->first.target_robot, current_iterator->first)
				);
				candidates_for_successor_states.emplace_back(
					robot_move(COUNT_NON_TARGET_ROBOTS, robot_move::NORTH),
					world_analyzer.get_next_field_on_north_move(current_iterator->first.target_robot, current_iterator->first)
				);
				candidates_for_successor_states.emplace_back(
					robot_move(COUNT_NON_TARGET_ROBOTS, robot_move::SOUTH),
					world_analyzer.get_next_field_on_south_move(current_iterator->first.target_robot, current_iterator->first)
				);

				for (std::size_t rob_id{ 0 }; rob_id < COUNT_NON_TARGET_ROBOTS; ++rob_id) {
					candidates_for_successor_states.emplace_back(
						robot_move(rob_id, robot_move::WEST),
						world_analyzer.get_next_field_on_west_move(current_iterator->first.other_robots_sorted[rob_id], current_iterator->first)
					);
					candidates_for_successor_states.emplace_back(
						robot_move(rob_id, robot_move::EAST),
						world_analyzer.get_next_field_on_east_move(current_iterator->first.other_robots_sorted[rob_id], current_iterator->first)
					);
					candidates_for_successor_states.emplace_back(
						robot_move(rob_id, robot_move::NORTH),
						world_analyzer.get_next_field_on_north_move(current_iterator->first.other_robots_sorted[rob_id], current_iterator->first)
					);
					candidates_for_successor_states.emplace_back(
						robot_move(rob_id, robot_move::SOUTH),
						world_analyzer.get_next_field_on_south_move(current_iterator->first.other_robots_sorted[rob_id], current_iterator->first)
					);
				}

				// check if reached goal
				for (std::size_t candidate{ 0 }; candidate < 4; ++candidate) {
					if (candidates_for_successor_states[candidate].next_field_paired_enable.first == p_target_field) {
						optimal_solution_size = current_iterator->second.steps + 1;
					}
				}

				// add candidates to map if they are valid:
				for (auto& c : candidates_for_successor_states) {
					if (c.next_field_paired_enable.second) { // there is a real move
						// create next state
						//current_iterator->first
						// add state to map
						// make leaf false
					}
				}


				/*
				auto [next_field_w, has_next_w] = world_analyzer.get_next_field_on_west_move(current_iterator->first.target_robot, current_iterator->first);
				auto [next_field_e, has_next_e] = world_analyzer.get_next_field_on_east_move(current_iterator->first.target_robot, current_iterator->first);
				auto [next_field_s, has_next_s] = world_analyzer.get_next_field_on_south_move(current_iterator->first.target_robot, current_iterator->first);
				auto [next_field_n, has_next_n] = world_analyzer.get_next_field_on_north_move(current_iterator->first.target_robot, current_iterator->first);

				// loop the following over all next states... using parameters.... WEST/EAST/...    robot id to be moved
				if (has_next_w) {
					state_type next_state;
					next_state.other_robots_sorted = current_iterator->first.other_robots_sorted;
					next_state.target_robot = nex_field_w;

					if (solutions_map[next_state].steps > current_iterator->second.steps + 1) { // check if found a new state
						current_iterator->second.is_leaf = false;
						solutions_map[next_state].steps = current_iterator->second.steps + 1;
						robot_move move;
						move.robot_id = COUNT_NON_TARGET_ROBOTS;
						move.direction = robot_move::WEST;
						solutions_map[next_state].predecessors.emplace_back(current_iterator, move);
						to_be_explored.push_back(); // iterator to new state....
						//### check for "reached goal state".
					}
					if (solutions_map[next_state].steps == current_iterator->second.steps + 1) { // check if found again a state with another optimal predecessor
						current_iterator->second.is_leaf = false;
						robot_move move;
						move.robot_id = COUNT_NON_TARGET_ROBOTS;
						move.direction = robot_move::WEST;
						solutions_map[next_state].predecessors.emplace_back(current_iterator, move);
					}
				}

				const auto& connections = std::get<0>(current_iter->second);
				std::size_t steps = connections.steps;

				// for all movable robots
					// for all directions
						// get next field
						// create next state

				++index_next_exploration;
			}





			auto initial_partial_solution = std::make_shared<partial_solution_record< universal_field_id, COUNT_NON_TARGET_ROBOTS>>();
			initial_partial_solution->state = initial_state;
			initial_partial_solution->steps = 0;


			//std::vector<std::shared_ptr<partial_solution_record< universal_field_id, COUNT_NON_TARGET_ROBOTS>>> to_be_explored;

			to_be_explored.push_back(std::move(initial_partial_solution));



			//std::size_t COUNT_THREADS{ 16 };

			auto all_partial_solutions = std::vector<std::forward_list<partial_solution_record>>(COUNT_THREADS);

			// single thread version:


			std::vector<all_partial_solutions::iterator> to_be_explored;
			std::vector<all_partial_solutions::iterator> to_be_explored_next;

			auto partial_solutions_map = all_partial_solutions();




			while (const auto & current_iter : to_be_explored) {
				//
				//current_iter->
				const auto& state = current_iter->first;
				const auto& connections = std::get<0>(current_iter->second);
				std::size_t steps = connections.steps;

				// for all movable robots
					// for all directions
						// get next field
						// create next state
						// put next state into map, if not already reached within less steps.
				*/
			}

		}


	}

}
