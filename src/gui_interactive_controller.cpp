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

void GuiInteractiveController::moveBySolver(bool forward)
{
	gameHistory.back().moveBySolver(forward);

}

void GuiInteractiveController::setPieceId(const tobor::v1_0::default_piece_id& piece_id) {

	switch (interactive_mode)
	{
	case GuiInteractiveController::InteractiveMode::NO_GAME:
		showErrorDialog("Cannot select a piece with no game opened.");
		break;
	case GuiInteractiveController::InteractiveMode::GAME_INTERACTIVE:
		this->selected_piece_id = piece_id;
		break;
	case GuiInteractiveController::InteractiveMode::SOLVER_INTERACTIVE_STEPS:
		showErrorDialog("Cannot select a piece while in Solver Mode."); // should never be reached (?), TODO disable acrions...
		break;
	default:
		showErrorDialog(
			QString("Undefined setPieceId for mode ") +
			QString::number(static_cast<uint64_t>(interactive_mode))
		);
		break;
	}

}

void GuiInteractiveController::refreshSVG()
{
	if (interactive_mode == InteractiveMode::GAME_INTERACTIVE || interactive_mode == InteractiveMode::SOLVER_INTERACTIVE_STEPS) {

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

	switch (interactive_mode)
	{
	case GuiInteractiveController::InteractiveMode::NO_GAME:
		showErrorDialog("Cannot move a piece with no game opened."); // should not be reachable, disable actions!
		break;
	case GuiInteractiveController::InteractiveMode::GAME_INTERACTIVE:

		gameHistory.back().movePiece(selected_piece_id, direction);

		refreshMenuButtonEnable();

		refreshSVG();

		refreshNumberOfSteps();

		break;

	case GuiInteractiveController::InteractiveMode::SOLVER_INTERACTIVE_STEPS:
		if ((direction == tobor::v1_0::direction::EAST()) || (direction == tobor::v1_0::direction::WEST())) {
			moveBySolver(direction == tobor::v1_0::direction::EAST());
			refreshAll();
		} // else ignore input
		break;

	default:
		showErrorDialog(
			QString("Undefined piece move for mode ") +
			QString::number(static_cast<uint64_t>(interactive_mode))
		);
		break;
	}
}

void GuiInteractiveController::undo() {

	// switch SOLVER_MODE

	gameHistory.back().undo();

	refreshSVG();

	refreshNumberOfSteps();

	refreshMenuButtonEnable();

}

void GuiInteractiveController::startSolver()
{
	gameHistory.back().startSolver(mainWindow);
	interactive_mode = InteractiveMode::SOLVER_INTERACTIVE_STEPS;
	viewSolutionPaths();
}

void GuiInteractiveController::selectSolution(std::size_t index)
{
	gameHistory.back().selectSolution(index);
	gameHistory.back().resetSolverSteps();
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

void GameController::startSolver(QMainWindow* mw) {

	// set solver begin
	solver_begin_index = path.size();

	// build graph
	optional_solver_state_graph.emplace(currentState());
	partial_state_graph_type& graph{ optional_solver_state_graph.value() };
	graph.build_state_graph_for_all_optimal_solutions(move_one_piece_calculator, target_cell);

	mw->statusBar()->showMessage("Extracting solution paths...");
	mw->repaint();

	// optimal paths
	std::map<positions_of_pieces_type, std::vector<move_path_type>> optimal_paths_map{ graph.optimal_paths(target_cell) };

	mw->statusBar()->showMessage("Classifying solution paths...");
	mw->repaint();

	// classify optimal paths...
	optional_classified_move_paths.reset();
	optional_classified_move_paths.emplace();
	auto& classified_move_paths{ optional_classified_move_paths.value() };

	for (const auto& pair : optimal_paths_map) {
		classified_move_paths[pair.first] = move_path_type::interleaving_partitioning(pair.second);
		for (auto& equivalence_class : classified_move_paths[pair.first]) {
			std::sort(equivalence_class.begin(), equivalence_class.end(), move_path_type::antiprettiness_relation);
		}
	}
	mw->statusBar()->showMessage("Done");
	mw->repaint();
}

void GameController::moveBySolver(bool forward) {

	auto& move_path = get_selected_solution_representant(selected_solution_index);

	qDebug() << "move by solver: path length " << move_path.vector().size();

	const auto index_next_move = path.size() - solver_begin_index;

	qDebug() << "move by solver: index next move " << index_next_move;

	if (forward) {
		movePiece(move_path.vector()[index_next_move].pid, move_path.vector()[index_next_move].dir);
	}
	else { // back
		if (index_next_move == 0) // already at solver start
			return;
		path.pop_back();
	}

	//selected_solution_index
}
