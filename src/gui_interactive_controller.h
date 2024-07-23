#pragma once

#include "game_factory.h"
#include "game_controller.h"

#include "world_generator_1_1.h" // remove here? only needed in factory!
#include "color_generator.h"
#include "svg_1_1.h"

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

	/*
	using board_generator_type = tobor::v1_1::world_generator::original_4_of_16;

	using state_generator_type = tobor::v1_1::world_generator::initial_state_generator<
		DRWGameController::positions_of_pieces_type_solver,
		256,
		DRWGameController::pieces_quantity_type::COUNT_TARGET_PIECES,
		DRWGameController::pieces_quantity_type::COUNT_NON_TARGET_PIECES,
		4>;

	using product_generator_type = tobor::v1_1::world_generator::product_group_generator<board_generator_type, state_generator_type>;
	*/

	
private:
	MainWindow* mainWindow;

	InteractiveMode interactive_mode;

	std::shared_ptr<AbstractGameController> current_game; /// check all positions where used!!!! #######



	std::vector<std::shared_ptr<AbstractGameFactory>> factory_history;
	std::vector<std::unique_ptr<AbstractGameFactory>> next_factory_1;


	std::size_t factory_select;

	//std::size_t selected_piece_id{ 0 };

	std::mt19937 generator;


	//tobor::v1_0::tobor_graphics<DRWGameController::positions_of_pieces_type>::coloring coloring = tobor::v1_0::tobor_graphics<DRWGameController::positions_of_pieces_type>::coloring("red", "yellow", "green", "blue");
	// needs tobor svg include which brings errors...


public:

	tobor::v1_0::color_vector current_color_vector;

	void startReferenceGame22();

	GuiInteractiveController(MainWindow* mainWindow) :
		mainWindow(mainWindow),
		interactive_mode(InteractiveMode::NO_GAME),
		//next_factory(),
		next_factory_1(),
		factory_select(2)
	{
		std::random_device rd;

		generator.seed(rd());

		next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 1>>());
		next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 2>>());
		next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 3>>());
		next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 4>>());
		next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 5>>());
		next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 6>>());
		next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 7>>());


		for (auto& factory : next_factory_1) {

			std::uniform_int_distribution<uint64_t> distribution_on_uint64_board(0, factory->world_generator_group_size());
			std::uniform_int_distribution<uint64_t> distribution_on_uint64_pieces(0, factory->state_generator_group_size());

			factory->set_world_generator_counter(distribution_on_uint64_board(generator));
			factory->set_state_generator_counter(distribution_on_uint64_pieces(generator));

		}

	}

	inline InteractiveMode interactiveMode() const {
		return interactive_mode;
	}

	void startGame();

	void createColorActions();

	void stopGame();

	void moveBySolver(bool forward);

	//[[deprecated]] void setPieceId(const std::size_t &piece_id);

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


