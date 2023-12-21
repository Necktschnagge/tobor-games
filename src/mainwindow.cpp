#include "predefined.h"

#include "mainwindow.h"

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



MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow),
	guiInteractiveController(this)
{
	ui->setupUi(this);
	statusbarItems.init(ui->statusbar);
    getColorBall();

	grabKeyboard(); // https://doc.qt.io/qt-6/qwidget.html#grabKeyboard

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
	statusbarItems.stepsKey->show();
	statusbarItems.stepsValue->show();
}

void MainWindow::on_actionStopGame_triggered() {
	guiInteractiveController.stopGame();
	statusBar()->showMessage("Game stopped.");
	statusbarItems.stepsValue->setText(QString::number(0)); // remove this line here. There should be some refresh called when displaying the new board.
	statusbarItems.stepsKey->hide();
	statusbarItems.stepsValue->hide();
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


void MainWindow::on_actionTest_ListView_triggered()
{
	static QStringListModel* model{ nullptr };

	if (model == nullptr) {
		model = new QStringListModel();
	}
	QStringList list;
	list << "a" << "b" << "c";
	list << "a" << "b" << "c";
	list << "a" << "b" << "c";
	list << "a" << "b" << "c";
	list << "a" << "b" << "c";
	list << "a" << "b" << "c";
	list << "a" << "b" << "c";
	model->setStringList(list);

	ui->listView->setModel(model);

}


void MainWindow::on_actionRED_triggered()
{
	statusBar()->showMessage("RED selected.");
	guiInteractiveController.setPieceId(0);
}


void MainWindow::on_actionYELLOW_triggered()
{
	statusBar()->showMessage("YELLOW selected.");
	guiInteractiveController.setPieceId(1);

}


void MainWindow::on_actionGREEN_triggered()
{
	statusBar()->showMessage("GREEN selected.");
	guiInteractiveController.setPieceId(2);

}


void MainWindow::on_actionBLUE_triggered()
{
	statusBar()->showMessage("BLUE selected.");
	guiInteractiveController.setPieceId(3);

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

void MainWindow::setNumberOfSteps(QString& c)
{
	statusbarItems.stepsValue->setText(c);
}

void MainWindow::StatusbarItems::init(QStatusBar* statusbar) {
	stepsKey = new QLabel(statusbar); // parent takes ownership
	stepsValue = new QLabel(statusbar); // parent takes ownership

	statusbar->addPermanentWidget(stepsKey); // parent is replaced?
	statusbar->addPermanentWidget(stepsValue); // parent is replaced?

	stepsKey->setText("Steps:");

	QString number_of_steps = QString::number(0);
	stepsValue->setText(number_of_steps);

	stepsKey->hide();
	stepsValue->hide();
}

void MainWindow::getColorBall()
{
    QString blue   = QColor( 66, 133, 244).name();
    QString green  = QColor( 52, 168,  83).name();
    QString red    = QColor(234,  67,  53).name();
    QString yellow = QColor(251, 188,   5).name();

    QString color_ball
(R"---(<svg id="emoji" viewBox="0 0 72 72" xmlns="http://www.w3.org/2000/svg">
<g id="color">
<circle cx="36" cy="36.0001" r="28" fill="#RRGGBB"/>
</g>
<g id="line">
<circle cx="36" cy="36.0001" r="28" fill="none" stroke="#000" stroke-linejoin="round" stroke-width="2"/>
</g>
</svg>)---");

    QString placeholder("#RRGGBB");
    QString blue_ball = color_ball.replace(color_ball.indexOf(placeholder), placeholder.size(), blue);
    qDebug() << blue_ball;
}
