#pragma once

#include "models.h" //tobor::v1_0::tobor_world

#include <QMainWindow>
#include <QXmlStreamReader>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>
#include <QStyle>

class MainWindow;

struct GameController {
public:

	using positions_of_pieces_type = tobor::v1_0::default_positions_of_pieces;

	tobor::v1_0::default_world tobor_world;

	tobor::v1_0::default_positions_of_pieces initial_state;

	tobor::v1_0::default_positions_of_pieces current_state;

	GameController(const tobor::v1_0::tobor_world<>& tobor_world, const tobor::v1_0::default_positions_of_pieces& initial_state) :
		tobor_world(tobor_world),
		initial_state(initial_state),
		current_state(initial_state)
	{

	}

};

class GuiInteractiveController final {

	MainWindow* mainWindow;

	enum class InteractiveMode {
		NO_GAME,
		GAME_INTERACTIVE
	};

	InteractiveMode interactive_mode;

	std::vector<GameController> gameHistory;

public:

	GuiInteractiveController(MainWindow* mainWindow) :
		mainWindow(mainWindow),
		interactive_mode(InteractiveMode::NO_GAME) {

	}

	void startGame();

	void stopGame();

	void refreshSVG();

	tobor::v1_0::tobor_world<> generateBoard();

};


