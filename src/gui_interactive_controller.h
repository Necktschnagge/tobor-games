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

	using cell_id_type = tobor::v1_0::default_cell_id;

	using positions_of_pieces_type = tobor::v1_0::positions_of_pieces<tobor::v1_0::default_pieces_quantity, cell_id_type, false, false>;

	using world_type = tobor::v1_0::default_world;

	using move_one_piece_calculator_type = tobor::v1_0::move_one_piece_calculator<positions_of_pieces_type>;


	/* data */

	world_type tobor_world;

	move_one_piece_calculator_type move_one_piece_calculator;

	std::vector<positions_of_pieces_type> path;

	cell_id_type target_cell;


	GameController(
		const world_type& tobor_world,
		const positions_of_pieces_type& initial_state,
		const cell_id_type& target_cell
	) :
		tobor_world(tobor_world),
		move_one_piece_calculator(this->tobor_world),
		path{ initial_state },
		target_cell(target_cell)
	{

	}

	inline bool isFinal() const {
		return path.back().is_final(target_cell);
	}

	inline void movePiece(const tobor::v1_0::default_piece_id& piece_id, const tobor::v1_0::direction& direction) {
		if (isFinal()) {
			return;
		}

		positions_of_pieces_type current_state = path.back();

		auto [next_state, valid] = move_one_piece_calculator.successor_state(current_state, piece_id, direction);
		if (valid) {
			path.push_back(next_state);
		}

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

	void refreshSVG();

	void setPieceId(const tobor::v1_0::default_piece_id& piece_id) {
		this->selected_piece_id = piece_id;
	}

	void viewNumberOfSteps();

	void movePiece(const tobor::v1_0::direction& direction);

	void undo();

};


