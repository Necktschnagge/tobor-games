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

void set_cell(
    tobor::v1_0::default_world& world,
    int x_coord,
    int y_coord,
    std::string direction){

    x_coord--;  // start counting with 1
    y_coord--;

    for(std::string::iterator it = direction.begin(); it != direction.end(); ++it) {
        switch (*it)
        {
            case 'n':
                world.north_wall_by_transposed_id(world.coordinates_to_transposed_cell_id(x_coord, y_coord)) = true;
                break;
            case 'o':
                world.east_wall_by_id(world.coordinates_to_cell_id(x_coord, y_coord)) = true;
                break;
            case 's':
                world.south_wall_by_transposed_id(world.coordinates_to_transposed_cell_id(x_coord, y_coord)) = true;
                break;
            case 'w':
                world.west_wall_by_id(world.coordinates_to_cell_id(x_coord, y_coord)) = true;
                break;
         }
    }
}

void init_quadrant(
    tobor::v1_0::default_world& world,
    int quad_no) {

    switch(quad_no)
    {
        case 1:
            set_cell(world, 5, 1, "w");     // |<-
            set_cell(world, 1, 5, "s");     //  _
            set_cell(world, 7, 3, "nw");    // ┌
            set_cell(world, 2, 6, "no");    //  ┐
            set_cell(world, 5, 7, "sw");    // └
            set_cell(world, 3, 2, "so");    //  ┘
            break;
        case 2:
            set_cell(world, 6, 1, "w");
            set_cell(world, 1, 6, "s");
            set_cell(world, 6, 5, "nw");
            set_cell(world, 7, 2, "no");
            set_cell(world, 2, 3, "sw");
            set_cell(world, 4, 7, "so");
            break;
        case 3:
            set_cell(world, 6, 1, "w");
            set_cell(world, 1, 6, "s");
            set_cell(world, 7, 6, "nw");
            set_cell(world, 2, 3, "no");
            set_cell(world, 8, 4, "no");
            set_cell(world, 5, 7, "sw");
            set_cell(world, 4, 2, "so");
            break;
        case 4:
            set_cell(world, 8, 1, "w");
            set_cell(world, 1, 5, "s");
            set_cell(world, 4, 2, "nw");
            set_cell(world, 6, 3, "no");
            set_cell(world, 6, 8, "no");
            set_cell(world, 5, 6, "sw");
            set_cell(world, 2, 7, "so");
            break;
    }
}

tobor::v1_0::default_world GuiInteractiveController::generateBoard()
{
	std::array<std::vector<tobor::v1_0::default_world>, 4> all_quadrants;
	constexpr static std::size_t RED_PLANET{ 0 };
	constexpr static std::size_t GREEN_PLANET{ 1 };
	constexpr static std::size_t BLUE_PLANET{ 2 };
	constexpr static std::size_t YELLOW_PLANET{ 3 };
    (void)GREEN_PLANET;
    (void)BLUE_PLANET;
    (void)YELLOW_PLANET;

    {
        all_quadrants[RED_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[RED_PLANET].back();
        world.block_center_cells(2, 2);
        init_quadrant(world, 1);
    }
    {
        all_quadrants[RED_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[RED_PLANET].back();
        world.block_center_cells(2, 2);
        set_cell(world, 6, 1, "w");
        set_cell(world, 1, 6, "s");
        set_cell(world, 6, 5, "nw");
        set_cell(world, 7, 2, "no");
        set_cell(world, 2, 3, "sw");
        set_cell(world, 4, 7, "so");
    }

    return all_quadrants[RED_PLANET][1];
}


