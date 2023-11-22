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


void init_quadrant(
	tobor::v1_0::default_world& world,
	const std::vector<tobor::v1_0::default_cell_id>& SW_corners,
	const std::vector<tobor::v1_0::default_cell_id>& SE_corners,
	const std::vector<tobor::v1_0::default_cell_id>& NE_corners,
	const std::vector<tobor::v1_0::default_cell_id>& NW_corners
) {
	for (const auto& cell_id : SW_corners) {
		world.west_wall_by_id(cell_id.get_id()) = true;
		world.south_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
	}
	for (const auto& cell_id : NW_corners) {
		world.west_wall_by_id(cell_id.get_id()) = true;
		world.north_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
	}
	for (const auto& cell_id : NE_corners) {
		world.east_wall_by_id(cell_id.get_id()) = true;
		world.north_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
	}
	for (const auto& cell_id : SE_corners) {
		world.east_wall_by_id(cell_id.get_id()) = true;
		world.south_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
	}

}


tobor::v1_0::default_world GuiInteractiveController::generateBoard()
{
	auto tobor_world = tobor::v1_0::default_world(16, 16);

	tobor_world.block_center_cells(2, 2);

	auto sw_cell = tobor::v1_0::default_cell_id::create_by_coordinates(4, 6, tobor_world);

	auto sw_cell_bottom_side = tobor::v1_0::default_cell_id::create_by_coordinates(4, 0, tobor_world);

	auto sw_cell_left_side = tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, tobor_world);


	auto ne_cell = tobor::v1_0::default_cell_id::create_by_coordinates(1, 5, tobor_world);
	
	auto nw_cell = tobor::v1_0::default_cell_id::create_by_coordinates(6, 2, tobor_world);

	auto se_cell = tobor::v1_0::default_cell_id::create_by_coordinates(2, 1, tobor_world);

	init_quadrant(
		tobor_world,
		{ sw_cell, sw_cell_bottom_side, sw_cell_left_side },
		{ se_cell },
		{ ne_cell },
		{ nw_cell }
	);

	return tobor_world;
}


