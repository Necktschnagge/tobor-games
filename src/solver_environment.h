#pragma once


#include "engine/bigraph_operations.h"
#include "engine/path_classificator.h"
#include "engine/distance_exploration.h"

#include "engine_typeset.h"

#include <functional>
#include <optional>

/**
*	@brief Encapsulation of solver usage, RAII friendly, for specific initial state and target cell.
*
*	@details Runs the solver on construction to produce a vector of optimal solutions.
*/
template<class Pieces_Quantity_T>
class SolverEnvironment {
public:


	using engine_typeset = ClassicEngineTypeSet<Pieces_Quantity_T>;


	using state_path_type_interactive = typename engine_typeset::state_path_type_interactive;

	using state_path_type_solver = typename engine_typeset::state_path_type_solver;

	using move_path_type = typename engine_typeset::move_path_type;

	using positions_of_pieces_type_interactive = typename engine_typeset::positions_of_pieces_type_interactive;

	using positions_of_pieces_type_solver = typename engine_typeset::positions_of_pieces_type_solver;

	using move_engine_type = typename engine_typeset::move_engine_type;

	using cell_id_type = typename engine_typeset::cell_id_type;

	using pieces_quantity_type = typename engine_typeset::pieces_quantity_type;

	using piece_move_type = typename engine_typeset::piece_move_type;

	using quick_move_cache_type = typename engine_typeset::quick_move_cache_type;



	using  piece_quantity_int_type = typename pieces_quantity_type::int_type;



	using optimal_solutions_vector = std::vector<std::pair<state_path_type_interactive, move_path_type>>;


private:

	/**
	*	@brief Graph node annotation for state graphs.
	*/
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

	using distance_exploration_type = tobor::v1_1::vector_distance_exploration<move_engine_type, positions_of_pieces_type_solver>;

	using bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_solver, std::vector<bool>>;

	using naked_bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_solver, void>;

	using pretty_evaluation_bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_interactive, piece_change_decoration_vector>;

	using pretty_evaluation_bigraph_map_iterator_type = typename pretty_evaluation_bigraph_type::map_iterator_type;

	using path_classificator_type = tobor::v1_1::path_classificator<positions_of_pieces_type_solver>;


	/** data **/

	positions_of_pieces_type_interactive _initial_state;

	cell_id_type _target_cell;

	const move_engine_type& _move_engine;

	uint8_t _status_code;

	distance_exploration_type _distance_explorer;

	optimal_solutions_vector _optimal_solutions;


	/**
	*	@brief Explores from map_iter_root recursively via successor states until final states and build decorations from final states to given initial state \p map_iter_root
	*	@details Purpose is counting min piece changes until final state.
	*	Invariant that must be provided: If a state has labels then this state and all its direct and indirect successors must have been evaluated and their labels are set correclty.
	*/
	void build_prettiness_decoration(pretty_evaluation_bigraph_type& pretty_evaluation_bigraph, pretty_evaluation_bigraph_map_iterator_type map_iter_root, const move_engine_type& engine) {
		if (!map_iter_root->second.labels.empty()) {
			return; // this map entry and all reachable direct and indirect successor states must have been decorated correctly
		}
		if (map_iter_root->second.successors.empty()) {
			// This is a final state, not yet decorated.
			for (std::size_t n{ 0 }; n < pieces_quantity_type::COUNT_ALL_PIECES; ++n) {
				map_iter_root->second.labels.emplace_back(
					0, // 0 piece changes left when in final state
					std::vector<positions_of_pieces_type_interactive>() // no successors
				);
			}
			return;
		}
		// we now have an undecorated state that is not final.
		//first make sure all successors have been decorated:
		for (const auto& succ : map_iter_root->second.successors) {
			auto map_jter = pretty_evaluation_bigraph.map.find(succ);
			if (map_jter == pretty_evaluation_bigraph.map.end()) {
				throw 0; //#### error in bigraph. invalid bigraph.
			}
			build_prettiness_decoration(pretty_evaluation_bigraph, map_jter, engine);
		}

		//now calculate current state's decoration using the successor decorations.

		// initialize with MAX distance
		for (std::size_t i{ 0 }; i < pieces_quantity_type::COUNT_ALL_PIECES; ++i) {
			map_iter_root->second.labels.emplace_back(
				piece_change_decoration::MAX,
				std::vector<positions_of_pieces_type_interactive>() // no successors
			);
		}

		for (const auto& succ_state : map_iter_root->second.successors) {

			// find successor
			auto succ_jter = pretty_evaluation_bigraph.map.find(succ_state);

			// obtain SELECTED_PIECE id
			piece_move_type move = engine.state_minus_state(succ_state, map_iter_root->first); // exceptions here!!
			const piece_quantity_int_type SELECTED_PIECE = move.pid.value;

			// obtain SELECTED_PIECE id after move
			positions_of_pieces_type_interactive from_state(map_iter_root->first);
			from_state.reset_permutation();
			positions_of_pieces_type_interactive to_state = engine.successor_state(from_state, move);

			const piece_quantity_int_type SELECTED_PIECE_AFTER{
				[&]() {
					for (piece_quantity_int_type i{ 0 }; i < pieces_quantity_type::COUNT_ALL_PIECES; ++i) {
						if (to_state.permutation()[i] == SELECTED_PIECE) {
							return i;
						}
					}
					throw 0;
				}()
			};

			const std::size_t SUB_DISTANCE{ succ_jter->second.labels[SELECTED_PIECE_AFTER].min_piece_change_distance };

			for (piece_quantity_int_type i{ 0 }; i < pieces_quantity_type::COUNT_ALL_PIECES; ++i) {
				const std::size_t UPDATE_DISTANCE{ SUB_DISTANCE + (SELECTED_PIECE != i) };
				if (UPDATE_DISTANCE < map_iter_root->second.labels[i].min_piece_change_distance) {

					map_iter_root->second.labels[i].optimal_successors.clear();
					map_iter_root->second.labels[i].min_piece_change_distance = UPDATE_DISTANCE;
					map_iter_root->second.labels[i].optimal_successors.push_back(succ_state);

				}
			}
		}
	}

	state_path_type_interactive get_representant(pretty_evaluation_bigraph_type& pretty_evaluation_bigraph, pretty_evaluation_bigraph_map_iterator_type map_iter_root
		//, const move_engine_type& engine
	) {
		state_path_type_interactive result;

		while (true) {
			result.vector().push_back(map_iter_root->first);
			auto piece_select_iter = std::min_element(
				map_iter_root->second.labels.cbegin(),
				map_iter_root->second.labels.cend(),
				[](const piece_change_decoration& l, const piece_change_decoration& r) { return l.min_piece_change_distance < r.min_piece_change_distance; }
			);
			if (piece_select_iter->optimal_successors.empty()) {
				return result;
			}
			const auto successor_state{ piece_select_iter->optimal_successors.front() };
			map_iter_root = pretty_evaluation_bigraph.map.find(successor_state);
		}
	}


	inline uint8_t dynamic_programming_prettiness_evaluation(
		const std::vector<naked_bigraph_type>& partition_bigraphs,
		std::function<void(const std::string&)> status_callback = nullptr
	) {
		std::vector<pretty_evaluation_bigraph_type> partition_bigraphs_decorated;

		for (std::size_t i{ 0 }; i < partition_bigraphs.size(); ++i) {

			if (status_callback) status_callback("Simulation-copying from solver state type to interactive state type of partition graphs...");
			// simulation copy here
			partition_bigraphs_decorated.emplace_back();
			auto iter_to_single_initial_state = tobor::v1_1::bigraph_operations::bigraph_simulation_copy<positions_of_pieces_type_solver, void, positions_of_pieces_type_interactive, piece_change_decoration_vector, cell_id_type, quick_move_cache_type, piece_move_type>::
				copy(partition_bigraphs[i], partition_bigraphs_decorated[i], _initial_state, _move_engine);

			if (status_callback) status_callback("Decorating partition subgraph for prettiness evaluation...");
			build_prettiness_decoration(partition_bigraphs_decorated[i], iter_to_single_initial_state, _move_engine);

			if (status_callback) status_callback("Selecting partition representant according to prettiness ranking...");
			const state_path_type_interactive representant{ get_representant(partition_bigraphs_decorated[i], iter_to_single_initial_state) };

			const move_path_type color_aware_move_path{ move_path_type::extract_unsorted_move_path(representant, _move_engine) };

			_optimal_solutions.emplace_back(representant, color_aware_move_path);
		}
		return 0; // status code: OK
	}

	inline uint8_t explicit_move_path_prettiness_evaluation(
		const std::vector<naked_bigraph_type>& partition_bigraphs,
		std::function<void(const std::string&)> status_callback = nullptr
	) {

		std::vector<std::vector<state_path_type_solver>> partitioned_state_paths;

		if (status_callback) status_callback("Generating state_paths ...");
		for (std::size_t i{ 0 }; i < partition_bigraphs.size(); ++i) {
			partitioned_state_paths.emplace_back(path_classificator_type::extract_all_state_paths(partition_bigraphs[i]));
		}

		std::vector<std::vector<std::pair<state_path_type_interactive, move_path_type>>> partitioned_path_pairs; // should replace the two above

		if (status_callback) status_callback("Generating augmented state_paths and color-aware move_paths...");

		for (std::size_t i{ 0 }; i < partitioned_state_paths.size(); ++i) {

			partitioned_path_pairs.emplace_back();

			for (const auto& state_path : partitioned_state_paths[i]) {

				const move_path_type color_agnostic_move_path{ move_path_type(state_path, _move_engine) };

				const state_path_type_interactive augmented_state_path{ color_agnostic_move_path.apply(_initial_state, _move_engine) };

				const move_path_type color_aware_move_path{ move_path_type::extract_unsorted_move_path(augmented_state_path, _move_engine) };

				partitioned_path_pairs.back().emplace_back(augmented_state_path, color_aware_move_path);
			}
		}

		if (status_callback) status_callback("Finding prettiest element inside each equivalence class...");
		for (auto& equivalence_class : partitioned_path_pairs) {
			auto min_iter = std::min_element(equivalence_class.begin(), equivalence_class.end(), [](const auto& pair_l, const auto& pair_r) { return move_path_type::antiprettiness_relation(pair_l.second, pair_r.second); });
			if (min_iter != equivalence_class.end()) {
				_optimal_solutions.push_back(*min_iter);
			}
		}

		// ### also add size of equivalence classes to solutions

		return 0; // status code: OK
	}

	inline uint8_t extract_solution_from_state_space(
		std::function<void(const std::string&)> status_callback = nullptr,
		uint8_t SELECT_STRATEGY = 0
	) {

		bigraph_type bigraph;

		if (status_callback) status_callback("Extracting solution state graph...");
		_distance_explorer.get_simple_bigraph(_move_engine, _target_cell, bigraph);

		// bigraph is now a sub - bigraph of exploration space where all states are decorated with an empty std::vector<bool>

		if (status_callback) status_callback("Partition optimal solutions...");
		std::size_t count_partitions = path_classificator_type::make_state_graph_path_partitioning(bigraph);

		// bigraph decoration std::vector<bool> now assigns a "color" (= index of vector where bit is set true) to every state of a partition of solution paths

		std::vector<naked_bigraph_type> partition_bigraphs;

		if (status_callback) status_callback("Extracting subgraph for each partition...");
		for (std::size_t i{ 0 }; i < count_partitions; ++i) {
			partition_bigraphs.emplace_back();
			path_classificator_type::extract_subgraph_by_label(bigraph, i, partition_bigraphs.back());
		}

		// Now in partition_bigraphs there is a separate bigraph for each color i.e. for each partition.

		if (SELECT_STRATEGY == 0) {
			return dynamic_programming_prettiness_evaluation(partition_bigraphs, status_callback);
		}
		else if (SELECT_STRATEGY == 1) {
			return explicit_move_path_prettiness_evaluation(partition_bigraphs, status_callback);
		}
		else {
			// DEFAULT
			return dynamic_programming_prettiness_evaluation(partition_bigraphs, status_callback);
		}
	}

	inline uint8_t run_solver_toolchain(
		std::function<void(const std::string&)> status_callback = nullptr,
		std::size_t MAX_DEPTH = distance_exploration_type::SIZE_TYPE_MAX,
		uint8_t SELECT_STRATEGY = 0
	) {

		// explore...
		if (status_callback) status_callback("Exploring state space until target...");
		auto optimal_depth = _distance_explorer.explore_until_target(_move_engine, _target_cell, MAX_DEPTH);
		// ### inside this call, log every distance level as a progress bar


		if (optimal_depth == decltype(_distance_explorer)::SIZE_TYPE_MAX) {
			// did not find any solution whithin MAX_DEPTH
			return _status_code = 1; // NOT FOUND WITHIN MAX_DEPTH
		}

		return _status_code = extract_solution_from_state_space(status_callback, SELECT_STRATEGY);
	}


public:
	/**
	*	@brief Constructs a SolverEnvironment object, running the solver for a specified initial state and target cell.
	*/
	SolverEnvironment(
		const positions_of_pieces_type_interactive& initial_state,
		const cell_id_type& target_cell,
		const move_engine_type& move_engine,
		std::function<void(const std::string&)> status_callback = nullptr,
		std::size_t MAX_DEPTH = distance_exploration_type::SIZE_TYPE_MAX
	) :
		_initial_state(initial_state),
		_target_cell(target_cell),
		_move_engine(move_engine),
		_status_code(0),
		_distance_explorer(initial_state.naked()),
		_optimal_solutions()
	{
		run_solver_toolchain(status_callback, MAX_DEPTH, 0);
	}

	/**
	*	@brief Reruns the solver in case it did not reach the target cell yet due to given depth limitation.
	*/
	inline void advance_max_depth(std::function<void(const std::string&)> status_callback = nullptr, const std::size_t MAX_DEPTH = distance_exploration_type::SIZE_TYPE_MAX) {
		if (_status_code == 0) {
			return;
		}
		run_solver_toolchain(status_callback, MAX_DEPTH, 0);
	}

	/**
	*	@brief Returns the number of solutions found by solving.
	*/
	[[nodiscard]] std::size_t solutions_size() const {
		return _optimal_solutions.size();
	}

	/**
	*	@brief Returns an error code telling the reason why no path to target cell was found by solver.
	*	@details 0 -> No Error, 1 -> Reached MAX_DEPTH, 2 -> Entirely explored all reachable states
	*/
	[[nodiscard]] uint8_t status_code() const {
		return _status_code;
	}

	/**
	*	@brief Returns an optimal solution state path.
	*	@param index has to be less than \p solution_size()
	*/
	[[nodiscard]] state_path_type_interactive get_solution_state_path(std::size_t index) const {
		return _optimal_solutions[index].first;
	}

	/**
	*	@brief Returns a vector with a representant for each equivalence class of all optimal solutions. The representant is a pair of augmented state path and move path.
	*	@details Note, unless https://github.com/Necktschnagge/tobor-games/issues/167 has been fixed,equivalence classes arising from crossing two other equivalence classes may be omitted as long as we cover all segments of optimal solutions.
	*/
	[[nodiscard]] optimal_solutions_vector optimal_solutions() const {
		return _optimal_solutions;
	}
};

