#include "predefined.h"
#include "gui_interactive_controller.h"

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QStringListModel>
#include "tobor_svg.h"


void GuiInteractiveController::startGame() {

	if (interactive_mode != InteractiveMode::NO_GAME) {

		QMessageBox msgBox(QMessageBox::Icon::Critical, QString("GUI ERROR"), "This action should not be available.");
		msgBox.exec();

		return;
	}

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
	qDebug() << originalGenerator.get_counter() << "\n";
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



	refreshAll();
}

void GuiInteractiveController::stopGame() {

	if (interactive_mode == InteractiveMode::NO_GAME) {

		QMessageBox msgBox(QMessageBox::Icon::Critical, QString("GUI ERROR"), "This action should not be available.");
		msgBox.exec();

	}

	interactive_mode = InteractiveMode::NO_GAME;

	refreshAll();

}

void GuiInteractiveController::refreshSVG()
{
	if (interactive_mode == InteractiveMode::GAME_INTERACTIVE) {

		auto coloring = tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::coloring("red", "green", "blue", "yellow");

		// coloring = originalGenerator.obtain_standard_4_coloring_permutation(coloring.colors);
		// we also have to permutate the selected (user input) color!
		// Otherwise choosing the yellow duck e.g. moves the blue duck.

		std::string example_svg_string =
			tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::draw_tobor_world(
				gameHistory.back().tobor_world,
				gameHistory.back().path.back(),
				gameHistory.back().target_cell,
				coloring
			);

		mainWindow->viewSvgInMainView(example_svg_string);
	}

	if (interactive_mode == InteractiveMode::NO_GAME) {
		QGraphicsScene* scene = new QGraphicsScene();
		mainWindow->ui->graphicsView->setScene(scene);
	}
}

void GuiInteractiveController::refreshMenuButtonEnable()
{
	if (interactive_mode == InteractiveMode::GAME_INTERACTIVE) {

		mainWindow->ui->actionNewGame->setEnabled(false);

		mainWindow->ui->actionStopGame->setEnabled(true);

		mainWindow->ui->actionStart_Solver->setEnabled(true);

		mainWindow->ui->actionMoveBack->setEnabled(!gameHistory.back().isEmptyPath());

	}
	else if (interactive_mode == InteractiveMode::NO_GAME) {

		mainWindow->ui->actionNewGame->setEnabled(true);

		mainWindow->ui->actionStopGame->setEnabled(false);

		mainWindow->ui->actionStart_Solver->setEnabled(false);

		mainWindow->ui->actionMoveBack->setEnabled(false);

	}
	else if (interactive_mode == InteractiveMode::SOLVER_INTERACTIVE_STEPS) {

		mainWindow->ui->actionNewGame->setEnabled(false);

		mainWindow->ui->actionStopGame->setEnabled(true);

		mainWindow->ui->actionStart_Solver->setEnabled(true);

		mainWindow->ui->actionMoveBack->setEnabled(!gameHistory.back().isEmptyPath());

	}


}

void GuiInteractiveController::refreshNumberOfSteps() {

	QString number_of_steps = QString::number(gameHistory.back().path.size() - 1);

	mainWindow->setWindowTitle(number_of_steps);

}

void GuiInteractiveController::movePiece(const tobor::v1_0::direction& direction) {

	gameHistory.back().movePiece(selected_piece_id, direction);

	refreshMenuButtonEnable();

	refreshSVG();

	refreshNumberOfSteps();

}

void GuiInteractiveController::undo() {

	gameHistory.back().undo();

	refreshSVG();

	refreshNumberOfSteps();

	refreshMenuButtonEnable();

}

void GuiInteractiveController::startSolver()
{
	gameHistory.back().startSolver();
	viewSolutionPaths();
}

void GuiInteractiveController::viewSolutionPaths()
{
	QStringList qStringList;


	std::size_t goal_counter{ 0 };

	for (const auto& pair : gameHistory.back().optional_classified_move_paths.value()) {
		//const auto& goal_state{ pair.first };
		const auto& equivalence_classes{ pair.second };

		for (std::size_t i = 0; i < equivalence_classes.size(); ++i) {
			QString s;
			s = s + "[" + QString::number(goal_counter) + "]     ";
			s = s + QString::number(i) + ": ";
			for (const GameController::piece_move_type& m : equivalence_classes[i][0].vector()) {
				
				std::string color = "RGBY";
				color = color.substr(m.pid.value, 1); // this is not okay. we need to properly use the coloring array.
				// please check #69 so that we may include tobor svg in this file's corresponding header to define coloring there....

				// the solution might be a "global" fixed coloring with full words and with Letter, 
				// additionally a permutation which is applied anywhere before reading the colors,
				// and which is also used at that position where GUI input is interpreted.

				s = s + "  " + QString::fromStdString(color) + QString::fromStdString(static_cast<std::string>(m.dir));
			}
			s = s + "     (x" + QString::number(equivalence_classes[i].size()) + ")";
			qStringList << s;
		}
		++goal_counter;
	}

	static QStringListModel* model{ nullptr };

	if (model == nullptr) {
		model = new QStringListModel();
	}

	model->setStringList(qStringList);

	mainWindow->ui->listView->setModel(model);

}

