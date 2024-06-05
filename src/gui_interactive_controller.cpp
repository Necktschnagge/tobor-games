#include "predefined.h"
#include "gui_interactive_controller.h"

#include "gui_helper.h"

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QStringListModel>

#include "tobor_svg.h" // has to be reordered!

#include <stdexcept>



template<class X>
inline void startReferenceGame22Helper(X& guiInteractiveController) {
	if constexpr (!
		(GameController::piece_quantity_type::COUNT_TARGET_PIECES == 1 && GameController::piece_quantity_type::COUNT_NON_TARGET_PIECES == 3)
		) {
		return;
	}
	else {


		if (guiInteractiveController.interactive_mode != X::InteractiveMode::NO_GAME) {
			return showErrorDialog("This action should not be available.");
		}

		guiInteractiveController.interactive_mode = X::InteractiveMode::GAME_INTERACTIVE;

		guiInteractiveController.current_game = SpecialCaseGameFactory().create(); // put in history!

		guiInteractiveController.current_color_vector = tobor::v1_0::color_vector::get_standard_coloring(GameController::piece_quantity_type::COUNT_ALL_PIECES);

		guiInteractiveController.createColorActions();

		guiInteractiveController.refreshAll();

	}

}

/**
*	@brief Starts a reference game where 22 steps are needed until goal
*	is deprecated, just for development and debugging
*/
void GuiInteractiveController::startReferenceGame22() {
	return startReferenceGame22Helper(*this); // need to do it in some templated context so that if constexpr - else paths will not cause errors.
}

void GuiInteractiveController::startGame() {
	// when editing, also edit inline void startReferenceGame22Helper(X& guiInteractiveController) and maybe use a new fascade for creating a new game
	if (interactive_mode != InteractiveMode::NO_GAME) {
		return showErrorDialog("This action should not be available.");
	}

	interactive_mode = InteractiveMode::GAME_INTERACTIVE;

	std::shared_ptr<GameFactory> current_factory = std::make_shared<OriginalGameFactory<GameController::piece_quantity_type>>(next_factory);
	++next_factory;

	game_history_222.push_back(current_factory);

	current_game = current_factory->create();


	/// old stuff...

	/*
	auto& boardGenerator{ current_factory->.main() };

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

	std::vector<GameController::piece_quantity_type::int_type> not_yet_permutated;

	for (GameController::piece_quantity_type::int_type i = 0; i < GameController::piece_quantity_type::COUNT_ALL_PIECES; ++i) {
		not_yet_permutated.push_back(i);
	}

	std::vector<GameController::piece_quantity_type::int_type> colorPermutation = not_yet_permutated;

	if constexpr (GameController::piece_quantity_type::COUNT_ALL_PIECES == 4) {
		colorPermutation = boardGenerator.obtain_standard_4_coloring_permutation(not_yet_permutated);
	}
	*/

	current_color_vector = tobor::v1_0::color_vector::get_standard_coloring(GameController::piece_quantity_type::COUNT_ALL_PIECES); // standard coloring without permutation

	createColorActions();

	refreshAll();
}


void GuiInteractiveController::createColorActions()
{

	QMenu* sub = mainWindow->getSelectPieceSubMenu();

	sub->clear();
	// actions are deleted,
	// according to QSignalMapper's docs, the map entries for these objects will be deleted on their destruction.


	for (std::size_t i = 0; i < current_color_vector.colors.size(); ++i) {

		auto action = sub->addAction(
			current_color_vector.colors[i].UPPERCASE_display_string_with_underscore()
		);
		// TODO need to add shortcuts (?) STRG+R ...

		mainWindow->inputConnections.push_back(
			QObject::connect(action, &QAction::triggered, mainWindow->signalMapper, qOverload<>(&QSignalMapper::map), Qt::AutoConnection)
		);
		//QObject::connect(action, &QAction::triggered, mainWindow->signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map), Qt::AutoConnection);

		mainWindow->signalMapper->setMapping(action, static_cast<int>(i));
	}
}

void GuiInteractiveController::stopGame() {

	mainWindow->statusbarItems.setSelectedPiece(Qt::darkGray);

	mainWindow->disconnectInputConnections();

	mainWindow->getSelectPieceSubMenu()->clear();


	if (interactive_mode == InteractiveMode::NO_GAME) {
		return showErrorDialog("This action should not be available.");
	}

	interactive_mode = InteractiveMode::NO_GAME;
	refreshAll();
}

void GuiInteractiveController::moveBySolver(bool forward)
{
	current_game->move_by_solver(forward);

}

void GuiInteractiveController::setPieceId(const GameController::piece_id_type& piece_id) {

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

	refreshStatusbar();
}

void GuiInteractiveController::selectPieceByColorId(const std::size_t& color_id)
{
	auto iter = std::find(
		current_game->current_state().get_permutation().cbegin(),
		current_game->current_state().get_permutation().cend(),
		//current_game->color_permutation().cbegin(),
		//current_game->color_permutation().cend(),
		color_id
	);

	const typename decltype(iter)::difference_type index{ iter - current_game->current_state().get_permutation().cbegin() };

	if (iter == current_game->current_state().get_permutation().cend())
		throw std::logic_error("Illegal color_id.");

	setPieceId(index);
}


void GuiInteractiveController::refreshSVG()
{
	if (interactive_mode == InteractiveMode::GAME_INTERACTIVE || interactive_mode == InteractiveMode::SOLVER_INTERACTIVE_STEPS) {

		auto permutated_color_vector = current_color_vector;

		for (std::size_t i{ 0 }; i < current_color_vector.colors.size(); ++i) {
			permutated_color_vector.colors[i] = current_color_vector.colors[current_game->current_state().get_permutation()[i]];
		}

		graphics_type::coloring coloring =
			make_coloring(
				permutated_color_vector,
				std::make_integer_sequence<GameController::piece_quantity_type::int_type, GameController::piece_quantity_type::COUNT_ALL_PIECES>{}
		);

		graphics_type::piece_shape_selection shape{ graphics_type::piece_shape_selection::BALL };

		if (mainWindow->shapeSelectionItems.getSelectedShape() == mainWindow->shapeSelectionItems.duck) {
			shape = graphics_type::piece_shape_selection::DUCK;
		}
		// else default ball.

		std::string example_svg_string =
			graphics_type::draw_tobor_world(
				current_game->world(),
				current_game->current_state().naked(),
				current_game->target_cell(),
				coloring,
				shape
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

		mainWindow->ui->actionMoveBack->setEnabled(!current_game->is_initial());

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

		mainWindow->ui->actionMoveBack->setEnabled(!current_game->is_initial());

		mainWindow->ui->menuSelect_Piece->setEnabled(false);

		mainWindow->ui->menuMove->setEnabled(false);

		mainWindow->ui->menuPlaySolver->setEnabled(true);
	}


}

void GuiInteractiveController::refreshStatusbar() {

	if (interactive_mode == InteractiveMode::GAME_INTERACTIVE || interactive_mode == InteractiveMode::SOLVER_INTERACTIVE_STEPS) {

		auto current_color = current_color_vector.colors[
			current_game->current_state().get_permutation()[
				selected_piece_id.value
			]
		].getQColor();

			mainWindow->statusbarItems.setSelectedPiece(current_color);

	}
	else {
		mainWindow->statusbarItems.setSelectedPiece(Qt::darkGray);
	}

	refreshNumberOfSteps();

}

void GuiInteractiveController::refreshNumberOfSteps() {

	QString number_of_steps;

	if (interactive_mode == InteractiveMode::GAME_INTERACTIVE || interactive_mode == InteractiveMode::SOLVER_INTERACTIVE_STEPS) {

		number_of_steps = QString::number(current_game->depth());

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

		current_game->move_feedback(selected_piece_id, direction); // here we need to update selected piece_id because of reordering. or we need to store colors...
		mainWindow->statusBar()->showMessage(QString::fromStdString(std::to_string((unsigned int)(selected_piece_id.value))));

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
		current_game->undo();
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
	auto showMessage = [&](const std::string& m) {
		mainWindow->statusBar()->showMessage(m.c_str());
		mainWindow->repaint();
		};

	current_game->start_solver(showMessage);
	interactive_mode = InteractiveMode::SOLVER_INTERACTIVE_STEPS;
	viewSolutionPaths();
	refreshAll();
}

void GuiInteractiveController::stopSolver()
{
	interactive_mode = InteractiveMode::GAME_INTERACTIVE;
	current_game->stop_solver();
	viewSolutionPaths();
	refreshAll();
}

void GuiInteractiveController::selectSolution(std::size_t index)
{
	if (interactive_mode != InteractiveMode::SOLVER_INTERACTIVE_STEPS) {
		return showErrorDialog("Cannot select any solver solution when not in solver mode!");
	}
	current_game->select_solution(index);
	current_game->reset_solver_steps();
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

	//std::size_t goal_counter{ 0 };

	auto permutated_color_vector = current_color_vector;

	//for (std::size_t i{ 0 }; i < current_color_vector.colors.size(); ++i) {
	//	permutated_color_vector.colors[i] = current_color_vector.colors[current_game->solver_begin_state().get_permutation()[i]];
	//}

	const auto& partitions{ current_game->optimal_solutions() /* get solver's move path for displaying on the upper right of main window */ };

	for (std::size_t i{ 0 }; i < partitions.size(); ++i) {
		QString s;
		s = s + "[" + QString::number(i) + "]     ";
		for (const GameController::piece_move_type& m : partitions[i].second.vector()) {
			//is not checked for emptiness!!

			const char letter{ permutated_color_vector.colors[m.pid.value].UPPERCASE_shortcut_letter() };

			std::string color = std::string(1, letter);

			s = s + "  " + QString::fromStdString(color) + QString::fromStdString(static_cast<std::string>(m.dir));

		}
		s = s + "     ( NO COUNT " + /*QString::number(partitions[i].size()) + */ ")";
		qStringList << s;
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

	const auto& world{ current_game->world() };

	auto raw_cell_id_vector = dynamic_cast<OriginalGameFactory<GameController::piece_quantity_type>*>(game_history_222.back().get())->product_generator().main().get_target_cell_id_vector(world);

	std::vector<GameController::cell_id_type> comfort_cell_id_vector;

	std::transform(raw_cell_id_vector.cbegin(), raw_cell_id_vector.cend(), std::back_inserter(comfort_cell_id_vector),
		[&](const auto& raw_cell_id) {
			return GameController::cell_id_type::create_by_id(raw_cell_id, world);
		}
	);

	std::string svg_string = graphics_type::draw_tobor_world_with_cell_markers(
		world,
		comfort_cell_id_vector
	);

	mainWindow->viewSvgInMainView(svg_string);

	QString m{ "Number of generator target cells:   " };
	m += QString::number(comfort_cell_id_vector.size());
	mainWindow->ui->statusbar->showMessage(m);
}
