#include "predefined.h"

#include "mainwindow.h"

#include "gui/gui_helper.h"

#include "custom_traits.h"

#include "ui_mainwindow.h"
#include "gui/license_dialog.h"


#include "special_case_22_game_factory.h"
#include "original_game_factory.h"


#include "spdlog/spdlog.h"
#include "spdlog/sinks/qt_sinks.h"


#include <QStringListModel>
#include <QMessageBox>
#include <QDebug>
#include <QStyle>
#include <QXmlStreamReader>
#include <QGraphicsSvgItem>
#include <QMessageBox>

// please add a enable all actions in menu to developer menu!

void MainWindow::setTextAndShortcutsForMainMenu() {

	/// FILE

	menubar_root.rootMenu->file.menuFile->setTitle(QCoreApplication::translate("MainWindow", "&File", nullptr));
	//### export SVG feature missing here!

	/// EDIT

	menubar_root.rootMenu->edit.menuEdit->setTitle(QCoreApplication::translate("MainWindow", "&Edit", nullptr));

	/// GAME

	//MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Tobor 1.0", nullptr));
	menubar_root.rootMenu->game.menuGame->setTitle(QCoreApplication::translate("MainWindow", "&Game", nullptr));
	menubar_root.rootMenu->game.actionNewGame->setText(QCoreApplication::translate("MainWindow", "&New Game", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionNewGame->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.menuHistory->setTitle(QCoreApplication::translate("MainWindow", "&History...", nullptr));
	menubar_root.rootMenu->game.actionStopGame->setText(QCoreApplication::translate("MainWindow", "&Stop Game", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionStopGame->setShortcut(QCoreApplication::translate("MainWindow", "Esc", nullptr));
#endif // QT_CONFIG(shortcut)


	menubar_root.rootMenu->game.menuSelect_Piece->setTitle(QCoreApplication::translate("MainWindow", "Select &Piece...", nullptr));
	menubar_root.rootMenu->game.menuMove->setTitle(QCoreApplication::translate("MainWindow", "&Move...", nullptr));
	menubar_root.rootMenu->game.actionNORTH->setText(QCoreApplication::translate("MainWindow", "&NORTH", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionNORTH->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Up", nullptr));
#endif // QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionEAST->setText(QCoreApplication::translate("MainWindow", "&EAST", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionEAST->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Right", nullptr));
#endif // QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionSOUTH->setText(QCoreApplication::translate("MainWindow", "&SOUTH", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionSOUTH->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Down", nullptr));
#endif // QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionWEST->setText(QCoreApplication::translate("MainWindow", "&WEST", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionWEST->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Left", nullptr));
#endif // QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionUndo->setText(QCoreApplication::translate("MainWindow", "&Undo", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionUndo->setShortcut(QCoreApplication::translate("MainWindow", "Backspace", nullptr));
#endif // QT_CONFIG(shortcut)



	menubar_root.rootMenu->game.actionStart_Solver->setText(QCoreApplication::translate("MainWindow", "Start S&olver", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionStart_Solver->setShortcut(QCoreApplication::translate("MainWindow", "F9", nullptr));
#endif // QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionStop_Solver->setText(QCoreApplication::translate("MainWindow", "Stop Solve&r", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionStop_Solver->setShortcut(QCoreApplication::translate("MainWindow", "F10", nullptr));
#endif // QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.menuPlaySolver->setTitle(QCoreApplication::translate("MainWindow", "&Play...", nullptr));
	menubar_root.rootMenu->game.actionForward->setText(QCoreApplication::translate("MainWindow", "&Forward", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionForward->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Right", nullptr));
#endif // QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionBack->setText(QCoreApplication::translate("MainWindow", "&Back", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionBack->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Left", nullptr));
#endif // QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionSolver_Configuration->setText(QCoreApplication::translate("MainWindow", "Solver &Configuration...", nullptr));
#if QT_CONFIG(shortcut)
	menubar_root.rootMenu->game.actionSolver_Configuration->setShortcut(QCoreApplication::translate("MainWindow", "F3", nullptr));
#endif // QT_CONFIG(shortcut)



	/// DEVELOPER

	menubar_root.rootMenu->developer.menuDeveloper->setTitle(QCoreApplication::translate("MainWindow", "&Developer", nullptr));
	menubar_root.rootMenu->developer.actionHighlightGeneratedTargetCells->setText(QCoreApplication::translate("MainWindow", "&Highlight generated target cells", nullptr));
	menubar_root.rootMenu->developer.actionEnableAllMenuBarItems->setText(QCoreApplication::translate("MainWindow", "&Enable all MenuBar items", nullptr));
	menubar_root.rootMenu->developer.action22ReferenceGame->setText(QCoreApplication::translate("MainWindow", "&Start 22 Reference Game", nullptr));


	/// VIEW

	menubar_root.rootMenu->view.menuView->setTitle(QCoreApplication::translate("MainWindow", "&View", nullptr));
	menubar_root.rootMenu->view.menuPieces->setTitle(QCoreApplication::translate("MainWindow", "&Pieces...", nullptr));

	/// HELP

	menubar_root.rootMenu->help.menuHelp->setTitle(QCoreApplication::translate("MainWindow", "&Help", nullptr));
	menubar_root.rootMenu->help.actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
	menubar_root.rootMenu->help.actionLicense_Information->setText(QCoreApplication::translate("MainWindow", "License Information", nullptr));

}

void MainWindow::connectSoltsForMainMenu()
{

	// QMetaObject::connectSlotsByName(this); is done during execution of auto-created code.

	//### connect slots!manually here!I dont like connect by name.

}


MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	menubar_root(this),
	controlKeyEventAgent(this),
	current_game(),
	factory_history(),
	next_factory_1(),
	factory_select(2)
{
	ui->setupUi(this);

	// menubar:
	setMenuBar(menubar_root.menubar);
	setTextAndShortcutsForMainMenu();
	connectSoltsForMainMenu();

	// statusbar:
	statusbarItems.init(ui->statusbar);

	shapeSelectionItems.createInsideQMenu(this, menubar_root.rootMenu->view.menuPieces);

	refreshAll();

	signalMapper = new QSignalMapper(this);

	historySignalMapper = new QSignalMapper(this);

	QObject::connect(signalMapper, QSignalMapper__mappedInt__OR__mapped__PTR, this, &MainWindow::selectPieceByColor, Qt::AutoConnection);

	QObject::connect(historySignalMapper, QSignalMapper__mappedInt__OR__mapped__PTR, this, &MainWindow::startGameFromHistory, Qt::AutoConnection);

	// in-game navigation input:
	ui->graphicsView->installEventFilter(&controlKeyEventAgent);
	ui->listView->installEventFilter(&controlKeyEventAgent);
	ui->treeView->installEventFilter(&controlKeyEventAgent);

	/*    auto log_widget = new QTextEdit();
		auto logger = spdlog::qt_logger_mt("qt_logger", log_widget);
		log_widget->setMinimumSize(640, 480);
		log_widget->setWindowTitle("Debug console");
		log_widget->show();
		logger->info("QLocale: " + QLocale().name().toStdString());
		logger->info("Qt Version: " + std::string(qVersion()));
	*/

	std::random_device rd;

	generator.seed(rd());

	next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 1>>());
	next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 2>>());
	next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 3>>());
	next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 4>>());
	next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 5>>());
	next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 6>>());
	next_factory_1.emplace_back(new OriginalGameFactory<tobor::v1_1::pieces_quantity<uint8_t, 1, 7>>());


	for (auto& factory : next_factory_1) {

		std::uniform_int_distribution<uint64_t> distribution_on_uint64_board(0, factory->world_generator_group_size());
		std::uniform_int_distribution<uint64_t> distribution_on_uint64_pieces(0, factory->state_generator_group_size());

		factory->set_world_generator_counter(distribution_on_uint64_board(generator));
		factory->set_state_generator_counter(distribution_on_uint64_pieces(generator));

	}

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::startSolver()
{
	if (!current_game) return showErrorDialog("Cannot start solver with no game opened.");

	auto showMessage = [&](const std::string& m) {
		statusBar()->showMessage(m.c_str());
		repaint();
		};

	ui->statusbar->showMessage("starting solver...");
	repaint();

	current_game->start_solver(showMessage);
	refreshAll();
}

void MainWindow::stopSolver()
{
	if (!current_game) return showErrorDialog("Cannot stop solver with no game opened.");
	current_game->stop_solver();
	refreshAll();
}

void MainWindow::stopGame()
{
	statusbarItems.setSelectedPiece(Qt::darkGray);
	disconnectInputConnections();
	menubar_root.rootMenu->game.menuSelect_Piece->clear();

	if (!current_game) return showErrorActionAvailable();
	current_game.reset();
	statusBar()->showMessage("Game stopped.");
	refreshAll();
}

void MainWindow::startGame(AbstractGameFactory* factory)
{
	if (current_game) return showErrorActionAvailable();

	current_game.reset(factory->create());
	current_color_vector = tobor::v1_0::color_vector::get_standard_coloring(static_cast<uint8_t>(current_game->count_pieces())); // standard coloring without permutation

	createColorActions();

	statusBar()->showMessage("Game started.");
	refreshAll();
}

void MainWindow::startGame()
{
	if (current_game) return showErrorActionAvailable();

	std::unique_ptr<CyclicGroupGameFactory>& fac{ next_factory_1[factory_select] };

	factory_history.emplace_back(fac->clone());

	startGame(fac.get());

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

}

void MainWindow::startGameFromHistory(int index)
{
	if (current_game) return showErrorActionAvailable();

	return startGame(factory_history[index].get());
}

void MainWindow::startReferenceGame22()
{
	if (current_game) return showErrorActionAvailable();

	auto fac = SpecialCaseGameFactory();

	factory_history.emplace_back(fac.clone());

	current_game.reset(fac.create());

	current_color_vector = tobor::v1_0::color_vector::get_standard_coloring(static_cast<uint8_t>(current_game->count_pieces())); // standard coloring without permutation

	createColorActions();

	refreshAll();
}

void MainWindow::selectPieceByColorId(const std::size_t& color_id)
{
	if (!current_game) return showErrorDialog("Cannot select piece with no game opened.");

	const uint8_t ERROR_CODE{ current_game->select_piece_by_color_id(color_id) };
	if (ERROR_CODE) throw std::logic_error("Illegal color_id.");
	refreshStatusbar();
}

/**
*	@brief Moves selected piece into given direction in interactive mode.
*		However, in solver mode it moves forward on EAST, backward on WEST and does nothing otherwise.
*/
void MainWindow::movePieceInteractiveAndSolver(const tobor::v1_0::direction& direction)
{
	if (!current_game) {
		showErrorDialog("Cannot move a piece with no game opened.");
		return;
	}

	if (current_game->solver()) {
		if ((direction == tobor::v1_0::direction::EAST()) || (direction == tobor::v1_0::direction::WEST())) {
			current_game->move_by_solver(direction == tobor::v1_0::direction::EAST());
			refreshAll();
		}
		// else ignore input
		return;
	}

	current_game->move_selected(direction);
	statusBar()->showMessage(QString::fromStdString(std::to_string(current_game->selected_piece_color_id())));
	refreshAll();
}

void MainWindow::undo()
{
	if (!current_game) return showErrorDialog("Cannot undo with no game opened.");
	if (current_game->solver()) {
		current_game->stop_solver();
	}
	current_game->undo();
	refreshAll();
}

void MainWindow::selectSolution(std::size_t index)
{
	if (!current_game) {
		return showErrorDialog("Cannot select solution with no game opened.");
	}
	if (!current_game->solver()) {
		return showErrorDialog("Cannot select solution without running solver.");
	}

	current_game->select_solution(index);
}


void MainWindow::on_actionHighlightGeneratedTargetCells_triggered() {
	highlightGeneratedTargetCells();
}

void MainWindow::on_actionAbout_triggered()
{
	qDebug() << "QLocale:" << QLocale().name();


	QMessageBox msgBox;
	msgBox.setText(QString("Qt Version used: ") + qVersion());
	msgBox.exec();
}


void MainWindow::viewSvgInMainView(const QString& svg_string)
{
	QXmlStreamReader xml;
	xml.addData(svg_string);

	SvgViewToolchain new_chain;

	new_chain.q_svg_renderer = std::make_unique<QSvgRenderer>(&xml); // doc does not require argument to be valid for duration of renderer

	auto local_q_graphics_svg_item = new QGraphicsSvgItem();
	local_q_graphics_svg_item->setSharedRenderer(new_chain.q_svg_renderer.get()); // does not take ownership

	new_chain.q_graphics_scene = std::make_unique<QGraphicsScene>();
	new_chain.q_graphics_scene->addItem(local_q_graphics_svg_item); // takes ownership

	ui->graphicsView->setScene(new_chain.q_graphics_scene.get()); // does not take ownership
	ui->graphicsView->fitInView(new_chain.q_graphics_scene.get()->sceneRect(), Qt::KeepAspectRatio);
	ui->graphicsView->show();

	svgViewToolchain = std::move(new_chain); // then destroy old objects in reverse order compared to construction...
}

void MainWindow::on_actionNewGame_triggered() {
	startGame();
}

void MainWindow::on_actionStopGame_triggered() {
	stopGame();
}

void MainWindow::on_actionUndo_triggered()
{
	undo();
}

void MainWindow::on_actionNORTH_triggered()
{
	if (!current_game || current_game->solver()) return showErrorActionAvailable();
	movePieceInteractiveAndSolver(tobor::v1_0::direction::NORTH());
}


void MainWindow::on_actionEAST_triggered()
{
	if (!current_game || current_game->solver()) return showErrorActionAvailable();
	movePieceInteractiveAndSolver(tobor::v1_0::direction::EAST());
}


void MainWindow::on_actionSOUTH_triggered()
{
	if (!current_game || current_game->solver()) return showErrorActionAvailable();
	movePieceInteractiveAndSolver(tobor::v1_0::direction::SOUTH());
}


void MainWindow::on_actionWEST_triggered()
{
	if (!current_game || current_game->solver()) return showErrorActionAvailable();
	movePieceInteractiveAndSolver(tobor::v1_0::direction::WEST());
}

void MainWindow::on_actionForward_triggered()
{
	if (!current_game || !current_game->solver()) return showErrorActionAvailable();
	current_game->move_by_solver(true);
	refreshAll();
}


void MainWindow::on_actionBack_triggered()
{
	if (!current_game || !current_game->solver()) return showErrorActionAvailable();
	current_game->move_by_solver(false);
	refreshAll();
}

void MainWindow::on_actionStart_Solver_triggered()
{
	if (!current_game || current_game->solver()) showErrorActionAvailable();
	startSolver();
}

void MainWindow::on_actionStop_Solver_triggered()
{
	stopSolver();
}

void MainWindow::on_actionLicense_Information_triggered()
{
	LicenseDialog* dialog = new LicenseDialog(this);
	dialog->open();
}


void MainWindow::on_listView_doubleClicked(const QModelIndex& index)
{
	selectSolution(index.row());
	refreshAll();
}

void MainWindow::setMenuButtonEnableForNoGame()
{
	menubar_root.rootMenu->game.actionNewGame->setEnabled(true);
	menubar_root.rootMenu->game.actionStopGame->setEnabled(false);
	menubar_root.rootMenu->game.actionStart_Solver->setEnabled(false);
	menubar_root.rootMenu->game.actionStop_Solver->setEnabled(false);
	menubar_root.rootMenu->game.actionUndo->setEnabled(false);
	menubar_root.rootMenu->game.menuSelect_Piece->setEnabled(false);
	menubar_root.rootMenu->game.menuMove->setEnabled(false);
	menubar_root.rootMenu->game.menuPlaySolver->setEnabled(false);
	menubar_root.rootMenu->game.menuHistory->setEnabled(true);
}

void MainWindow::setMenuButtonEnableForInteractiveGame()
{
	menubar_root.rootMenu->game.actionNewGame->setEnabled(false);
	menubar_root.rootMenu->game.actionStopGame->setEnabled(true);
	menubar_root.rootMenu->game.actionStart_Solver->setEnabled(true);
	menubar_root.rootMenu->game.actionStop_Solver->setEnabled(false);
	menubar_root.rootMenu->game.actionUndo->setEnabled(!current_game->is_initial());
	menubar_root.rootMenu->game.menuSelect_Piece->setEnabled(true);
	menubar_root.rootMenu->game.menuMove->setEnabled(true);
	menubar_root.rootMenu->game.menuPlaySolver->setEnabled(false);
	menubar_root.rootMenu->game.menuHistory->setEnabled(false);
}

void MainWindow::setMenuButtonEnableForSolverGame()
{
	menubar_root.rootMenu->game.actionNewGame->setEnabled(false);
	menubar_root.rootMenu->game.actionStopGame->setEnabled(true);
	menubar_root.rootMenu->game.actionStart_Solver->setEnabled(false);
	menubar_root.rootMenu->game.actionStop_Solver->setEnabled(true);
	menubar_root.rootMenu->game.actionUndo->setEnabled(!current_game->is_initial());
	menubar_root.rootMenu->game.menuSelect_Piece->setEnabled(false);
	menubar_root.rootMenu->game.menuMove->setEnabled(false);
	menubar_root.rootMenu->game.menuPlaySolver->setEnabled(true);
	menubar_root.rootMenu->game.menuHistory->setEnabled(false);
}

void MainWindow::createColorActions()
{
	QMenu* sub = menubar_root.rootMenu->game.menuSelect_Piece;

	sub->clear();
	// actions are deleted,
	// according to QSignalMapper's docs, the map entries for these objects will be deleted on their destruction.


	for (std::size_t i = 0; i < current_color_vector.colors.size(); ++i) {

		auto action = sub->addAction(
			current_color_vector.colors[i].UPPERCASE_display_string_with_underscore()
		);
		// TODO need to add shortcuts (?) STRG+R ...

		inputConnections.push_back(
			QObject::connect(action, &QAction::triggered, signalMapper, qOverload<>(&QSignalMapper::map), Qt::AutoConnection)
		);
		//QObject::connect(action, &QAction::triggered, mainWindow->signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map), Qt::AutoConnection);

		signalMapper->setMapping(action, static_cast<int>(i));
	}
}

void MainWindow::refreshSVG()
{
	if (current_game) {

		tobor::v1_1::general_piece_shape_selection shape{ tobor::v1_1::general_piece_shape_selection::BALL };

		if (shapeSelectionItems.getSelectedShape() == shapeSelectionItems.duck) {
			shape = tobor::v1_1::general_piece_shape_selection::DUCK;
		}

		auto svg_as_string = current_game->svg(current_color_vector, shape);

		viewSvgInMainView(svg_as_string);
	}
	else {
		QGraphicsScene* scene = new QGraphicsScene();
		ui->graphicsView->setScene(scene);
	}
}

void MainWindow::refreshNumberOfSteps()
{
	QString number_of_steps;
	if (current_game) {
		number_of_steps = QString::number(current_game->depth());
	}
	statusbarItems.stepsValue->setText(number_of_steps);
}

void MainWindow::refreshMenuButtonEnable()
{
	if (!current_game) return setMenuButtonEnableForNoGame();

	if (current_game->solver()) return setMenuButtonEnableForSolverGame();

	return setMenuButtonEnableForInteractiveGame();
}

void MainWindow::refreshStatusbar()
{
	if (current_game) {
		auto current_color = current_color_vector.colors[current_game->selected_piece_color_id()].getQColor();
		statusbarItems.setSelectedPiece(current_color);
	}
	else {
		statusbarItems.setSelectedPiece(Qt::darkGray);
	}
	refreshNumberOfSteps();
}

void MainWindow::refreshSolutionPaths()
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

	ui->listView->setModel(model); // not needed multiple times ###
}

void MainWindow::refreshHistory()
{
	QMenu* sub = menubar_root.rootMenu->game.menuHistory;

	sub->clear();
	// actions are deleted,
	// according to QSignalMapper's docs, the map entries for these objects will be deleted on their destruction.


	for (std::size_t i = 0; i < factory_history.size(); ++i) {

		const auto i_reverse{ factory_history.size() - i - 1 };

		const auto world_counter{ factory_history[i_reverse]->get_world_generator_counter() };
		const auto state_counter{ factory_history[i_reverse]->get_state_generator_counter() };

		auto action = sub->addAction(
			QString::number(i_reverse + 1) + " :    " + QString::number(world_counter) + " : " + QString::number(state_counter)
		);

		QObject::connect(action, &QAction::triggered, historySignalMapper, qOverload<>(&QSignalMapper::map), Qt::AutoConnection);

		historySignalMapper->setMapping(action, static_cast<int>(i_reverse));
	}

}

void MainWindow::highlightGeneratedTargetCells()
{
	if (!current_game) {
		return showErrorDialog("Target cell markers not supported without running a game");
	}
	std::pair<std::string, std::size_t> svg_and_count = factory_history.back()->svg_highlighted_targets();

	viewSvgInMainView(svg_and_count.first);

	QString m{ "Number of generator target cells:   " };
	m += QString::number(svg_and_count.second);
	ui->statusbar->showMessage(m);
}

void MainWindow::refreshAll()
{
	refreshSVG();
	refreshStatusbar();
	refreshMenuButtonEnable();
	refreshSolutionPaths();
	refreshHistory();
}

void MainWindow::StatusbarItems::init(QStatusBar* statusbar) {
	stepsKey = new QLabel(statusbar); // parent takes ownership
	stepsValue = new QLabel(statusbar); // parent takes ownership

	boardIdKey = new QLabel(statusbar);
	boardIdValue = new QLabel(statusbar);

	solverKey = new QLabel(statusbar);
	solverValue = new QLabel(statusbar);

	pieceSelectedKey = new QLabel(statusbar);
	pieceSelectedValue = new QLabel(statusbar);
	pieceSelectedValue->setMinimumSize(15, 15);
	pieceSelectedValue->setMaximumSize(15, 15);

	/* label order */

	statusbar->addPermanentWidget(solverKey);
	statusbar->addPermanentWidget(solverValue);

	statusbar->addPermanentWidget(boardIdKey);
	statusbar->addPermanentWidget(boardIdValue);

	statusbar->addPermanentWidget(pieceSelectedKey);
	statusbar->addPermanentWidget(pieceSelectedValue);

	statusbar->addPermanentWidget(stepsKey); // parent is replaced?
	statusbar->addPermanentWidget(stepsValue); // parent is replaced?

	setSelectedPiece(Qt::darkGray);

	stepsKey->setText("Steps:");

	//QString number_of_steps = QString::number(0);
	//stepsValue->setText(number_of_steps);

	//stepsKey->hide();
	//stepsValue->hide();

	boardIdKey->setText("Board:");

	solverKey->setText("Solver:");

	pieceSelectedKey->setText("Piece:");
}

void MainWindow::StatusbarItems::setSelectedPiece(const QColor& c)
{
	QPixmap pm(QUADRATIC_COLOR_LABEL_SIZE, QUADRATIC_COLOR_LABEL_SIZE);

	pm.fill(c);

	QImage img = pm.toImage();
	for (int i = 0; i < QUADRATIC_COLOR_LABEL_SIZE; ++i) {
		img.setPixelColor(0, i, Qt::black);
		img.setPixelColor(QUADRATIC_COLOR_LABEL_SIZE - 1, i, Qt::black);
		img.setPixelColor(i, 0, Qt::black);
		img.setPixelColor(i, QUADRATIC_COLOR_LABEL_SIZE - 1, Qt::black);
	}

	pm = QPixmap::fromImage(img);

	pieceSelectedValue->setPixmap(pm.scaled(pieceSelectedValue->size(), Qt::KeepAspectRatio));
}

void MainWindow::selectPieceByColor(int index) {
	selectPieceByColorId(index); // where to check range correctness? SignalMapper should not fire an int greater than color vector, make some additional check here (or somewhere else?)
}

template<class QMenu_OR_QMenuBar>
inline void menu_recursion(QMenu_OR_QMenuBar* m) {
	m->setEnabled(true);
	for (QAction* item : m->actions()) {
		qDebug() << item->text();
		item->setEnabled(true);

		if (item->isSeparator()) {
		}
		else if (item->menu()) {
			QMenu* sub = item->menu();
			menu_recursion(sub);
		}
		else /* normal action */ {
		}
	}
}


void MainWindow::on_actionEnableAllMenuBarItems_triggered()
{
	menu_recursion(menubar_root.menubar);
}

void MainWindow::on_action22ReferenceGame_triggered()
{
	startReferenceGame22();
}

void MainWindow::ShapeSelectionItems::createInsideQMenu(MainWindow* mainWindow, QMenu* qMenu) {
	(void)mainWindow;

	group = new QActionGroup(qMenu);

	ball = new QAction(qMenu);
	duck = new QAction(qMenu);
	swan = new QAction(qMenu);

	ball->setObjectName("actionBall");
	duck->setObjectName("actionDuck");
	swan->setObjectName("actionSwan");
	swan->setEnabled(false);


	ball->setText(QString("&Ball"));
	duck->setText(QString("&Duck"));
	swan->setText(QString("&Swan"));

	ball->setCheckable(true);
	duck->setCheckable(true);
	swan->setCheckable(true);

	qMenu->addAction(ball);
	qMenu->addAction(duck);
	qMenu->addAction(swan);

	group->addAction(ball);
	group->addAction(duck);
	group->addAction(swan);
	//group->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive); not available on older Qt5 versions. :(
	group->setExclusive(true);
	ball->setChecked(true);

	QObject::connect(group, &QActionGroup::triggered, mainWindow, &MainWindow::refreshAll, Qt::AutoConnection);

}

QAction* MainWindow::ShapeSelectionItems::getSelectedShape() const {
	if (ball->isChecked()) {
		return ball;
	}
	if (duck->isChecked()) {
		return duck;
	}
	if (swan->isChecked()) {
		return swan;
	}
	ball->setChecked(true);
	return ball;
}
