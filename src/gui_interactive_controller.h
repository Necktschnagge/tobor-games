#pragma once

#include "solver.h" // ..., tobor::v1_0::tobor_world

#include "world_generator.h"



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

class MainWindow;

/**
* @brief Keeps track of the path of states visited so far
*
*/
struct GameController {
public:

	/*
	class ExplorationTree {

		std::vector<std::shared_ptr<ExplorationTree>> children;

	public:
		ExplorationTree() {} // create root

	};
	*/


	/* Types */

	using world_type = tobor::v1_0::default_world;

	using cell_id_type = tobor::v1_0::default_cell_id;

	using positions_of_pieces_type = tobor::v1_0::positions_of_pieces<tobor::v1_0::default_pieces_quantity, cell_id_type, false, false>;

	using piece_move_type = tobor::v1_0::default_piece_move;

	using move_one_piece_calculator_type = tobor::v1_0::move_one_piece_calculator<positions_of_pieces_type, tobor::v1_0::default_quick_move_cache, piece_move_type>;

	using state_graph_node_type = tobor::v1_0::state_graph_node<positions_of_pieces_type, piece_move_type>;

	using partial_state_graph_type = tobor::v1_0::partial_state_graph<move_one_piece_calculator_type, state_graph_node_type>;


	/* data */

	world_type tobor_world;

	move_one_piece_calculator_type move_one_piece_calculator;

	std::vector<positions_of_pieces_type> path;

	cell_id_type target_cell;

	std::vector<positions_of_pieces_type>::iterator solver_begin;

	std::optional<partial_state_graph_type> optional_solver_state_graph;


	GameController(
		const world_type& tobor_world,
		const positions_of_pieces_type& initial_state,
		const cell_id_type& target_cell
	) :
		tobor_world(tobor_world),
		move_one_piece_calculator(this->tobor_world),
		path{ initial_state },
		target_cell(target_cell),
		solver_begin(path.begin())
	{

	}

	inline bool isFinal() const {
		return currentState().is_final(target_cell);
	}

	inline bool isEmptyPath() const {
		return path.size() == 1;
	}

	const positions_of_pieces_type& currentState() const {
		return path.back();
	}

	inline void movePiece(const tobor::v1_0::default_piece_id& piece_id, const tobor::v1_0::direction& direction) {
		if (isFinal()) {
			return;
		}

		auto [next_state, valid] = move_one_piece_calculator.successor_state(currentState(), piece_id, direction);

		if (valid) {
			path.push_back(next_state);
		}

	}

	inline void undo() {
		if (path.size() > 1) {
			path.pop_back();
		}
	}

	inline void startSolver() {

		optional_solver_state_graph.emplace(currentState());

		partial_state_graph_type& graph{ optional_solver_state_graph.value() };

		graph.build_state_graph_for_all_optimal_solutions(move_one_piece_calculator, target_cell);

		// now extract all paths...




		// turn into solver mode,
		// run solver from current state
		// display all optimal solutions in ListView
	}

	inline void selectSolution(const std::size_t& index) {
		(void)index;
		// selects a solution from the list of solutions
	}

	inline void solverStep() {
		// move one step forward according to the current selected solver solution.
		// disable the ListView... make it gray, cannot click there anymore...
	}

	inline void resetSolverSteps() {
		// go back to solver_init
	}

};

class GuiInteractiveController final {

	MainWindow* mainWindow;

	enum class InteractiveMode {
		NO_GAME,
		GAME_INTERACTIVE,
		SOLVER_INTERACTIVE_STEPS
	};

	using board_generator_type = tobor::v1_0::world_generator::original_4_of_16;

	InteractiveMode interactive_mode;

	std::vector<GameController> gameHistory;

	tobor::v1_0::default_piece_id selected_piece_id{ 0 };

	board_generator_type originalGenerator;

	std::mt19937 generator;



public:

	GuiInteractiveController(MainWindow* mainWindow) :
		mainWindow(mainWindow),
		interactive_mode(InteractiveMode::NO_GAME),
		originalGenerator()
	{
		std::random_device rd;

		generator.seed(rd());

		std::uniform_int_distribution<uint64_t> distribution_on_uint64(0, board_generator_type::CYCLIC_GROUP_SIZE);

		originalGenerator.set_counter(distribution_on_uint64(generator));
	}

	void startGame();

	void stopGame();

	void setPieceId(const tobor::v1_0::default_piece_id& piece_id) {
		this->selected_piece_id = piece_id;
	}

	void refreshNumberOfSteps();

	void refreshSVG();

	void refreshMenuButtonEnable();

	inline void refreshAll() {
		refreshSVG();
		refreshNumberOfSteps();
		refreshMenuButtonEnable();
	}

	void movePiece(const tobor::v1_0::direction& direction);

	void undo();

};


