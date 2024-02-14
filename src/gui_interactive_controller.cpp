#include "predefined.h"
#include "gui_interactive_controller.h"

#include "gui_helper.h"

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QStringListModel>
#include <QSignalMapper>

#include "tobor_svg.h" // has to be reordered!

#include <stdexcept>

/**
*	@brief Starts a reference game where 22 steps are needed until goal
*	is deprecated, just for development and debugging
*/
void GuiInteractiveController::startReferenceGame22() {

	interactive_mode = InteractiveMode::GAME_INTERACTIVE;

	auto world = GameController::world_type(16, 16);
	world.block_center_cells(2, 2);

	world.west_wall_by_id(6) = true;
	world.west_wall_by_id(11) = true;
	world.west_wall_by_id(16 + 2) = true;
	world.west_wall_by_id(16 + 14) = true;
	world.west_wall_by_id(3 * 16 + 7) = true;
	world.west_wall_by_id(3 * 16 + 10) = true;
	world.west_wall_by_id(4 * 16 + 1) = true;
	world.west_wall_by_id(5 * 16 + 11) = true;
	world.west_wall_by_id(6 * 16 + 4) = true;
	world.west_wall_by_id(6 * 16 + 14) = true;
	world.west_wall_by_id(8 * 16 + 11) = true;
	world.west_wall_by_id(9 * 16 + 14) = true;
	world.west_wall_by_id(10 * 16 + 3) = true;
	world.west_wall_by_id(11 * 16 + 5) = true;
	world.west_wall_by_id(12 * 16 + 2) = true;
	world.west_wall_by_id(13 * 16 + 11) = true;
	world.west_wall_by_id(14 * 16 + 6) = true;
	world.west_wall_by_id(14 * 16 + 14) = true;
	world.west_wall_by_id(15 * 16 + 4) = true;
	world.west_wall_by_id(15 * 16 + 10) = true;

	world.south_wall_by_transposed_id(2) = true;
	world.south_wall_by_transposed_id(9) = true;
	world.south_wall_by_transposed_id(16 + 4) = true;
	world.south_wall_by_transposed_id(16 + 13) = true;
	world.south_wall_by_transposed_id(2 * 16 + 2) = true;
	world.south_wall_by_transposed_id(2 * 16 + 10) = true;
	world.south_wall_by_transposed_id(3 * 16 + 7) = true;
	world.south_wall_by_transposed_id(5 * 16 + 12) = true;
	world.south_wall_by_transposed_id(6 * 16 + 3) = true;
	world.south_wall_by_transposed_id(6 * 16 + 14) = true;
	world.south_wall_by_transposed_id(9 * 16 + 4) = true;
	world.south_wall_by_transposed_id(10 * 16 + 9) = true;
	world.south_wall_by_transposed_id(11 * 16 + 5) = true;
	world.south_wall_by_transposed_id(11 * 16 + 13) = true;
	world.south_wall_by_transposed_id(13 * 16 + 1) = true;
	world.south_wall_by_transposed_id(13 * 16 + 9) = true;
	world.south_wall_by_transposed_id(14 * 16 + 7) = true;
	world.south_wall_by_transposed_id(14 * 16 + 15) = true;
	world.south_wall_by_transposed_id(15 * 16 + 5) = true;
	world.south_wall_by_transposed_id(15 * 16 + 12) = true;

	gameHistory.emplace_back(
		world,
		GameController::positions_of_pieces_type(
			{
				GameController::cell_id_type::create_by_coordinates(15, 15, world)
			},
			{
				GameController::cell_id_type::create_by_coordinates(1,0, world),
				GameController::cell_id_type::create_by_coordinates(12,14, world),
				GameController::cell_id_type::create_by_coordinates(12,15, world)
			}
		),
		GameController::cell_id_type::create_by_coordinates(9, 3, world)
	);

	refreshAll();

}

void GuiInteractiveController::startGame() {

	if (interactive_mode != InteractiveMode::NO_GAME) {
		return showErrorDialog("This action should not be available.");
	}

	interactive_mode = InteractiveMode::GAME_INTERACTIVE;


	auto& boardGenerator{ productWorldGenerator.main() };

	auto& initialStateGenerator{ productWorldGenerator.side() };

	auto tup = boardGenerator.split_element();
	(void)tup;
	auto x = std::get<0>(tup);
	auto a = x % 4; x /= 4;
	auto b = x % 4; x /= 4;
	auto c = x % 4; x /= 4;
	auto d = x % 4; x /= 4;
	auto e = x;
	qDebug() << a << "   :   " << b << "   :   " << c << "   :   " << d << "   :   " << e << "   :   " << std::get<1>(tup) << "   :   " << std::get<2>(tup) << "\n";
	qDebug() << boardGenerator.get_counter() << "\n";

	auto world = boardGenerator.get_tobor_world();

	auto target = boardGenerator.get_target_cell();

	gameHistory.emplace_back(
		world,
		initialStateGenerator.get_positions_of_pieces(world),
		target
	);

	++productWorldGenerator;

	refreshAll();

	current_color_vector = tobor::v1_0::color_vector::get_standard_coloring(); // giva a size, how many colors!

	createColorActions();

}


void GuiInteractiveController::createColorActions()
{

	QMenu* sub = mainWindow->getSelectPieceSubMenu();

	sub->clear();
	// actions are deleted,
	// according to QSignalMapper's docs, the map entries for these objects will be deleted on their destruction.


	for (std::size_t i = 0; i < current_color_vector.colors.size(); ++i) {

		auto action = sub->addAction(
			QString::fromStdString(
				current_color_vector.colors[i].display_string_with_underscore
			)
		);

		mainWindow->inputConnections.push_back(
			QObject::connect(action, &QAction::triggered, mainWindow->signalMapper, qOverload<>(&QSignalMapper::map), Qt::AutoConnection)
		);
		//QObject::connect(action, &QAction::triggered, mainWindow->signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map), Qt::AutoConnection);

		mainWindow->signalMapper->setMapping(action, static_cast<int>(i));

		//qDebug() << "added  " << current_color_vector.colors[i].display_string_with_underscore;
	}

	
}

void GuiInteractiveController::stopGame() {

	mainWindow->disconnectInputConnections();
	mainWindow->getSelectPieceSubMenu()->clear();


	if (interactive_mode == InteractiveMode::NO_GAME) {
		return showErrorDialog("This action should not be available.");
	}

	gameHistory.back().lazyFreeSolverData();

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

		mainWindow->ui->actionStop_Solver->setEnabled(false);

		mainWindow->ui->actionMoveBack->setEnabled(!gameHistory.back().isEmptyPath());

		mainWindow->ui->menuSelect_Piece->setEnabled(true);

		mainWindow->ui->menuMove->setEnabled(true);

		mainWindow->ui->menuPlaySolver->setEnabled(false);

	}
	else if (interactive_mode == InteractiveMode::NO_GAME) {

		mainWindow->ui->actionNewGame->setEnabled(true);

		mainWindow->ui->actionStopGame->setEnabled(false);

		mainWindow->ui->actionStart_Solver->setEnabled(false);

		mainWindow->ui->actionStop_Solver->setEnabled(false);

		mainWindow->ui->actionMoveBack->setEnabled(false);

		mainWindow->ui->menuSelect_Piece->setEnabled(false);

		mainWindow->ui->menuMove->setEnabled(false);

		mainWindow->ui->menuPlaySolver->setEnabled(false);

	}
	else if (interactive_mode == InteractiveMode::SOLVER_INTERACTIVE_STEPS) {

		mainWindow->ui->actionNewGame->setEnabled(false);

		mainWindow->ui->actionStopGame->setEnabled(true);

		mainWindow->ui->actionStart_Solver->setEnabled(false);

		mainWindow->ui->actionStop_Solver->setEnabled(true);

		mainWindow->ui->actionMoveBack->setEnabled(!gameHistory.back().isEmptyPath());

		mainWindow->ui->menuSelect_Piece->setEnabled(false);

		mainWindow->ui->menuMove->setEnabled(false);

		mainWindow->ui->menuPlaySolver->setEnabled(true);
	}


}

void GuiInteractiveController::refreshStatusbar() {

	// may be initial, not updated everytime:

	MainWindow::SvgViewToolchain new_chain; // this data type aggregates more member data than enough.

	new_chain.q_graphics_scene = std::make_unique<QGraphicsScene>();
	// currently we do not show the SVG inside the graphicsScene

	mainWindow->statusbarItems.colorSquare->setScene(new_chain.q_graphics_scene.get()); // does not take ownership
	mainWindow->statusbarItems.colorSquare->fitInView(new_chain.q_graphics_scene.get()->sceneRect(), Qt::IgnoreAspectRatio);
	mainWindow->statusbarItems.colorSquare->show();

	mainWindow->statusbarItems.svgC = std::move(new_chain); // then destroy old objects in reverse order compared to construction...

	if (interactive_mode == InteractiveMode::GAME_INTERACTIVE || interactive_mode == InteractiveMode::SOLVER_INTERACTIVE_STEPS) {

		int r{ 240 };
		int g{ 20 };
		int b{ 50 };

		auto color = QColor(r, g, b);

		auto brush = QBrush(color);

		mainWindow->statusbarItems.colorSquare->setBackgroundBrush(brush);

	}
	else {

		mainWindow->statusbarItems.colorSquare->setBackgroundBrush(Qt::white);
	}

	refreshNumberOfSteps();

}

void GuiInteractiveController::refreshNumberOfSteps() {

	QString number_of_steps;

	if (interactive_mode == InteractiveMode::GAME_INTERACTIVE || interactive_mode == InteractiveMode::SOLVER_INTERACTIVE_STEPS) {

		number_of_steps = QString::number(gameHistory.back().path.size() - 1);

	}

	mainWindow->statusbarItems.stepsValue->setText(number_of_steps);
}

void GuiInteractiveController::movePiece(const tobor::v1_0::direction& direction) {

	switch (interactive_mode)
	{
	case GuiInteractiveController::InteractiveMode::NO_GAME:
		showErrorDialog("Cannot move a piece with no game opened."); // should not be reachable, disable actions!
		break;
	case GuiInteractiveController::InteractiveMode::GAME_INTERACTIVE:

		gameHistory.back().movePiece(selected_piece_id, direction);

		refreshAll();

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

	switch (interactive_mode)
	{
	case GuiInteractiveController::InteractiveMode::NO_GAME:
		return showErrorDialog("Cannot undo with no game opened.");

	case GuiInteractiveController::InteractiveMode::GAME_INTERACTIVE:
		gameHistory.back().undo();
		refreshAll();
		break;

	case GuiInteractiveController::InteractiveMode::SOLVER_INTERACTIVE_STEPS:
		return showErrorDialog("Cannot yet undo in solver mode.");

	default:
		showErrorDialog(
			QString("Undefined undo for mode ") +
			QString::number(static_cast<uint64_t>(interactive_mode))
		);
		break;
	}
}

void GuiInteractiveController::startSolver()
{
	gameHistory.back().startSolver(mainWindow);
	interactive_mode = InteractiveMode::SOLVER_INTERACTIVE_STEPS;
	viewSolutionPaths();
	refreshAll();
}

void GuiInteractiveController::stopSolver()
{
	interactive_mode = InteractiveMode::GAME_INTERACTIVE;
	gameHistory.back().stopSolver();
	viewSolutionPaths();
	refreshAll();
}

void GuiInteractiveController::selectSolution(std::size_t index)
{
	if (interactive_mode != InteractiveMode::SOLVER_INTERACTIVE_STEPS) {
		return showErrorDialog("Cannot select any solver solution when not in solver mode!");
	}
	gameHistory.back().selectSolution(index);
	gameHistory.back().resetSolverSteps();
}

void GuiInteractiveController::viewSolutionPaths() // this has to be improved!!!
{
	static QStringListModel* model{ nullptr };

	if (model == nullptr) {
		model = new QStringListModel();
	}

	if (interactive_mode != InteractiveMode::SOLVER_INTERACTIVE_STEPS) {

		QStringList emptyStringList;

		model->setStringList(emptyStringList);
		mainWindow->ui->listView->setModel(model);
		return;
	}


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



	model->setStringList(qStringList);

	mainWindow->ui->listView->setModel(model);

}

void GuiInteractiveController::highlightGeneratedTargetCells()
{
	const bool STATE_OK{ interactive_mode == InteractiveMode::GAME_INTERACTIVE || interactive_mode == InteractiveMode::SOLVER_INTERACTIVE_STEPS };

	if (!STATE_OK) {
		return showErrorDialog("Target cell markers not supported without running a game");
	}

	auto& world{ gameHistory.back().tobor_world };

	auto raw_cell_id_vector = productWorldGenerator.main().get_target_cell_id_vector(world);

	std::vector<GameController::cell_id_type> comfort_cell_id_vector;

	std::transform(raw_cell_id_vector.cbegin(), raw_cell_id_vector.cend(), std::back_inserter(comfort_cell_id_vector),
		[&](const auto& raw_cell_id) {
			return GameController::cell_id_type::create_by_id(raw_cell_id, world);
		}
	);

	std::string svg_string = tobor::v1_0::tobor_graphics<GameController::positions_of_pieces_type>::draw_tobor_world_with_cell_markers(
		world,
		comfort_cell_id_vector
	);

	mainWindow->viewSvgInMainView(svg_string);

	QString m{ "Number of generator target cells:   " };
	m += QString::number(comfort_cell_id_vector.size());
	mainWindow->ui->statusbar->showMessage(m);
}

GameController::move_path_type& GameController::get_selected_solution_representant(std::size_t index) {
	for (auto& pair : optional_classified_move_paths.value()) {
		//for (auto& equi_class : pair.second) {
		if (index < pair.second.size()) {
			return pair.second[index].front();
		}
		index -= pair.second.size();
		//}

	}
	// out of range
	showErrorDialog("selected solution index out of range, try to use teh first solution instead.");
	if (optional_classified_move_paths.value().empty()) {
		showErrorDialog("no solution found at all [1]");
		throw std::runtime_error("should select a solution where there is no solution [1]");
	}

	if (optional_classified_move_paths.value().begin()->second.empty()) {
		showErrorDialog("no solution found in first class [2]");
		throw std::runtime_error("should select a solution where there is no solution [2]");
	}
	if (optional_classified_move_paths.value().begin()->second.front().empty()) {
		showErrorDialog("no solution found in first class [3]");
		throw std::runtime_error("should select a solution where there is no solution [3]");
	}
	return optional_classified_move_paths.value().begin()->second.front().front();
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
		if (isFinal())
			return;
		movePiece(move_path.vector()[index_next_move].pid, move_path.vector()[index_next_move].dir);
	}
	else { // back
		if (index_next_move == 0) // already at solver start
			return;
		path.pop_back();
	}

	//selected_solution_index
}
