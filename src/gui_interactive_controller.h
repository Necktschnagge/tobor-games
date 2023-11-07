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

class GuiInteractiveController final {

	MainWindow* mainWindow;

	enum class InteractiveMode {
		NO_GAME,
		GAME_INTERACTIVE
	};

	InteractiveMode interactive_mode;



public:

	GuiInteractiveController(MainWindow* mainWindow) : 
		mainWindow(mainWindow),
		interactive_mode(InteractiveMode::NO_GAME) {

	}

	void startGame();
	void stopGame();

	tobor::v1_0::tobor_world generateBoard();

};


