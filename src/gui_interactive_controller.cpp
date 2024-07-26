#include "predefined.h"
#include "gui_interactive_controller.h"

#include "gui_helper.h"

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QStringListModel>

#include <stdexcept>


/**
*	@brief Starts a reference game where 22 steps are needed until goal
*	is deprecated, just for development and debugging
*/
void GuiInteractiveController::startReferenceGame22() {
	if (current_game) {
		return showErrorDialog("This action should not be available.");
	}

	auto fac = SpecialCaseGameFactory();

	factory_history.emplace_back(fac.clone());

	current_game.reset(fac.create());

	current_color_vector = tobor::v1_0::color_vector::get_standard_coloring(static_cast<uint8_t>(current_game->count_pieces())); // standard coloring without permutation

	createColorActions();

	refreshAll();
}

void GuiInteractiveController::startGame() {
	// when editing, also edit inline void startReferenceGame22Helper(X& guiInteractiveController) and maybe use a new fascade for creating a new game
	if (current_game) {
		return showErrorDialog("This action should not be available.");
	}

	auto& fac{ next_factory_1[factory_select] };

	factory_history.emplace_back(fac->clone());

	current_game.reset(fac->create());


	fac->increment();

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

	std::vector<DRWGameController::pieces_quantity_type::int_type> not_yet_permutated;

	for (DRWGameController::pieces_quantity_type::int_type i = 0; i < DRWGameController::pieces_quantity_type::COUNT_ALL_PIECES; ++i) {
		not_yet_permutated.push_back(i);
	}

	std::vector<DRWGameController::pieces_quantity_type::int_type> colorPermutation = not_yet_permutated;

	if constexpr (DRWGameController::pieces_quantity_type::COUNT_ALL_PIECES == 4) {
		colorPermutation = boardGenerator.obtain_standard_4_coloring_permutation(not_yet_permutated);
	}
	*/

	current_color_vector = tobor::v1_0::color_vector::get_standard_coloring(static_cast<uint8_t>(current_game->count_pieces())); // standard coloring without permutation

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


	if (!current_game) {
		return showErrorDialog("This action should not be available.");
	}

	current_game.reset();

	refreshAll();
}

void GuiInteractiveController::moveBySolver(bool forward)
{
	current_game->move_by_solver(forward);

}

void GuiInteractiveController::selectPieceByColorId(const std::size_t& color_id)
{
	const bool OK{ current_game->select_piece_by_color_id(color_id) };

	if (!OK) throw std::logic_error("Illegal color_id.");

	refreshStatusbar();
}

void GuiInteractiveController::refreshMenuButtonEnable()
{
	if (!current_game) return mainWindow->setMenuButtonEnableForNoGame();
	
	if (current_game->solver()) return mainWindow->setMenuButtonEnableForSolverGame();
	
	return mainWindow->setMenuButtonEnableForInteractiveGame(); 
}

void GuiInteractiveController::refreshStatusbar() {
	if (current_game) {
		auto current_color = current_color_vector.colors[current_game->selected_piece_color_id()].getQColor();
		mainWindow->statusbarItems.setSelectedPiece(current_color);
	}
	else {
		mainWindow->statusbarItems.setSelectedPiece(Qt::darkGray);
	}
	mainWindow->refreshNumberOfSteps();
}

void GuiInteractiveController::refreshAll() {
	mainWindow->refreshSVG();
	refreshStatusbar();
	refreshMenuButtonEnable();
	viewSolutionPaths();
}

void GuiInteractiveController::movePiece(const tobor::v1_0::direction& direction) {

	if (!current_game) {
		showErrorDialog("Cannot move a piece with no game opened."); // should not be reachable, disable actions!
		return;
	}

	if (current_game->solver()) {
		if ((direction == tobor::v1_0::direction::EAST()) || (direction == tobor::v1_0::direction::WEST())) {
			moveBySolver(direction == tobor::v1_0::direction::EAST());
			refreshAll();
		}
		// else ignore input
		return;
	}

	current_game->move_selected(direction);
	mainWindow->statusBar()->showMessage(QString::fromStdString(std::to_string(current_game->selected_piece_color_id())));
	refreshAll();
}

void GuiInteractiveController::undo() {

	if (!current_game) {
		return showErrorDialog("Cannot undo with no game opened.");
	}
	if (current_game->solver()) {
		return showErrorDialog("Cannot yet undo in solver mode.");
	}
	current_game->undo();
	refreshAll();
}

void GuiInteractiveController::startSolver()
{
	if (!current_game) {
		return showErrorDialog("Cannot start solver with no game opened.");
	}

	auto showMessage = [&](const std::string& m) {
		mainWindow->statusBar()->showMessage(m.c_str());
		mainWindow->repaint();
		};

	current_game->start_solver(showMessage);
	viewSolutionPaths();
	refreshAll();
}

void GuiInteractiveController::stopSolver()
{
	if (!current_game) {
		return showErrorDialog("Cannot start solver with no game opened.");
	}
	current_game->stop_solver();
	viewSolutionPaths();
	refreshAll();
}

void GuiInteractiveController::selectSolution(std::size_t index)
{
	if (!current_game) {
		return showErrorDialog("Cannot select solution with no game opened.");
	}
	if (!current_game->solver()) {
		return showErrorDialog("Cannot select solution without running solver.");
	}

	current_game->select_solution(index);
}

void GuiInteractiveController::viewSolutionPaths() // this has to be improved!!!
{
	static QStringListModel* model{ nullptr };

	if (model == nullptr) {
		model = new QStringListModel();
	}

	auto permutated_color_vector = current_color_vector;

	QStringList qStringList;

	if (current_game) {
		qStringList = current_game->optimal_solutions_list(permutated_color_vector);
	}

	model->setStringList(qStringList);

	mainWindow->ui->listView->setModel(model); // not needed multiple times ###

}

void GuiInteractiveController::highlightGeneratedTargetCells()
{
	if (!current_game) {
		return showErrorDialog("Target cell markers not supported without running a game");
	}
	std::pair<std::string, std::size_t> svg_and_count = factory_history.back()->svg_highlighted_targets();

	mainWindow->viewSvgInMainView(svg_and_count.first);

	QString m{ "Number of generator target cells:   " };
	m += QString::number(svg_and_count.second);
	mainWindow->ui->statusbar->showMessage(m);
}
