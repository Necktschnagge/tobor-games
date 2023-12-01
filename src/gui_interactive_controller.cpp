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
			GameController::positions_of_pieces_type(
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
			gameHistory.back().path.back(),
			GameController::positions_of_pieces_type::cell_id_type::create_by_coordinates(13, 14, gameHistory.back().tobor_world),
			tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::coloring("red", "yellow", "green", "blue")
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
    const std::vector<tobor::v1_0::default_cell_id>& W_wall,
    const std::vector<tobor::v1_0::default_cell_id>& S_wall,
    const std::vector<tobor::v1_0::default_cell_id>& NW_corners,
    const std::vector<tobor::v1_0::default_cell_id>& NE_corners,
    const std::vector<tobor::v1_0::default_cell_id>& SW_corners,
    const std::vector<tobor::v1_0::default_cell_id>& SE_corners)
{
    for (const auto& cell_id : W_wall) {
        world.west_wall_by_id(cell_id.get_id()) = true;
    }
    for (const auto& cell_id : S_wall) {
        world.south_wall_by_transposed_id(cell_id.get_transposed_id()) = true;
    }
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
	std::array<std::vector<tobor::v1_0::default_world>, 4> all_quadrants;
	constexpr static std::size_t RED_PLANET{ 0 };
	constexpr static std::size_t GREEN_PLANET{ 1 };
	constexpr static std::size_t BLUE_PLANET{ 2 };
	constexpr static std::size_t YELLOW_PLANET{ 3 };

    {
        all_quadrants[RED_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[RED_PLANET].back();
        world.block_center_cells(2, 2);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(4, 0, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 6, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(1, 5, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(6, 2, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(2, 1, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { gear },       // NW
            { cross },      // NE
            { moon },       // SW
            { planet }      // SE
        );
    }
    {
        all_quadrants[RED_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[RED_PLANET].back();
        world.block_center_cells(2, 2);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(5, 0, world);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 2, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(6, 1, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 6, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 4, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { planet },     // NW
            { cross },      // NE
            { moon },       // SW
            { gear }        // SE
        );
    }
    {
        all_quadrants[RED_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[RED_PLANET].back();
        world.block_center_cells(2, 2);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 6, world);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(4, 0, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(2, 3, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(5, 4, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 5, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { planet },     // NW
            { cross },      // NE
            { moon },       // SW
            { gear }        // SE
        );
    }
    {
        all_quadrants[RED_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[RED_PLANET].back();
        world.block_center_cells(2, 2);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 3, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(2, 5, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(7, 4, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 1, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { gear },       // NW
            { cross },      // NE
            { moon },       // SW
            { planet }      // SE
        );
    }
    {
        all_quadrants[GREEN_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[GREEN_PLANET].back();
        world.block_center_cells(2, 2);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(5, 0, world);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(6, 5, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(1, 2, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(4, 6, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
        auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(7, 3, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { moon },               // NW
            { cross, swirl },       // NE
            { gear },               // SW
            { planet }              // SE
        );
    }
    {
        all_quadrants[GREEN_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[GREEN_PLANET].back();
        world.block_center_cells(2, 2);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 6, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(5, 2, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(4, 5, world);
        auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(5, 7, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { gear },               // NW
            { cross, swirl },       // NE
            { planet },             // SW
            { moon }                // SE
        );
    }
    {
        all_quadrants[GREEN_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[GREEN_PLANET].back();
        world.block_center_cells(2, 2);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(3, 2, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(5, 1, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(3, 3, world);
        auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(7, 5, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { moon },               // NW
            { cross, swirl },       // NE
            { gear },               // SW
            { planet }              // SE
        );
    }
    {
        all_quadrants[GREEN_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[GREEN_PLANET].back();
        world.block_center_cells(2, 2);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(4, 0, world);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 3, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 5, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(4, 3, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(6, 1, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 6, world);
        auto swirl = tobor::v1_0::default_cell_id::create_by_coordinates(2, 7, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { gear },               // NW
            { cross, swirl },       // NE
            { planet },             // SW
            { moon }                // SE
        );
    }
    {
        all_quadrants[BLUE_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[BLUE_PLANET].back();
        world.block_center_cells(2, 2);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(1, 6, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(2, 1, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 6, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { planet },     // NW
            { cross },      // NE
            { gear },       // SW
            { moon }        // SE
        );
    }
    {
        all_quadrants[BLUE_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[BLUE_PLANET].back();
        world.block_center_cells(2, 2);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 2, world);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 4, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(3, 6, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(2, 1, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { gear },       // NW
            { cross },      // NE
            { moon },       // SW
            { planet }      // SE
        );
    }
    {
        all_quadrants[BLUE_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[BLUE_PLANET].back();
        world.block_center_cells(2, 2);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 6, world);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(4, 6, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(6, 2, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { gear },       // NW
            { cross },      // NE
            { moon },       // SW
            { planet }      // SE
        );
    }
    {
        all_quadrants[BLUE_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[BLUE_PLANET].back();
        world.block_center_cells(2, 2);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 2, world);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(7, 0, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 1, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(3, 6, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(1, 3, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(6, 4, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { planet },     // NW
            { cross },      // NE
            { gear },       // SW
            { moon }        // SE
        );
    }
    {
        all_quadrants[YELLOW_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[YELLOW_PLANET].back();
        world.block_center_cells(2, 2);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 5, world);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(3, 0, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(4, 3, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(6, 5, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { moon },       // NW
            { cross },      // NE
            { planet },     // SW
            { gear }        // SE
        );
    }
    {
        all_quadrants[YELLOW_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[YELLOW_PLANET].back();
        world.block_center_cells(2, 2);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 2, world);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(1, 4, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(6, 3, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(3, 1, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 5, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { moon },       // NW
            { cross },      // NE
            { planet },     // SW
            { gear }        // SE
        );
    }
    {
        all_quadrants[YELLOW_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[YELLOW_PLANET].back();
        world.block_center_cells(2, 2);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(5, 0, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(2, 5, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(5, 7, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(6, 1, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { moon },       // NW
            { cross },      // NE
            { planet },     // SW
            { gear }        // SE
        );
    }
    {
        all_quadrants[YELLOW_PLANET].emplace_back(16, 16);
        auto& world = all_quadrants[YELLOW_PLANET].back();
        world.block_center_cells(2, 2);
        auto bottom = tobor::v1_0::default_cell_id::create_by_coordinates(0, 4, world);
        auto left = tobor::v1_0::default_cell_id::create_by_coordinates(6, 0, world);
        auto moon = tobor::v1_0::default_cell_id::create_by_coordinates(2, 6, world);
        auto cross = tobor::v1_0::default_cell_id::create_by_coordinates(4, 2, world);
        auto gear = tobor::v1_0::default_cell_id::create_by_coordinates(1, 1, world);
        auto planet = tobor::v1_0::default_cell_id::create_by_coordinates(5, 7, world);
        init_quadrant(
            world,
            { left },
            { bottom },
            { moon },       // NW
            { cross },      // NE
            { planet },     // SW
            { gear }        // SE
        );
    }
    return all_quadrants[0][0];
}

