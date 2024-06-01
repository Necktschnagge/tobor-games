#pragma once

#include "all_solver.h" // ..., tobor::v1_0::tobor_world

#include <functional>
#include <optional>

struct EngineTypeSet {

	using world_type = tobor::v1_1::dynamic_rectangle_world<uint16_t, uint8_t>;

	using cell_id_type = tobor::v1_1::min_size_cell_id<world_type>;

	using piece_quantity_type = tobor::v1_1::pieces_quantity<uint8_t, 1, 3>;

	using piece_id_type = tobor::v1_1::piece_id<piece_quantity_type>;

	using positions_of_pieces_type_solver = tobor::v1_1::positions_of_pieces<piece_quantity_type, cell_id_type, true, true>;

	using positions_of_pieces_type_interactive = tobor::v1_1::augmented_positions_of_pieces<piece_quantity_type, cell_id_type, true, true>;

	using piece_move_type = tobor::v1_1::piece_move<piece_id_type>;

	using quick_move_cache_type = tobor::v1_1::quick_move_cache<world_type>;

	using move_one_piece_calculator_type = tobor::v1_1::move_one_piece_calculator<cell_id_type, quick_move_cache_type, piece_move_type>;

	using move_path_type = tobor::v1_1::move_path<piece_move_type>;

	using state_path_type_interactive = tobor::v1_1::state_path<positions_of_pieces_type_interactive>;

	using state_path_type_solver = tobor::v1_1::state_path<positions_of_pieces_type_solver>;

};


class SolverEnvironment : public EngineTypeSet {

	struct piece_change_decoration {
		static constexpr std::size_t MAX{ std::numeric_limits<std::size_t>::max() };

		std::size_t min_piece_change_distance;
		std::vector<positions_of_pieces_type_interactive> optimal_successors;
		//std::size_t count_total_paths_from_here;

		piece_change_decoration(
			std::size_t min_piece_change_distance,
			std::vector<positions_of_pieces_type_interactive> optimal_successors
			//, std::size_t count_total_paths_from_here
		) :
			min_piece_change_distance(min_piece_change_distance),
			optimal_successors(optimal_successors)
			//, count_total_paths_from_here(count_total_paths_from_here)
		{}
	};

	using piece_change_decoration_vector = std::vector<piece_change_decoration>;

	using distance_exploration_type = tobor::v1_1::distance_exploration<move_one_piece_calculator_type, positions_of_pieces_type_solver>;

	using bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_solver, std::vector<bool>>;

	using naked_bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_solver, void>;

	using pretty_evaluation_bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_interactive, piece_change_decoration_vector>;

	using path_classificator_type = tobor::v1_1::path_classificator<positions_of_pieces_type_solver>;

	using optimal_solutions_vector = std::vector<std::pair<state_path_type_interactive, move_path_type>>;



	//std::size_t selected_solution_index;

	distance_exploration_type distance_explorer;

	optimal_solutions_vector optimal_solutions;



	/**
	*	@brief Explores from map_iter recursively to final states and build decorations from final states to initial states
	*   For counting min piece changes until final state
	*/
	void prettiness_decoration_helper(pretty_evaluation_bigraph_type& pretty_evaluation_bigraph, pretty_evaluation_bigraph_type::map_iterator_type map_iter, const move_one_piece_calculator_type& engine) {
		if (!map_iter->second.labels.empty()) {
			return; // this map entry and all reachable direct and indirect successor states must have been decorated correctly
		}
		if (map_iter->second.successors.empty()) {
			// This is a final state, not yet decorated.
			for (std::size_t n{ 0 }; n < piece_quantity_type::COUNT_ALL_PIECES; ++n) {
				map_iter->second.labels.emplace_back(
					0, // 0 piece changes left when in final state
					std::vector<positions_of_pieces_type_interactive>() // no successors
				);
			}
			return;
		}
		// we now have an undecorated state that is not final.
		//first make sure all successors have been decorated:
		for (const auto& succ : map_iter->second.successors) {
			auto map_jter = pretty_evaluation_bigraph.map.find(succ);
			if (map_jter == pretty_evaluation_bigraph.map.end()) {
				throw 0; //#### error in bigraph. invalid bigraph.
			}
			prettiness_decoration_helper(pretty_evaluation_bigraph, map_jter, engine);
		}

		//now calculate current state's decoration using the successor decorations.

		// initialize with MAX distance
		for (std::size_t i{ 0 }; i < piece_quantity_type::COUNT_ALL_PIECES; ++i) {
			map_iter->second.labels.emplace_back(
				piece_change_decoration::MAX,
				std::vector<positions_of_pieces_type_interactive>() // no successors
			);
		}

		for (const auto& succ_state : map_iter->second.successors) {

			// find successor
			auto succ_jter = pretty_evaluation_bigraph.map.find(succ_state);

			// obtain SELECTED_PIECE id
			piece_move_type move = engine.state_minus_state(succ_state, map_iter->first); // exceptions here!!
			const piece_quantity_type::int_type SELECTED_PIECE = move.pid.value;

			// obtain SELECTED_PIECE id after move
			positions_of_pieces_type_interactive from_state(map_iter->first);
			from_state.reset_permutation();
			positions_of_pieces_type_interactive to_state = engine.state_plus_move(from_state, move);

			const piece_quantity_type::int_type SELECTED_PIECE_AFTER{
				[&]() {
					for (piece_quantity_type::int_type i{ 0 }; i < piece_quantity_type::COUNT_ALL_PIECES; ++i) {
						if (to_state.get_permutation()[i] == SELECTED_PIECE) {
							return i;
						}
					}
					throw 0;
				}()
			};

			const std::size_t SUB_DISTANCE{ succ_jter->second.labels[SELECTED_PIECE_AFTER].min_piece_change_distance };

			for (piece_quantity_type::int_type i{ 0 }; i < piece_quantity_type::COUNT_ALL_PIECES; ++i) {
				const std::size_t UPDATE_DISTANCE{ SUB_DISTANCE + (SELECTED_PIECE != i) };
				if (UPDATE_DISTANCE < map_iter->second.labels[i].min_piece_change_distance) {

					map_iter->second.labels[i].optimal_successors.clear();
					map_iter->second.labels[i].min_piece_change_distance = UPDATE_DISTANCE;
					map_iter->second.labels[i].optimal_successors.push_back(succ_state);

				}
			}
		}
	}

public:
	SolverEnvironment(const positions_of_pieces_type_interactive& initial_state) :
		distance_explorer(initial_state.naked()),
		optimal_solutions(),
	{
		run();
	}

	// call run from constructor########
	[[nodiscard]] inline bool run(
		const move_one_piece_calculator_type& move_engine,
		const positions_of_pieces_type_interactive& initial_state,
		const cell_id_type& target_cell,
		std::function<void(const std::string&)> status_callback = nullptr,
		std::size_t MAX_DEPTH = distance_exploration_type::SIZE_TYPE_MAX,
		bool explicilty_create_all_optimal_solution_paths = false
	) {

		// explore...
		if (status_callback) status_callback("Exploring state space until target...");
		auto optimal_depth = distance_explorer.explore_until_target(move_engine, target_cell, MAX_DEPTH);

		// ### inside this call, log every distance level as a progress bar
		if (optimal_depth == decltype(distance_explorer)::SIZE_TYPE_MAX) {
			return false;
		}


		bigraph_type bigraph;

		if (status_callback) status_callback("Extracting solution state graph...");
		distance_explorer.get_simple_bigraph(move_engine, target_cell, bigraph);

		// bigraph is now a sub - bigraph of exploration space where all states are decorated with an empty std::vector<bool>

		if (status_callback) status_callback("Partition state graph...");
		std::size_t count_partitions = path_classificator_type::make_state_graph_path_partitioning(bigraph);

		// bigraph decoration std::vector<bool> now assigns a "color" (= index of vector where bit is set true) to every state of a partition of solution paths

		std::vector<naked_bigraph_type> partition_bigraphs;

		if (status_callback) status_callback("Extracting subgraph for each partition...");
		for (std::size_t i{ 0 }; i < count_partitions; ++i) {
			partition_bigraphs.emplace_back();
			path_classificator_type::extract_subgraph_by_label(bigraph, i, partition_bigraphs.back());
		}

		// Now in partition_bigraphs there is a separate bigraph for each color i.e. for each partition.


		if (!explicilty_create_all_optimal_solution_paths) {
			std::vector<pretty_evaluation_bigraph_type> partition_bigraphs_decorated;

			if (status_callback) status_callback("Decorating each partition subgraph for prettiness evaluation...");
			for (std::size_t i{ 0 }; i < partition_bigraphs.size(); ++i) {

				// simulation copy here
				partition_bigraphs_decorated.emplace_back();
				auto iter_to_single_initial_state = tobor::v1_1::bigraph_operations::bigraph_simulation_copy<positions_of_pieces_type_solver, void, positions_of_pieces_type_interactive, piece_change_decoration_vector, cell_id_type, quick_move_cache_type, piece_move_type>::
					copy(partition_bigraphs[i], partition_bigraphs_decorated[i], initial_state, move_engine);

				prettiness_decoration_helper(partition_bigraphs_decorated[i], iter_to_single_initial_state, move_engine);
			}

			//// TODO here: pick one of the prettiest solutions in each partition.

		}
		else {



			std::vector<std::vector<state_path_type_solver>> partitioned_state_paths;

			std::vector<std::vector<state_path_type_interactive>> partitioned_augmented_state_paths;

			std::vector<std::vector<move_path_type>> partitioned_color_aware_move_paths;

			std::vector<std::vector<std::pair<state_path_type_interactive, move_path_type>>> partitioned_pairs; // should replace the two above




			if (status_callback) status_callback("Generating state_paths ...");
			for (std::size_t i{ 0 }; i < partition_bigraphs.size(); ++i) {
				partitioned_state_paths.emplace_back(path_classificator_type::extract_all_state_paths(partition_bigraphs[i]));
			}


			if (status_callback) status_callback("Generating move_paths ...");
			for (std::size_t i{ 0 }; i < partitioned_state_paths.size(); ++i) {

				partitioned_augmented_state_paths.emplace_back();
				partitioned_color_aware_move_paths.emplace_back();
				partitioned_pairs.emplace_back();

				for (const auto& state_path : partitioned_state_paths[i]) {

					auto color_agnostic_move_path = tobor::v1_1::move_path<piece_move_type>(state_path, move_engine);

					partitioned_augmented_state_paths.back().push_back(
						color_agnostic_move_path.apply(initial_state, move_engine)
					);

					partitioned_color_aware_move_paths.back().push_back(
						move_path_type::extract_unsorted_move_path(partitioned_augmented_state_paths.back().back(), move_engine)
					);

					partitioned_pairs.back().emplace_back(partitioned_augmented_state_paths.back().back(), partitioned_color_aware_move_paths.back().back());
				}
			}

			if (status_callback) status_callback("Prettiness sorting inside equivalence classes...");
			for (auto& equivalence_class : partitioned_color_aware_move_paths) {
				std::sort(equivalence_class.begin(), equivalence_class.end(), move_path_type::antiprettiness_relation);
			}
			for (auto& equivalence_class : partitioned_pairs) {
				std::sort(equivalence_class.begin(), equivalence_class.end(), [](const auto& pair_l, const auto& pair_r) { return move_path_type::antiprettiness_relation(pair_l.second, pair_r.second); });
			}


			for (const auto& vec : partitioned_pairs) {
				optimal_solutions.push_back(vec[0]);
			}
		}

		return true;
	}

	[[deprecated]] inline void select_solution(const std::size_t& index) { /// remove this ####
		selected_solution_index = index;
		if (!(selected_solution_index < partition_bigraphs.size())) {
			selected_solution_index = 0;
		}
	}

	/**
	*	@brief Returns the number of solutions found by solving.
	*/
	std::size_t solutions_size() const {
		throw 0;
	}

	/**
	*	@brief Returns an error code telling the reason why no path to target cell was found by solver.
	*	@details 0 -> No Error, 1 -> Reached MAX_DEPTH, 2 -> Entirely explored all reachable states
	*/
	uint8_t error_code() const {
		throw 0;
	}

	/**
	*	@brief Returns an optimal solution state path.
	*	@param index has to be less than \p solution_size()
	*/
	state_path_type_interactive get_solution(std::size_t index) const {
		(void)index;
		throw 0;
	}

};


class GameController : public EngineTypeSet {

private:

	/* data */

	world_type _world;

	move_one_piece_calculator_type _move_engine;

	state_path_type_interactive _path;

	std::vector<uint8_t> _color_permutation;

	cell_id_type _target_cell;

	std::optional<SolverEnvironment> _solver;

	std::size_t _solver_begin_index;

	std::size_t _solution_index;

public:

	GameController(
		const world_type& world,
		const positions_of_pieces_type_interactive& initial_state,
		const cell_id_type& target_cell,
		const std::vector<uint8_t>& color_permutation
	) :
		_world(world),
		_move_engine(this->_world),
		_path({ initial_state }),
		_color_permutation(color_permutation),
		_target_cell(target_cell),
		_solver(),
		_solver_begin_index(0),
		_solution_index(0)
	{}



	/* non-modifying */

	const positions_of_pieces_type_interactive& current_state() const { return _path.vector().back(); }

	inline bool is_final() const { return current_state().is_final(_target_cell); }

	inline bool is_initial() const { return _path.vector().size() == 1; }

	inline const std::vector<uint8_t>& color_permutation() const { return _color_permutation; }

	inline const world_type& world() const { return _world; }

	inline const cell_id_type& target_cell() const { return _target_cell; }

	inline std::size_t depth() const { return _path.vector().size() - 1; }

	/* modifying */

	inline uint8_t move(const piece_id_type& piece_id, const tobor::v1_0::direction& direction) {
		if (_solver) return 4;

		if (is_final()) return 2;

		auto next_state = _move_engine.successor_state(current_state(), piece_id, direction);

		if (next_state == current_state()) return 1;

		_path += next_state;

		return 0;
	}

	inline void undo() {
		if (_solver) return; // or stop solver if undoing out of solver (?)

		if (_path.vector().size() > 1) {
			_path.vector().pop_back();
		}
	}

	void start_solver(std::function<void(const std::string&)> status_callback = nullptr);

	inline void reset_solver_steps() {
		// go back to solver_begin_index
		_path.vector().erase(_path.vector().begin() + _solver_begin_index, _path.vector().end());
	}

	void stop_solver() {
		_solver_begin_index = 0;
		_solver.reset();
	}

	inline void select_solution(const std::size_t& index) {
		if (_solver) {
			_solution_index = index;
			//_solver.value().select_solution(index);
			reset_solver_steps();
		}
	}

	void move_by_solver(bool forward);

	inline state_path_type_interactive path() const noexcept {
		return _path;
	}

	/*
	inline const std::vector<std::vector<std::pair<state_path_type_interactive, move_path_type>>>& partitioned_pairs() const {
		if (!_solver) {
			throw 0;
		}
		return _solver.value().partitioned_pairs;
	}
	*/

	~GameController() {}

	// todo: functions below:

	//move_path_type& get_selected_solution_representant(std::size_t index); no longer supported!

	// must implement copy / move ctor and operator=
};

