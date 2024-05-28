#pragma once

class GameController; // to be removed! ps_map is private, this is needed for friend class to work. Find another solution.

#include "game_controller.h"

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


