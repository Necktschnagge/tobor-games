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
		tobor::v1_0::tobor_world tobor_world = generateBoard();

		gameHistory.emplace_back(
			tobor_world,
			tobor::v1_0::default_positions_of_pieces(
				{
					tobor::v1_0::default_cell_id::create_by_coordinates(2, 3, tobor_world)
				},
				{
					tobor::v1_0::default_cell_id::create_by_coordinates(12,3, tobor_world),
					tobor::v1_0::default_cell_id::create_by_coordinates(11,12, tobor_world),
					tobor::v1_0::default_cell_id::create_by_coordinates(5,13, tobor_world)
				}
			)
		);

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
			gameHistory.back().current_state,
			GameController::positions_of_pieces_type::cell_id_type::create_by_coordinates(13, 14, gameHistory.back().tobor_world),
			tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::coloring("red", "green", "yellow", "blue")
		);


		QXmlStreamReader xml;
		xml.addData(QString::fromStdString(example_svg_string));

		QSvgRenderer* svgRenderer = new QSvgRenderer(&xml);
		QGraphicsSvgItem* item = new QGraphicsSvgItem();
		QGraphicsScene* scene = new QGraphicsScene();

		item->setSharedRenderer(svgRenderer);
		scene->addItem(item);
		mainWindow->ui->graphicsView->setScene(scene);
		mainWindow->ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
		mainWindow->ui->graphicsView->show();

	}

	if (interactive_mode == InteractiveMode::NO_GAME) {
		QGraphicsScene* scene = new QGraphicsScene();
		mainWindow->ui->graphicsView->setScene(scene);
	}
}

tobor::v1_0::default_world GuiInteractiveController::generateBoard()
{
	auto tobor_world = tobor::v1_0::default_world(16, 16);

	tobor_world.block_center_cells(2, 2);

	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(6, 0)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(12, 0)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(2, 1)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(10, 1)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(14, 3)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(1, 4)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(11, 4)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(13, 5)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(4, 6)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(12, 9)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(7, 10)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(14, 10)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(3, 11)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(7, 13)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(10, 13)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(1, 14)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(13, 14)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(4, 15)) = true;
	tobor_world.west_wall_by_id(tobor_world.coordinates_to_cell_id(12, 15)) = true;

	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(0, 2)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(0, 10)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(1, 4)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(1, 14)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(2, 2)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(2, 11)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(3, 7)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(6, 3)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(6, 14)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(7, 11)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(9, 2)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(9, 13)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(10, 4)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(11, 10)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(13, 6)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(13, 15)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(14, 3)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(14, 10)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(15, 7)) = true;
	tobor_world.south_wall_by_transposed_id(tobor_world.coordinates_to_transposed_cell_id(15, 12)) = true;

	return tobor_world;
}

