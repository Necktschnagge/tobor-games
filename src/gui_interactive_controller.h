#pragma once

class GameController; // to be removed! ps_map is private, this is needed for friend class to work. Find another solution.

#include "all_solver.h" // ..., tobor::v1_0::tobor_world

#include "world_generator.h"
#include "color_generator.h"
#include "svg_1_1.h"

// #include "tobor_svg.h" produces error


#include <QMainWindow>
#include <QXmlStreamReader>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>
#include <QStyle>


#include <memory>
#include <random>
#include <thread>


class MainWindow;

/**
* @brief Keeps track of the path of states visited so far
*
*/
class GameController {
public:

	/* Types */

	//using world_type = tobor::v1_0::tobor_world<uint16_t>;
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

	struct solver_environment {

		using distance_exploration_type = tobor::v1_1::distance_exploration<move_one_piece_calculator_type, positions_of_pieces_type_solver>;

		using bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_solver, std::vector<bool>>;

		using naked_bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_solver, void>;

		using path_classificator_type = tobor::v1_1::path_classificator<positions_of_pieces_type_solver>;


		/*
		std::optional<
			std::map<
			positions_of_pieces_type_solver,
			std::vector<
			std::vector<move_path_type>
			>
			>
		> optional_classified_move_paths;
		*/

		std::size_t selected_solution_index;

		distance_exploration_type distance_explorer;

		bigraph_type bigraph;

		std::vector<naked_bigraph_type> partition_bigraphs;

		std::vector<std::vector<state_path_type_solver>> partitioned_state_paths;

	public:
		solver_environment(const positions_of_pieces_type_solver& initial_state) :
			selected_solution_index(0),
			distance_explorer(initial_state),
			bigraph(),
			partition_bigraphs(),
			partitioned_state_paths()
		{}

		inline void run(const GameController& controller, std::function<void(const std::string&)> status_callback = nullptr) {

			// explore...
			if (status_callback) status_callback("Exploring state space until target...");
			distance_explorer.explore_until_target(controller._move_engine, controller._target_cell);
			// ### inside this call, log every distance level as a progress bar


			if (status_callback) status_callback("Extracting solution state graph...");
			distance_explorer.get_simple_bigraph(controller._move_engine, controller._target_cell, bigraph);


			if (status_callback) status_callback("Partition state graph...");
			std::size_t count_partitions = path_classificator_type::make_state_graph_path_partitioning(bigraph);


			if (status_callback) status_callback("Extracting subgraph for each partition...");
			for (std::size_t i{ 0 }; i < count_partitions; ++i) {
				partition_bigraphs.emplace_back();
				path_classificator_type::extract_subgraph_by_label(bigraph, i, partition_bigraphs.back());
			}


			if (status_callback) status_callback("Generating state_paths ...");
			for (std::size_t i{ 0 }; i < partition_bigraphs.size(); ++i) {
				partitioned_state_paths.emplace_back(path_classificator_type::extract_all_state_paths(partition_bigraphs[i]));
			}


			if (status_callback) status_callback("Generating move_paths ...");
			for (std::size_t i{ 0 }; i < partitioned_state_paths.size(); ++i) {
				for (const auto& state_path : partitioned_state_paths[i]) {
					auto mp = tobor::v1_1::move_path<piece_move_type>(state_path, controller._move_engine);

					using aug_state = tobor::v1_1::augmented_positions_of_pieces<piece_quantity_type, cell_id_type, true, true>;

					auto initial_state_aug = aug_state(); // need a constructor copying from another state?

					auto s_path = mp.apply(initial_state_aug, controller._move_engine);
				}
			}

			// classify optimal paths...
			//optional_classified_move_paths.reset();
			//optional_classified_move_paths.emplace();
			//auto& classified_move_paths{ optional_classified_move_paths.value() };
			//
			//for (const auto& pair : optimal_paths_map) {
			//	classified_move_paths[pair.first] = move_path_type::interleaving_partitioning(pair.second);
			//	for (auto& equivalence_class : classified_move_paths[pair.first]) {
			//		std::sort(equivalence_class.begin(), equivalence_class.end(), move_path_type::antiprettiness_relation);
			//	}
			//}
			(void)optimal_depth;
		}
	};
private:

	/* data */

	world_type _world;

	move_one_piece_calculator_type _move_engine;

	state_path_type_interactive _path;

	std::vector<uint8_t> _color_permutation;

	cell_id_type _target_cell;

	std::optional<solver_environment> _solver;

	std::size_t _solver_begin_index;

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
		_solver_begin_index(0)
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
		if (is_final()) return 1;

		auto next_state = _move_engine.successor_state(current_state(), piece_id, direction);

		if (next_state == current_state()) return 2;

		_path += next_state;
	}

	inline void undo() {
		if (_path.vector().size() > 1) {
			_path.vector().pop_back();
		}
	}

	move_path_type& get_selected_solution_representant(std::size_t index);

	void start_solver(std::function<void(const std::string&)> status_callback = nullptr);

	void stopSolver() {

		_solver_begin_index = 0;

		optional_classified_move_paths.reset();
		selected_solution_index = 0;
	}

	void moveBySolver(bool forward);

	inline void selectSolution(const std::size_t& index) {
		selected_solution_index = index;
		// selects a solution from the list of solutions
	}

	inline void resetSolverSteps() {
		// go back to solver_begin_index
		_path.erase(_path.begin() + _solver_begin_index, _path.end());
	}

	~GameController() {}

	// must implement copy / move ctor and operator=
};

class GuiInteractiveController final {

	template<class X>
	friend void startReferenceGame22Helper(X& guiInteractiveController);

public:
	enum class InteractiveMode {
		NO_GAME,
		GAME_INTERACTIVE,
		SOLVER_INTERACTIVE_STEPS
	};

	using board_generator_type = tobor::v1_1::world_generator::original_4_of_16;

	using state_generator_type = tobor::v1_1::world_generator::initial_state_generator<
		GameController::positions_of_pieces_type_solver,
		256,
		GameController::piece_quantity_type::COUNT_TARGET_PIECES,
		GameController::piece_quantity_type::COUNT_NON_TARGET_PIECES,
		4>;

	using product_generator_type = tobor::v1_1::world_generator::product_group_generator<board_generator_type, state_generator_type>;

	using graphics_type = tobor::v1_1::tobor_graphics<GameController::world_type, GameController::positions_of_pieces_type_solver>;

	using graphics_coloring_type = typename graphics_type::coloring;

private:
	MainWindow* mainWindow;

	InteractiveMode interactive_mode;

	std::list<GameController> gameHistory;

	GameController::piece_id_type selected_piece_id{ 0 };

	product_generator_type productWorldGenerator;

	std::mt19937 generator;


	//tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::coloring coloring = tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::coloring("red", "yellow", "green", "blue");
	// needs tobor svg include which brings errors...

	template<class T, GameController::piece_quantity_type::int_type ... Index_Sequence>
	graphics_coloring_type make_coloring(
		T& permutated_color_vector,
		std::integer_sequence<GameController::piece_quantity_type::int_type, Index_Sequence...>
	) {
		auto coloring = graphics_coloring_type{
			(permutated_color_vector.colors[Index_Sequence].getSVGColorString()) ...
		};
		return coloring;
	}

public:

	tobor::v1_0::color_vector current_color_vector;

	void startReferenceGame22();

	GuiInteractiveController(MainWindow* mainWindow) :
		mainWindow(mainWindow),
		interactive_mode(InteractiveMode::NO_GAME),
		productWorldGenerator()
	{
		std::random_device rd;

		generator.seed(rd());

		std::uniform_int_distribution<uint64_t> distribution_on_uint64_board(0, board_generator_type::CYCLIC_GROUP_SIZE);
		std::uniform_int_distribution<uint64_t> distribution_on_uint64_pieces(0, product_generator_type::side_group_generator_type::CYCLIC_GROUP_SIZE);

		productWorldGenerator.main().set_counter(distribution_on_uint64_board(generator));
		productWorldGenerator.side().set_counter(distribution_on_uint64_pieces(generator));

		//originalGenerator.set_counter(distribution_on_uint64(generator));

		//originalGenerator.set_counter(73021); // 72972 73021

		//originalGenerator.set_generator(1);
		//originalGenerator.set_counter(3223);

		//productWorldGenerator.main().set_counter(73021);
	}

	inline InteractiveMode interactiveMode() const {
		return interactive_mode;
	}

	void startGame();

	void createColorActions();

	void stopGame();

	void moveBySolver(bool forward);

	void setPieceId(const GameController::piece_id_type& piece_id);

	void selectPieceByColorId(const std::size_t& color_id);

	void refreshNumberOfSteps();

	void refreshSVG();

	void refreshMenuButtonEnable();

	void refreshStatusbar();

	inline void refreshAll() {
		refreshSVG();
		refreshStatusbar();
		refreshMenuButtonEnable();
		viewSolutionPaths();
	}

	void movePiece(const tobor::v1_0::direction& direction);

	void undo();

	void startSolver();

	void stopSolver();

	void selectSolution(std::size_t index);

	void viewSolutionPaths();

	void highlightGeneratedTargetCells();

};


