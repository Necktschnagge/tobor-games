#include "gui_interactive_controller.h"
#include "predefined.h"

#include "./ui_mainwindow.h"
#include "mainwindow.h"

#include "tobor_svg.h"

void GuiInteractiveController::startGame() {

  if (interactive_mode == InteractiveMode::NO_GAME) {

    mainWindow->ui->actionNewGame->setEnabled(false);
    mainWindow->ui->actionStopGame->setEnabled(true);

    interactive_mode = InteractiveMode::GAME_INTERACTIVE;

    // create a board

    auto tup = originalGenerator.split_element();
    (void)tup;
    auto x = std::get<0>(tup);
    auto a = x % 4;
    x /= 4;
    auto b = x % 4;
    x /= 4;
    auto c = x % 4;
    x /= 4;
    auto d = x % 4;
    x /= 4;
    auto e = x;
    qDebug() << a << "   :   " << b << "   :   " << c << "   :   " << d
             << "   :   " << e << "   :   " << std::get<1>(tup) << "   :   "
             << std::get<2>(tup) << "\n";
    qDebug() << originalGenerator.get_counter() << "\n";
    auto world = originalGenerator.get_tobor_world();
    auto target = originalGenerator.get_target_cell();

    gameHistory.emplace_back(
        world,
        GameController::positions_of_pieces_type(
            {tobor::v1_0::default_cell_id::create_by_coordinates(0, 0, world)},
            {tobor::v1_0::default_cell_id::create_by_coordinates(0, 15, world),
             tobor::v1_0::default_cell_id::create_by_coordinates(15, 15, world),
             tobor::v1_0::default_cell_id::create_by_coordinates(15, 0,
                                                                 world)}),
        target);

    ++originalGenerator;

    refreshSVG();
  } else {

    QMessageBox msgBox(QMessageBox::Icon::Critical, QString("GUI ERROR"),
                       "This action should not be available.");
    msgBox.exec();
  }
}

void GuiInteractiveController::stopGame() {

  if (interactive_mode == InteractiveMode::GAME_INTERACTIVE) {

    mainWindow->ui->actionNewGame->setEnabled(true);
    mainWindow->ui->actionStopGame->setEnabled(false);

    interactive_mode = InteractiveMode::NO_GAME;

    refreshSVG();

  } else {

    QMessageBox msgBox(QMessageBox::Icon::Critical, QString("GUI ERROR"),
                       "This action should not be available.");
    msgBox.exec();
  }
}

void GuiInteractiveController::refreshSVG() {
  if (interactive_mode == InteractiveMode::GAME_INTERACTIVE) {

    auto coloring = tobor::v1_0::tobor_graphics<
        GameController::positions_of_pieces_type>::coloring("red", "yellow",
                                                            "green", "blue");

    // coloring =
    // originalGenerator.obtain_standard_4_coloring_permutation(coloring.colors);
    // we also have to permutate the selected (user input) color!
    // Otherwise choosing the yellow duck e.g. moves the blue duck.

    std::string example_svg_string =
        tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::
            draw_tobor_world(gameHistory.back().tobor_world,
                             gameHistory.back().path.back(),
                             gameHistory.back().target_cell, coloring);

    mainWindow->viewSvgInMainView(example_svg_string);
  }

  if (interactive_mode == InteractiveMode::NO_GAME) {
    QGraphicsScene *scene = new QGraphicsScene();
    mainWindow->ui->graphicsView->setScene(scene);
  }
}

void GuiInteractiveController::viewNumberOfSteps() {
  QString number_of_steps = QString::number(gameHistory.back().path.size() - 1);

  mainWindow->setWindowTitle(number_of_steps);
}

void GuiInteractiveController::movePiece(
    const tobor::v1_0::direction &direction) {
  gameHistory.back().movePiece(selected_piece_id, direction);

  mainWindow->ui->actionMoveBack->setEnabled(true);

  refreshSVG();

  viewNumberOfSteps();
  // refresh number of steps...
}

void GuiInteractiveController::undo() {
  auto &vec = gameHistory.back().path;

  if (vec.size() > 1) {
    vec.pop_back();
  }

  mainWindow->ui->actionMoveBack->setEnabled(vec.size() > 1);

  refreshSVG();
  viewNumberOfSteps();
}
