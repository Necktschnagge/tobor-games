#pragma once

#include "models.h"

#include <map>
#include <array>
#include <algorithm>


namespace tobor {

	namespace v1_0 {
		
		/**
		*	@brief Stores the cell ids which can be reached by moving in one of the four directions,
					assuming moving as far as possible until hitting a wall,
					also assuming there are no other obstacles than walls on the way.
					Should be used as one object per cell.
		*/
		class quick_moves_of_cell {

			using cell_id = universal_cell_id;

			using game_board = tobor_world;

		public:
			cell_id next_north; // ### cannot be const, because of ctor, should be only const-accessable!
			cell_id next_east;
			cell_id next_south;
			cell_id next_west;

			quick_moves_of_cell(const cell_id& start_field, const game_board& world) {
				// ### inefficient: try to use some recursive call on next east of left neighbour cell, if next_east is not equal to start cell
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


		/**
		*	@brief Stores the quick move information for all cells of the game's board.
		*/
		class quick_move_board {
		public:
			using cell_id = universal_cell_id;
			using game_board = tobor_world;

			// maps:   id |-> quick_moves_of_cell of cell with given id
			std::vector<quick_moves_of_cell> cells;

			quick_move_board() {}

			quick_move_board(const game_board& world) {
				cells.reserve(world.count_cells());
				for (std::size_t id = 0; id < world.count_cells(); ++id) {
					cell_id cell;
					cell.set_id(id, world);

					cells.emplace_back(quick_moves_of_cell(cell, world));
				}
			}
		};

		/**
		*	@brief Calculates successor cells of one pice to move at a time.
		*/
		template<std::size_t COUNT_NON_TARGET_PIECES>
		class move_one_piece_calculator {
		public:
			using world_type = tobor_world;

		private:
			const world_type& my_world;

			quick_move_board table;

		public:


			move_one_piece_calculator(const world_type& my_world) : my_world(my_world) {}

			inline void create_quick_move_table() {
				table = quick_move_board(my_world);
			}


			// put the following four functions together using some if constexpr. Use the direction as template parameter. ####
			// it may be better to not return a bool, just return the start cell itself if no move happens. (?)

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving west until obstacle.
			*/
			inline std::pair<universal_cell_id, bool> get_next_field_on_west_move(const universal_cell_id& start_cell, const positions_of_pieces<COUNT_NON_TARGET_PIECES>& state) {
				const std::size_t x_coord_start{ start_cell.get_x_coord() }; // ## use id instead, transposed_id respectively for other directions: less comparison operations in code, maybe not at runtime...
				const std::size_t y_coord{ start_cell.get_y_coord() };
				const universal_cell_id& next_without_obstacle{ table.cells[start_cell.get_id()].next_west };
				std::size_t x_coord_last{ next_without_obstacle.get_x_coord() };
				universal_cell_id next_west;
				if (x_coord_start == x_coord_last) {
					/* no move possible */
					return std::make_pair(next_west, false);
				}
				// looking for an obstacle...
				if (state.target_piece.get_y_coord() == y_coord) { // if we compare ids we will not have to check if in correct line.
					if (state.target_piece.get_x_coord() < x_coord_start && state.target_piece.get_x_coord() >= x_coord_last) {
						x_coord_last = state.target_piece.get_x_coord() + 1;
					}
				}
				for (auto& ntp : state.non_target_pieces) {
					if (ntp.get_y_coord() == y_coord) {
						if (ntp.get_x_coord() < x_coord_start && ntp.get_x_coord() >= x_coord_last) {
							x_coord_last = ntp.get_x_coord() + 1;
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

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving east until obstacle.
			*/
			inline std::pair<universal_cell_id, bool> get_next_field_on_east_move(const universal_cell_id& start_cell, const positions_of_pieces< COUNT_NON_TARGET_PIECES>& state) {
				const std::size_t x_coord_start{ start_cell.get_x_coord() };
				const std::size_t y_coord{ start_cell.get_y_coord() };
				const universal_cell_id& next_without_obstacle{ table.cells[start_cell.get_id()].next_east };
				std::size_t x_coord_last{ next_without_obstacle.get_x_coord() };
				universal_cell_id next_east;
				if (x_coord_start == x_coord_last) {
					/* no move possible */
					return std::make_pair(next_east, false);
				}
				// looking for an obstacle...
				if (state.target_piece.get_y_coord() == y_coord) {
					if (state.target_piece.get_x_coord() > x_coord_start && state.target_piece.get_x_coord() <= x_coord_last) {
						x_coord_last = state.target_piece.get_x_coord() - 1;
					}
				}
				for (auto& ntp : state.non_target_pieces) {
					if (ntp.get_y_coord() == y_coord) {
						if (ntp.get_x_coord() > x_coord_start && ntp.get_x_coord() <= x_coord_last) {
							x_coord_last = ntp.get_x_coord() - 1;
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

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving south until obstacle.
			*/
			inline std::pair<universal_cell_id, bool> get_next_field_on_south_move(const universal_cell_id& start_cell, const positions_of_pieces< COUNT_NON_TARGET_PIECES>& state) {
				const std::size_t x_coord{ start_cell.get_x_coord() };
				const std::size_t y_coord_start{ start_cell.get_y_coord() };
				const universal_cell_id& next_without_obstacle{ table.cells[start_cell.get_id()].next_south };
				std::size_t y_coord_last{ next_without_obstacle.get_y_coord() };
				universal_cell_id next_south;
				if (y_coord_start == y_coord_last) {
					/* no move possible */
					return std::make_pair(next_south, false);
				}
				// looking for an obstacle...
				if (state.target_piece.get_x_coord() == x_coord) {
					if (state.target_piece.get_y_coord() < y_coord_start && state.target_piece.get_y_coord() >= y_coord_last) {
						y_coord_last = state.target_piece.get_y_coord() + 1;
					}
				}
				for (auto& ntp : state.non_target_pieces) {
					if (ntp.get_x_coord() == x_coord) {
						if (ntp.get_y_coord() < y_coord_start && ntp.get_y_coord() >= y_coord_last) {
							y_coord_last = ntp.get_y_coord() + 1;
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

			/**
			*	@brief Calculates the successor cell to reach starting at \p start_cell moving north until obstacle.
			*/
			inline std::pair<universal_cell_id, bool> get_next_field_on_north_move(const universal_cell_id& start_cell, const positions_of_pieces<  COUNT_NON_TARGET_PIECES>& state) {
				const std::size_t x_coord{ start_cell.get_x_coord() };
				const std::size_t y_coord_start{ start_cell.get_y_coord() };
				const universal_cell_id& next_without_obstacle{ table.cells[start_cell.get_id()].next_north };
				std::size_t y_coord_last{ next_without_obstacle.get_y_coord() };
				universal_cell_id next_north;
				if (y_coord_start == y_coord_last) {
					/* no move possible */
					return std::make_pair(next_north, false);
				}
				// looking for an obstacle...
				if (state.target_piece.get_x_coord() == x_coord) {
					if (state.target_piece.get_y_coord() > y_coord_start && state.target_piece.get_y_coord() <= y_coord_last) {
						y_coord_last = state.target_piece.get_y_coord() - 1;
					}
				}
				for (auto& ntp : state.non_target_pieces) {
					if (ntp.get_x_coord() == x_coord) {
						if (ntp.get_y_coord() > y_coord_start && ntp.get_y_coord() <= y_coord_last) {
							y_coord_last = ntp.get_y_coord() - 1;
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





		/*
		template <std::size_t COUNT_NON_TARGET_PIECES>
		class partial_solution_record {
		public:
			positions_of_pieces<COUNT_NON_TARGET_PIECES> state;
			std::vector<std::shared_ptr<partial_solution_record>> predecessors;
			std::size_t steps;
		};
		*/

		/*template<class cell_id, std::size_t COUNT_NON_TARGET_PIECES>
		using partial_solutions_container =
			std::vector< // partitioning by target ntp position id
			std::vector< // partitioning by additional ntp x_coord
			std::map<
			positions_of_pieces<cell_id, COUNT_NON_TARGET_PIECES>,
			std::tuple<partial_solution_record<cell_id, COUNT_NON_TARGET_PIECES>, std::mutex>
			>
			>
			>;*/ // for multi threaded


		/*
		*	@brief Equivalent to a pair of a piece_id and a direction where to move it.
		*/
		class piece_move {
		public:
			using piece_id_type = uint8_t;
			using move_direction_type = uint8_t;

			static constexpr move_direction_type NORTH{ 1 << 0 };
			static constexpr move_direction_type EAST{ 1 << 1 };
			static constexpr move_direction_type SOUTH{ 1 << 2 };
			static constexpr move_direction_type WEST{ 1 << 3 };

			piece_id_type piece_id;
			move_direction_type direction;

			piece_move(const piece_id_type& _piece_id, const move_direction_type& _direction) : piece_id(_piece_id), direction(_direction) {}
		};



		template <std::size_t COUNT_NON_TARGET_PIECES>
		class partial_solution_connections {
		public:
			using cell_id = universal_cell_id;
			using State_Type = positions_of_pieces<COUNT_NON_TARGET_PIECES>;


			using partial_solutions_map_type = std::map<positions_of_pieces<COUNT_NON_TARGET_PIECES>, partial_solution_connections>;
			using map_iterator_type = typename partial_solutions_map_type::iterator;

			static constexpr std::size_t MAX{ std::numeric_limits<std::size_t>::max() };

			std::vector<std::tuple<map_iterator_type, piece_move>> predecessors;
			std::size_t steps{ MAX };
			std::size_t count_successors{ 0 }; // is leaf iff == 0
		};

		class move_candidate {
		public:
			piece_move move;
			std::pair<universal_cell_id, bool> next_field_paired_enable;

			move_candidate(const piece_move& m, const std::pair<universal_cell_id, bool>& n) : move(m), next_field_paired_enable(n) {}
		};

		template<std::size_t COUNT_NON_TARGET_PIECES = 3>
		inline std::size_t get_all_optimal_solutions(
			move_one_piece_calculator<COUNT_NON_TARGET_PIECES>& world_analyzer,
			const universal_cell_id& p_target_field,
			const universal_cell_id& p_target_robot,
			std::array<universal_cell_id, COUNT_NON_TARGET_PIECES>&& p_other_robots
		) {

			//using state_type = positions_of_pieces<COUNT_NON_TARGET_PIECES>; // remove this!
			//using connect_type = partial_solution_connections<COUNT_NON_TARGET_PIECES>; // remove this!
			//using partial_solutions_map_type = typename partial_solution_connections<COUNT_NON_TARGET_PIECES>::partial_solutions_map_type;
			using map_iterator = typename partial_solution_connections<COUNT_NON_TARGET_PIECES>::map_iterator_type;

			const auto initial_state = positions_of_pieces<COUNT_NON_TARGET_PIECES>(p_target_robot, std::move(p_other_robots));


			typename partial_solution_connections<COUNT_NON_TARGET_PIECES>::partial_solutions_map_type solutions_map;
			//type_helper::partial_solutions_map_type<COUNT_NON_TARGET_PIECES> solutions_map;
			std::vector<std::vector<map_iterator>> to_be_explored;
			std::size_t optimal_solution_size{ std::numeric_limits<std::size_t>::max() };

			solutions_map[initial_state].steps = 0; //insert initial_state
			// predecessors are empty
			// is_leaf ist always true until 

			// what if initial state is already final? check this!

			to_be_explored.push_back(std::vector<map_iterator>{solutions_map.begin()});

			world_analyzer.create_quick_move_table();

			for (std::size_t expand_size{ 0 }; expand_size < optimal_solution_size; ++expand_size) {
				to_be_explored.emplace_back(); // possibly invalidates iterators on sub-vectors, but on most compilers it will work anyway. But please do not rely on this behaviour.

				for (const auto& current_iterator : to_be_explored[expand_size]) {
					//const auto current_iterator{ to_be_explored[index_next_exploration] };

					std::vector<move_candidate> candidates_for_successor_states;

					// get next fields in our world with respect to current state
					candidates_for_successor_states.emplace_back(
						piece_move(COUNT_NON_TARGET_PIECES, piece_move::WEST),
						world_analyzer.get_next_field_on_west_move(current_iterator->first.target_piece, current_iterator->first)
					);
					candidates_for_successor_states.emplace_back(
						piece_move(COUNT_NON_TARGET_PIECES, piece_move::EAST),
						world_analyzer.get_next_field_on_east_move(current_iterator->first.target_piece, current_iterator->first)
					);
					candidates_for_successor_states.emplace_back(
						piece_move(COUNT_NON_TARGET_PIECES, piece_move::NORTH),
						world_analyzer.get_next_field_on_north_move(current_iterator->first.target_piece, current_iterator->first)
					);
					candidates_for_successor_states.emplace_back(
						piece_move(COUNT_NON_TARGET_PIECES, piece_move::SOUTH),
						world_analyzer.get_next_field_on_south_move(current_iterator->first.target_piece, current_iterator->first)
					);

					for (std::size_t rob_id{ 0 }; rob_id < COUNT_NON_TARGET_PIECES; ++rob_id) {
						candidates_for_successor_states.emplace_back(
							piece_move(static_cast<piece_move::piece_id_type>(rob_id), piece_move::WEST),
							world_analyzer.get_next_field_on_west_move(current_iterator->first.non_target_pieces[rob_id], current_iterator->first)
						);
						candidates_for_successor_states.emplace_back(
							piece_move(static_cast<piece_move::piece_id_type>(rob_id), piece_move::EAST),
							world_analyzer.get_next_field_on_east_move(current_iterator->first.non_target_pieces[rob_id], current_iterator->first)
						);
						candidates_for_successor_states.emplace_back(
							piece_move(static_cast<piece_move::piece_id_type>(rob_id), piece_move::NORTH),
							world_analyzer.get_next_field_on_north_move(current_iterator->first.non_target_pieces[rob_id], current_iterator->first)
						);
						candidates_for_successor_states.emplace_back(
							piece_move(static_cast<piece_move::piece_id_type>(rob_id), piece_move::SOUTH),
							world_analyzer.get_next_field_on_south_move(current_iterator->first.non_target_pieces[rob_id], current_iterator->first)
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
							// c.next_field_paired_enable.first; // new cell id
							auto new_state = positions_of_pieces<COUNT_NON_TARGET_PIECES>(current_iterator->first);

							if (c.move.piece_id < COUNT_NON_TARGET_PIECES) {
								new_state.non_target_pieces[c.move.piece_id] = c.next_field_paired_enable.first;
								new_state.sort_non_target_pieces();
							}
							else {
								new_state.target_piece = c.next_field_paired_enable.first;
							}

							if (solutions_map[new_state].steps > current_iterator->second.steps + 1) { // check if path to successor state is an optimal one (as far as we have seen)
								// to make it more efficient: use an .insert(...) get the iterator to newly inserted element.

								// hint: on map entry creation by if condition, steps defaults to MAX value of std::size_t

								// delete all predecessors!
								//for (const auto& tuple : solutions_map[new_state].predecessors) { // it is always empty because of fifo order of to_be_explored
								//	--(std::get<0>(tuple)->second.count_successors);
								//}
								//solutions_map[new_state].predecessors.clear();

								solutions_map[new_state].steps = current_iterator->second.steps + 1;
								solutions_map[new_state].predecessors.emplace_back(current_iterator, c.move);
								++(current_iterator->second.count_successors);
								to_be_explored[expand_size + 1].push_back(solutions_map.find(new_state));
							}
							else {
								if (solutions_map[new_state].steps == current_iterator->second.steps + 1) {
									solutions_map[new_state].predecessors.emplace_back(current_iterator, c.move);
									++(current_iterator->second.count_successors);
									// to_be_explored.push_back(solutions_map.find(new_state)); don't add, already added on first path reaching new_state
								}
							}
						}
					}
				}

			}
			return optimal_solution_size;

		}


	}

}
