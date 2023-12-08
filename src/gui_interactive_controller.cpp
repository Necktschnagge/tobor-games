#include "predefined.h"
#include "gui_interactive_controller.h"

#include "mainwindow.h"
#include "./ui_mainwindow.h"


#include"tobor_svg.h"



void GuiInteractiveController::startGame() {
	if (interactive_mode == InteractiveMode::NO_GAME) {

		mainWindow->ui->actionNewGame->setEnabled(false);
		mainWindow->ui->actionStopGame->setEnabled(true);

		interactive_mode = InteractiveMode::GAME_INTERACTIVE;

		// create a board

		auto tup = originalGenerator.split_element();
		(void)tup;
		auto x = std::get<0>(tup);
		auto a = x % 4; x /= 4;
		auto b = x % 4; x /= 4;
		auto c = x % 4; x /= 4;
		auto d = x % 4; x /= 4;
		auto e = x;
		qDebug() << a << "   :   " << b << "   :   " << c << "   :   " << d << "   :   " << e << "   :   " << std::get<1>(tup) << "   :   " << std::get<2>(tup) << "\n";
		auto world = originalGenerator.get_tobor_world();
		auto target = originalGenerator.get_target_cell();

		gameHistory.emplace_back(
			world,
			GameController::positions_of_pieces_type(
				{
					tobor::v1_0::default_cell_id::create_by_coordinates(0, 0, world)
				},
				{
					tobor::v1_0::default_cell_id::create_by_coordinates(0,15, world),
					tobor::v1_0::default_cell_id::create_by_coordinates(15,15, world),
					tobor::v1_0::default_cell_id::create_by_coordinates(15,0, world)
				}
			),
			target
		);

		++originalGenerator;

		refreshSVG();
	}
	else {

		QMessageBox msgBox(QMessageBox::Icon::Critical, QString("GUI ERROR"), "This action should not be available.");
		msgBox.exec();

	}
}

void GuiInteractiveController::stopGame() {

	if (interactive_mode == InteractiveMode::GAME_INTERACTIVE) {

		mainWindow->ui->actionNewGame->setEnabled(true);
		mainWindow->ui->actionStopGame->setEnabled(false);

		interactive_mode = InteractiveMode::NO_GAME;

		refreshSVG();

	}
	else {

		QMessageBox msgBox(QMessageBox::Icon::Critical, QString("GUI ERROR"), "This action should not be available.");
		msgBox.exec();

	}
}

void GuiInteractiveController::refreshSVG()
{
	if (interactive_mode == InteractiveMode::GAME_INTERACTIVE) {

		std::string example_svg_string = tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::draw_tobor_world(
			gameHistory.back().tobor_world,
			gameHistory.back().path.back(),
			gameHistory.back().target_cell,
			tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::coloring("red", "yellow", "green", "blue")
			// use the color permutation given by generator
		);

		mainWindow->viewSvgInMainView(example_svg_string);
	}

	if (interactive_mode == InteractiveMode::NO_GAME) {
		QGraphicsScene* scene = new QGraphicsScene();
		mainWindow->ui->graphicsView->setScene(scene);
	}
}


