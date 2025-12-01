#pragma once


#include "engine/path_classificator.h"
#include "engine/byte_tree_distance_exploration.h"
#include "engine/prettiness_evaluator.h"


#include "engine_typeset.h"

#include <concepts>
#include <functional>
#include <optional>

#if false
namespace tobor {
	namespace identifier {

		static const char* naked_bigraph_type{ "naked_bigraph_type" };

	}

	namespace traits {

		template<class T, const char* U>
		class has_type;

		template<class T>
		class has_type<T, ::tobor::identifier::naked_bigraph_type> {
		public:
			static constexpr bool value = std::is_class_v<typename T::naked_bigraph_type>;
		};
	}

	namespace concepts {

		//template<class T, class U>
		//concept has_type = true;

			//<naked_bigraph_type> && std::is_class_v<typename EngineTypeSet::naked_bigraph_type>

	}
}


template<class T>
concept has_member_type__naked_bigraph_type = requires(T t) {
	true;
};

template<class T>
concept WrapperType = std::integral<T> && std::signed_integral<T>;

template<WrapperType T>
class MyWrapper {
public:
	T t;
};
#endif


template<class EngineTypeSet>
concept PrettinessEvaluationStrategyTypeSet =
(
	true
	&& std::is_class_v<typename EngineTypeSet::naked_bigraph_type>
	//&& std::is_class_v<typename EngineTypeSet::pretty_evaluation_bigraph_type>
	&& requires (EngineTypeSet::callback_type t) { t("text"); t(std::string("text")); }
	);

template<PrettinessEvaluationStrategyTypeSet EngineTypeSet>
class PrettinessEvaluationStrategy {

public:
	using naked_bigraph_type = typename EngineTypeSet::naked_bigraph_type;
	using callback_type = typename EngineTypeSet::callback_type;
	//using pretty_evaluation_bigraph_type = typename EngineTypeSet::pretty_evaluation_bigraph_type;
	using positions_of_pieces_type_interactive = typename EngineTypeSet::positions_of_pieces_type_interactive;
	using move_engine_type = typename EngineTypeSet::move_engine_type;
	using state_path_type_interactive = typename EngineTypeSet::state_path_type_interactive;
	using move_path_type = typename EngineTypeSet::move_path_type;
	using optimal_solutions_vector = std::vector<std::pair<state_path_type_interactive, move_path_type>>;

	using status_code_type = uint8_t;



	static constexpr status_code_type STATUS_OK{ 0 };

	PrettinessEvaluationStrategy() {}

	virtual status_code_type operator()(
		const std::vector<naked_bigraph_type>& partition_bigraphs,
		const positions_of_pieces_type_interactive& initial_state,
		const move_engine_type& move_engine,
		optimal_solutions_vector& optimal_solutions,
		const callback_type& status_callback = nullptr
		)
		const = 0;

	virtual ~PrettinessEvaluationStrategy() {}
};

template<class EngineTypeSet>
class GraphAnnotationStrategy : public PrettinessEvaluationStrategy<EngineTypeSet> {
public:
	using parent = PrettinessEvaluationStrategy<EngineTypeSet>;

	using naked_bigraph_type = typename EngineTypeSet::naked_bigraph_type;
	using callback_type = typename EngineTypeSet::callback_type;
	using pretty_evaluation_bigraph_type = typename EngineTypeSet::pretty_evaluation_bigraph_type;
	using positions_of_pieces_type_solver = typename EngineTypeSet::positions_of_pieces_type_solver;
	using positions_of_pieces_type_interactive = typename EngineTypeSet::positions_of_pieces_type_interactive;
	using piece_change_decoration_vector = typename EngineTypeSet::piece_change_decoration_vector;
	using cell_id_type = typename EngineTypeSet::cell_id_type;
	using quick_move_cache_type = typename EngineTypeSet::quick_move_cache_type;
	using piece_move_type = typename EngineTypeSet::piece_move_type;
	using move_engine_type = typename EngineTypeSet::move_engine_type;
	using pieces_quantity_type = typename EngineTypeSet::pieces_quantity_type;
	using state_path_type_interactive = typename EngineTypeSet::state_path_type_interactive;
	using move_path_type = typename EngineTypeSet::move_path_type;

	using prettiness_evaluator_type = tobor::v1_1::prettiness_evaluator<pieces_quantity_type>;
	using status_code_type = typename parent::status_code_type;
	using optimal_solutions_vector = std::vector<std::pair<state_path_type_interactive, move_path_type>>;


	/**
	*	@brief Fills _optimal_solutions by applying a dynamic programming approach using labeled bigraph
	*/
	virtual status_code_type operator()(
		const std::vector<naked_bigraph_type>& partition_bigraphs,
		const positions_of_pieces_type_interactive& initial_state,
		const move_engine_type& move_engine,
		optimal_solutions_vector& optimal_solutions,
		const callback_type& status_callback = nullptr
		)
		const override
	{
		/// code structure: never move this function into engine. This is to much related to a specific use case.
		std::vector<pretty_evaluation_bigraph_type> partition_bigraphs_decorated;

		for (std::size_t i{ 0 }; i < partition_bigraphs.size(); ++i) {

			if (status_callback) status_callback("Simulation-copying from solver state type to interactive state type of partition graphs...");
			// simulation copy here
			partition_bigraphs_decorated.emplace_back();
			auto iter_to_single_initial_state = tobor::v1_1::bigraph_operations::
				bigraph_simulation_copy<positions_of_pieces_type_solver, void, positions_of_pieces_type_interactive, piece_change_decoration_vector, cell_id_type, quick_move_cache_type, piece_move_type>::
				copy(partition_bigraphs[i], partition_bigraphs_decorated[i], initial_state, move_engine);

			if (status_callback) status_callback("Decorating partition subgraph for prettiness evaluation...");
			prettiness_evaluator_type::build_prettiness_decoration(partition_bigraphs_decorated[i], iter_to_single_initial_state, move_engine);

			if (status_callback) status_callback("Selecting partition representant according to prettiness ranking...");
			const state_path_type_interactive representant{ prettiness_evaluator_type::get_representant(partition_bigraphs_decorated[i], iter_to_single_initial_state) };

			const move_path_type color_aware_move_path{ move_path_type::extract_unsorted_move_path(representant, move_engine) };

			optimal_solutions.emplace_back(representant, color_aware_move_path);
		}
		return parent::STATUS_OK;
	}

	virtual ~GraphAnnotationStrategy() override {}
};
#if false


template<class EngineTypeSet>
class ExplicitBruteForceStrategy : public PrettinessEvaluationStrategy<EngineTypeSet> {
public:
	using parent = PrettinessEvaluationStrategy<EngineTypeSet>;

	using naked_bigraph_type = typename EngineTypeSet::naked_bigraph_type;
	using callback_type = typename EngineTypeSet::callback_type;
	using pretty_evaluation_bigraph_type = typename EngineTypeSet::pretty_evaluation_bigraph_type;
	using positions_of_pieces_type_solver = typename EngineTypeSet::positions_of_pieces_type_solver;
	using positions_of_pieces_type_interactive = typename EngineTypeSet::positions_of_pieces_type_interactive;
	using piece_change_decoration_vector = typename EngineTypeSet::piece_change_decoration_vector;
	using cell_id_type = typename EngineTypeSet::cell_id_type;
	using quick_move_cache_type = typename EngineTypeSet::quick_move_cache_type;
	using piece_move_type = typename EngineTypeSet::piece_move_type;
	using move_engine_type = typename EngineTypeSet::move_engine_type;
	using pieces_quantity_type = typename EngineTypeSet::pieces_quantity_type;
	using state_path_type_interactive = typename EngineTypeSet::state_path_type_interactive;
	using state_path_type_solver = typename EngineTypeSet::state_path_type_solver;
	using move_path_type = typename EngineTypeSet::move_path_type;

	using prettiness_evaluator_type = tobor::v1_1::prettiness_evaluator<pieces_quantity_type>;
	using status_code_type = typename parent::status_code_type;
	using optimal_solutions_vector = std::vector<std::pair<state_path_type_interactive, move_path_type>>;

	/**
	*	@brief Fills _optimal_solutions by applying an explicit approach for prettiness evaluation
	*	@details Creates all optimal solutions, sorts them by prettiness relation, Brute force
	*/
	virtual status_code_type operator()(
		const std::vector<naked_bigraph_type>& partition_bigraphs,
		const positions_of_pieces_type_interactive& initial_state,
		const move_engine_type& move_engine,
		optimal_solutions_vector& optimal_solutions,
		const callback_type& status_callback = nullptr
		) const override {

		// code structure: never move this function into engine. This is to much related to a specific use case.

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
	}
};
#endif
#if false
#endif


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
	static_assert(std::is_same<Pieces_Quantity_T, pieces_quantity_type>::value, "SolverEnvironment: type error: pieces_quantity_type");

	using piece_move_type = typename engine_typeset::piece_move_type;

	using quick_move_cache_type = typename engine_typeset::quick_move_cache_type;



	using  piece_quantity_int_type = typename pieces_quantity_type::int_type;



	using optimal_solutions_vector = std::vector<std::pair<state_path_type_interactive, move_path_type>>;

	using prettiness_evaluator_type = tobor::v1_1::prettiness_evaluator<pieces_quantity_type>;

	using pretty_evaluation_bigraph_type = typename prettiness_evaluator_type::pretty_evaluation_bigraph_type;

	using piece_change_decoration_vector = typename prettiness_evaluator_type::piece_change_decoration_vector;

	using status_code_type = uint8_t;

	static constexpr status_code_type STATUS_OK{ 0 };
	//static constexpr status_code_type STATUS_OK{ 0 };

private:

	using distance_exploration_type = tobor::v1_1::byte_tree_distance_exploration<move_engine_type, positions_of_pieces_type_solver>;

	using bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_solver, std::vector<bool>>;

	using naked_bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_solver, void>;

	using path_classificator_type = tobor::v1_1::path_classificator<positions_of_pieces_type_solver>;


	/** data **/

	positions_of_pieces_type_interactive _initial_state;

	cell_id_type _target_cell;

	const move_engine_type& _move_engine;

	uint8_t _status_code;

	distance_exploration_type _distance_explorer;

	optimal_solutions_vector _optimal_solutions;


	/**
	*	@brief Fills _optimal_solutions by applying a dynamic programming approach using labeled bigraph
	*/
	inline uint8_t dynamic_programming_prettiness_evaluation(
		const std::vector<naked_bigraph_type>& partition_bigraphs,
		std::function<void(const std::string&)> status_callback = nullptr
	) {
		/// code structure: never move this function into engine. This is to much related to a specific use case.
		std::vector<pretty_evaluation_bigraph_type> partition_bigraphs_decorated;

		for (std::size_t i{ 0 }; i < partition_bigraphs.size(); ++i) {

			if (status_callback) status_callback("Simulation-copying from solver state type to interactive state type of partition graphs...");
			// simulation copy here
			partition_bigraphs_decorated.emplace_back();
			auto iter_to_single_initial_state = tobor::v1_1::bigraph_operations::bigraph_simulation_copy<positions_of_pieces_type_solver, void, positions_of_pieces_type_interactive, piece_change_decoration_vector, cell_id_type, quick_move_cache_type, piece_move_type>::
				copy(partition_bigraphs[i], partition_bigraphs_decorated[i], _initial_state, _move_engine);

			if (status_callback) status_callback("Decorating partition subgraph for prettiness evaluation...");
			prettiness_evaluator_type::build_prettiness_decoration(partition_bigraphs_decorated[i], iter_to_single_initial_state, _move_engine);

			if (status_callback) status_callback("Selecting partition representant according to prettiness ranking...");
			const state_path_type_interactive representant{ prettiness_evaluator_type::get_representant(partition_bigraphs_decorated[i], iter_to_single_initial_state) };

			const move_path_type color_aware_move_path{ move_path_type::extract_unsorted_move_path(representant, _move_engine) };

			_optimal_solutions.emplace_back(representant, color_aware_move_path);
		}
		return 0; // status code: OK
	}



	/**
	*	@brief Fills _optimal_solutions by applying an explicit approach for prettiness evaluation
	*	@details Creates all optimal solutions, sorts them by prettiness relation, Brute force
	*/
	inline uint8_t explicit_move_path_prettiness_evaluation(
		const std::vector<naked_bigraph_type>& partition_bigraphs,
		std::function<void(const std::string&)> status_callback = nullptr
	) {
		/// code structure: never move this function into engine. This is to much related to a specific use case.

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
	*	@
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
		_distance_explorer(initial_state.naked(), move_engine),
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
	*
	* ####### we need to add the out of range case where there are further explorable states but we cannot because of too small type for depth.
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
	*	@brief Returns an optimal solution move path.
	*	@param index has to be less than \p solution_size()
	*/
	[[nodiscard]] move_path_type get_solution_move_path(std::size_t index) const {
		return _optimal_solutions[index].second;
	}

	/**
	*	@brief Returns a vector with a representant for each equivalence class of all optimal solutions. The representant is a pair of augmented state path and move path.
	*	@details Note, unless https://github.com/Necktschnagge/tobor-games/issues/167 has been fixed,equivalence classes arising from crossing two other equivalence classes may be omitted as long as we cover all segments of optimal solutions.
	*/
	[[nodiscard]] optimal_solutions_vector optimal_solutions() const {
		return _optimal_solutions;
	}
};

