#include "predefined.h"

#include "mainwindow.h"

#include "gui_helper.h"

#include "color_generator.h"
#include "custom_traits.h"

#include "./ui_mainwindow.h"
#include "gui_interactive_controller.h"
#include "solver_1_1.h"

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

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow),
	guiInteractiveController(this),
	controlKeyEventAgent(this)
{
	ui->setupUi(this);
	statusbarItems.init(ui->statusbar);

	shapeSelectionItems.createInsideQMenu(this, ui->menuPieces);

	refreshAll();

	signalMapper = new QSignalMapper(this);

	QObject::connect(signalMapper, QSignalMapper__mappedInt__OR__mapped__PTR, this, &MainWindow::selectPieceByColor, Qt::AutoConnection);

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
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::startSolver()
{
	auto game = guiInteractiveController.currentGame();

	if (!game) {
		return showErrorDialog("Cannot start solver with no game opened.");
	}

	auto showMessage = [&](const std::string& m) {
		statusBar()->showMessage(m.c_str());
		repaint();
		};

	game->start_solver(showMessage);

	refreshAll();
}

void MainWindow::stopSolver()
{
	auto game = guiInteractiveController.currentGame();

	if (!game) {
		return showErrorDialog("Cannot start solver with no game opened.");
	}

	game->stop_solver();
	refreshAll();
}

void MainWindow::stopGame()
{
	statusbarItems.setSelectedPiece(Qt::darkGray);
	disconnectInputConnections();
	getSelectPieceSubMenu()->clear();

	auto game = guiInteractiveController.currentGame();

	if (!game) {
		return showErrorDialog("This action should not be available.");
	}

	game.reset();

	refreshAll();
}

void MainWindow::on_actionshowSVG_triggered()
{
	const QString example_svg_string{
	  R"xxx(<?xml version="1.0" ?>
<!DOCTYPE svg  PUBLIC '-//W3C//DTD SVG 1.1//EN'  'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>
<svg enable-background="new 0 0 512 512.068" height="512.068px" id="Layer_1" version="1.1" viewBox="0 0 512 512.068" width="512px" xml:space="preserve" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="meanicons_x5F_23">
<path d="M256.34-0.068C114.936-0.068,0.345,114.628,0.345,255.939c0,141.442,114.591,256.062,255.995,256.062   c141.424,0,256.005-114.619,256.005-256.062C512.346,114.628,397.764-0.068,256.34-0.068z M273.314,355.815l-73.994,74.068   l-74.089-74.068l-85.545-85.631l74.032-74.051l85.602,85.563L398.942,82.122l74.089,74.004L273.314,355.815z"/>
</g>
<g id="Layer_1_1_"/>
</svg>
)xxx"
	};

	viewSvgInMainView(example_svg_string);
}

void MainWindow::on_actionHighlightGeneratedTargetCells_triggered() {
	guiInteractiveController.highlightGeneratedTargetCells();
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
	guiInteractiveController.startGame();
	statusBar()->showMessage("Game started.");
	//statusbarItems.stepsKey->show();
	//statusbarItems.stepsValue->show();
}

void MainWindow::on_actionStopGame_triggered() {
	stopGame();
	statusBar()->showMessage("Game stopped.");
}

void MainWindow::on_actionMoveBack_triggered()
{
	guiInteractiveController.undo();
}

void MainWindow::on_actionNORTH_triggered()
{
	statusBar()->showMessage("Went north.");
	guiInteractiveController.movePiece(tobor::v1_0::direction::NORTH());
}


void MainWindow::on_actionEAST_triggered()
{
	statusBar()->showMessage("Went east.");
	guiInteractiveController.movePiece(tobor::v1_0::direction::EAST());

}


void MainWindow::on_actionSOUTH_triggered()
{
	statusBar()->showMessage("Went south.");
	guiInteractiveController.movePiece(tobor::v1_0::direction::SOUTH());

}


void MainWindow::on_actionWEST_triggered()
{
	statusBar()->showMessage("Went west.");
	guiInteractiveController.movePiece(tobor::v1_0::direction::WEST());

}

void MainWindow::on_actionForward_triggered()
{
	on_actionEAST_triggered(); // change this
}


void MainWindow::on_actionBack_triggered()
{
	on_actionWEST_triggered(); // change this
}

void MainWindow::on_actionStart_Solver_triggered()
{
	ui->statusbar->showMessage("starting solver...");
	repaint();
	startSolver();
}

void MainWindow::on_actionStop_Solver_triggered()
{
	stopSolver();
}

void MainWindow::on_actionLicense_Information_triggered()
{
	showErrorDialog("Not yet implemented.");
}


void MainWindow::on_listView_doubleClicked(const QModelIndex& index)
{
	guiInteractiveController.selectSolution(index.row());
	refreshAll();
}

void MainWindow::setMenuButtonEnableForNoGame()
{
	ui->actionNewGame->setEnabled(true);
	ui->actionStopGame->setEnabled(false);
	ui->actionStart_Solver->setEnabled(false);
	ui->actionStop_Solver->setEnabled(false);
	ui->actionMoveBack->setEnabled(false);
	ui->menuSelect_Piece->setEnabled(false);
	ui->menuMove->setEnabled(false);
	ui->menuPlaySolver->setEnabled(false);
}

void MainWindow::setMenuButtonEnableForInteractiveGame()
{
	ui->actionNewGame->setEnabled(false);
	ui->actionStopGame->setEnabled(true);
	ui->actionStart_Solver->setEnabled(true);
	ui->actionStop_Solver->setEnabled(false);
	ui->actionMoveBack->setEnabled(!guiInteractiveController.currentGame()->is_initial());
	ui->menuSelect_Piece->setEnabled(true);
	ui->menuMove->setEnabled(true);
	ui->menuPlaySolver->setEnabled(false);
}

void MainWindow::setMenuButtonEnableForSolverGame()
{
	ui->actionNewGame->setEnabled(false);
	ui->actionStopGame->setEnabled(true);
	ui->actionStart_Solver->setEnabled(false);
	ui->actionStop_Solver->setEnabled(true);
	ui->actionMoveBack->setEnabled(!guiInteractiveController.currentGame()->is_initial());
	ui->menuSelect_Piece->setEnabled(false);
	ui->menuMove->setEnabled(false);
	ui->menuPlaySolver->setEnabled(true);
}

void MainWindow::refreshSVG()
{
	auto game = guiInteractiveController.currentGame();
	if (game) {

		tobor::v1_1::general_piece_shape_selection shape{ tobor::v1_1::general_piece_shape_selection::BALL };

		if (shapeSelectionItems.getSelectedShape() == shapeSelectionItems.duck) {
			shape = tobor::v1_1::general_piece_shape_selection::DUCK;
		}

		auto svg_as_string = game->svg(guiInteractiveController.current_color_vector, shape);

		viewSvgInMainView(svg_as_string);
	}
	else {
		QGraphicsScene* scene = new QGraphicsScene();
		ui->graphicsView->setScene(scene);
	}
}

void MainWindow::refreshNumberOfSteps()
{
	auto game = guiInteractiveController.currentGame();

	QString number_of_steps;
	if (game) {
		number_of_steps = QString::number(game->depth());
	}
	statusbarItems.stepsValue->setText(number_of_steps);
}

void MainWindow::refreshMenuButtonEnable()
{
	auto game = guiInteractiveController.currentGame();

	if (!game) return setMenuButtonEnableForNoGame();

	if (game->solver()) return setMenuButtonEnableForSolverGame();

	return setMenuButtonEnableForInteractiveGame();
}

void MainWindow::refreshStatusbar()
{
	auto game = guiInteractiveController.currentGame();

	if (game) {
		auto current_color = guiInteractiveController.current_color_vector.colors[game->selected_piece_color_id()].getQColor();
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

	auto game = guiInteractiveController.currentGame();


	if (model == nullptr) {
		model = new QStringListModel();
	}

	auto permutated_color_vector = guiInteractiveController.current_color_vector;

	QStringList qStringList;

	if (game) {
		qStringList = game->optimal_solutions_list(permutated_color_vector);
	}

	model->setStringList(qStringList);

	ui->listView->setModel(model); // not needed multiple times ###
}

void MainWindow::refreshAll()
{
	refreshSVG();
	refreshStatusbar();
	refreshMenuButtonEnable();
	refreshSolutionPaths();
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

QMenu* MainWindow::getSelectPieceSubMenu() {
	return ui->menuSelect_Piece;
}

void MainWindow::selectPieceByColor(int index) {
	guiInteractiveController.selectPieceByColorId(index); // where to check range correctness? SignalMapper should not fire an int greater than color vector, make some additional check here (or somewhere else?)
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
	menu_recursion(ui->menubar);
}

void MainWindow::on_action22ReferenceGame_triggered()
{
	guiInteractiveController.startReferenceGame22();
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
