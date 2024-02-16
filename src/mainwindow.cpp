#include "predefined.h"

#include "mainwindow.h"

#include "gui_helper.h"

#include "color_generator.h"

#include "./ui_mainwindow.h"
#include "gui_interactive_controller.h"
#include "solver.h"
#include "tobor_svg.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/qt_sinks.h"

#include <QStringListModel>
#include <QMessageBox>
#include <QDebug>
#include <QStyle>
#include <QXmlStreamReader>
#include <QGraphicsSvgItem>
#include <QMessageBox>

#include "custom_traits.h"

// please add a enable all actions in menu to developer menu!

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow),
	guiInteractiveController(this)
{
	ui->setupUi(this);
	statusbarItems.init(ui->statusbar);

	guiInteractiveController.refreshAll();

	grabKeyboard(); // https://doc.qt.io/qt-6/qwidget.html#grabKeyboard

	signalMapper = new QSignalMapper(this);

	QObject::connect(signalMapper, QSignalMapper__mappedInt__OR__mapped__PTR, this, &MainWindow::selectPieceByColor, Qt::AutoConnection);

	//ui->menubar->installEventFilter(this); // this -> bool eventFilter(QObject* object, QEvent* event)

	// releaseKeyboard();  when entering main menu
	// again call grabKeyboard() when exiting main menu (by triggering event or exiting without clicking any menu button)

	// can we check this via some FocusEvent? Just check when the focus is changed?
	// https://stackoverflow.com/questions/321656/get-a-notification-event-signal-when-a-qt-widget-gets-focus
	// https://doc.qt.io/qt-6/qfocusevent.html#details

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
	guiInteractiveController.stopGame();
	statusBar()->showMessage("Game stopped.");
	//statusbarItems.stepsKey->hide();
	//statusbarItems.stepsValue->hide();
}

void MainWindow::on_actionMoveBack_triggered()
{
	guiInteractiveController.undo();
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
	// see: https://doc.qt.io/qt-6/qt.html#Key-enum

	switch (e->key()) {

	case Qt::Key_Alt:
		releaseKeyboard();
		break;

	case Qt::Key_Up:
		on_actionNORTH_triggered();
		break;

	case Qt::Key_Down:
		on_actionSOUTH_triggered();
		break;

	case Qt::Key_Left:
		on_actionWEST_triggered();
		break;

	case Qt::Key_Right:
		on_actionEAST_triggered();
		break;
	}
	qDebug() << "catch keyboard";
}

void MainWindow::getTypes(QObject* object, bool in) {
	/*
	if (dynamic_cast<QMenuBar*>(object) != nullptr) {
		qWarning("QMenuBar");
	}
	if (dynamic_cast<QMenu*>(object) != nullptr) {
		qWarning("QMenu");
	}
	if (dynamic_cast<QAction*>(object) != nullptr) {
		qWarning("QAction");
	}
	*/

	try {
		auto x = object->metaObject()->className();
		qDebug() << x;
		if (in)
			ui->statusbar->showMessage(x);
	}
	catch (...) {
		//QString x = QString(typeid(object).name());
		QString x = QString::number(typeid(object).hash_code());
		qDebug() << x;
		if (in)
			ui->statusbar->showMessage(x);
	}

}

bool MainWindow::eventFilter(QObject* object, QEvent* e)
{
	static constexpr bool SKIP{ true };

	if constexpr (SKIP) {
		return false;
	}
	else {


		QString event_name = typeid(*e).name();



		if (
			dynamic_cast<QGraphicsView*>(object) != nullptr ||
			dynamic_cast<QGraphicsScene*>(object) != nullptr ||
			dynamic_cast<QListView*>(object) != nullptr ||
			dynamic_cast<QTreeView*>(object) != nullptr ||
			false
			) {
			if (e->type() == QEvent::FocusIn) {
				//MainWindow::setWindowTitle("++++++++++++++++");
				statusbarItems.setKciColor(Qt::green);
				qDebug() << "green";
			}
		}

		if (
			dynamic_cast<QMenuBar*>(object) != nullptr ||
			dynamic_cast<QMenu*>(object) != nullptr ||
			//dynamic_cast<QMenuBar*>(object) != nullptr ||
			false
			) {
			if (e->type() == QEvent::FocusIn || e->type() == QEvent::MouseButtonPress) {
				//MainWindow::setWindowTitle("----------------");
				statusbarItems.setKciColor(Qt::red);
				qDebug() << "red";
			}
		}

		//if (e->type() != QEvent::FocusIn && e->type() != QEvent::FocusOut) {
		//	return false;
		//}

		try {
			QString x = object->metaObject()->className();
			qDebug() << event_name << " " << x;
			//if (in) ui->statusbar->showMessage(x);
		}
		catch (...) {
			//QString x = QString(typeid(object).name());
			QString x = QString::number(typeid(*object).hash_code());
			qDebug() << event_name << " " << x;
			//if (in) ui->statusbar->showMessage(x);
		}

		/*

		if (e->type() == QEvent::FocusOut)
		{


			qWarning("Focus Out");
			//qWarning(object->objectName().toLatin1().data());
			getTypes(object);

		}
		if (e->type() == QEvent::FocusIn)
		{
			qWarning("Focus In");
			//qWarning(object->objectName().toLatin1().data());
			getTypes(object, true);

		}
		*/
		return false;
	}
}


void MainWindow::on_actionTest_ListView_triggered()
{
	guiInteractiveController.startReferenceGame22();

}


void MainWindow::on_actionRED_triggered()
{
	statusBar()->showMessage("RED selected.");
	guiInteractiveController.setPieceId(0);
}


void MainWindow::on_actionYELLOW_triggered()
{
	statusBar()->showMessage("YELLOW selected.");
	guiInteractiveController.setPieceId(3);
}


void MainWindow::on_actionGREEN_triggered()
{
	statusBar()->showMessage("GREEN selected.");
	guiInteractiveController.setPieceId(1);
}


void MainWindow::on_actionBLUE_triggered()
{
	statusBar()->showMessage("BLUE selected.");
	guiInteractiveController.setPieceId(2);
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
	//update(); //repaint();
	//ui->statusbar->update();
	repaint();
	guiInteractiveController.startSolver();
}

void MainWindow::on_actionStop_Solver_triggered()
{
	guiInteractiveController.stopSolver();
}

void MainWindow::on_actionLicense_Information_triggered()
{
	showErrorDialog("Not yet implemented.");
}


void MainWindow::on_listView_doubleClicked(const QModelIndex& index)
{
	//QString m{"double-clicked ListView on" };
	//m+=QString::number(index.row());
	//ui->statusbar->showMessage(m);

	guiInteractiveController.selectSolution(index.row());
	guiInteractiveController.refreshAll();
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

	colorSquare = new QGraphicsView(statusbar);
	colorSquare->setMinimumSize(15, 15);
	colorSquare->setMaximumSize(15, 15);

	keyboardCaptureIcon = new QGraphicsView(statusbar);
	keyboardCaptureIcon->setMinimumSize(15, 15);
	keyboardCaptureIcon->setMaximumSize(15, 15);

	kci = new QLabel(statusbar);
	kci->setMinimumSize(15, 15);
	kci->setMaximumSize(15, 15);




	/* label order */

	statusbar->addPermanentWidget(solverKey);
	statusbar->addPermanentWidget(solverValue);

	statusbar->addPermanentWidget(boardIdKey);
	statusbar->addPermanentWidget(boardIdValue);

	statusbar->addPermanentWidget(pieceSelectedKey);
	statusbar->addPermanentWidget(pieceSelectedValue);
	statusbar->addPermanentWidget(colorSquare);

	statusbar->addPermanentWidget(stepsKey); // parent is replaced?
	statusbar->addPermanentWidget(stepsValue); // parent is replaced?

	statusbar->addPermanentWidget(keyboardCaptureIcon);
	statusbar->addPermanentWidget(kci);


	setKciColor(Qt::darkYellow);

	stepsKey->setText("Steps:");

	//QString number_of_steps = QString::number(0);
	//stepsValue->setText(number_of_steps);

	//stepsKey->hide();
	//stepsValue->hide();

	boardIdKey->setText("Board:");

	solverKey->setText("Solver:");

	pieceSelectedKey->setText("Piece:");
}

void MainWindow::StatusbarItems::setKciColor(const QColor& c)
{
	constexpr int SIZE{ 15 }; // match with size of label.

	QPixmap pm(SIZE, SIZE);

	pm.fill(c);

	QImage img = pm.toImage();
	for (int i = 0; i < SIZE; ++i) {
		img.setPixelColor(0, i, Qt::black);
		img.setPixelColor(SIZE - 1, i, Qt::black);
		img.setPixelColor(i, 0, Qt::black);
		img.setPixelColor(i, SIZE - 1, Qt::black);
	}

	pm = QPixmap::fromImage(img);

	//keyboardCaptureIcon->
	kci->setPixmap(pm.scaled(kci->size(), Qt::KeepAspectRatio));
}

QMenu* MainWindow::getSelectPieceSubMenu() {

	return ui->menuSelect_Piece;

}
void MainWindow::selectPieceByColor(int index) {
	qDebug() << "selectPieceByColor  " << index;
	guiInteractiveController.setPieceId(index); // where to check range correctness?

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
