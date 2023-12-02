#include "predefined.h"
#include "mainwindow.h"

#include "./ui_mainwindow.h"

#include "gui_interactive_controller.h"


#include "solver.h"
#include "tobor_svg.h"


#include <QStringListModel>
#include <QMessageBox>
#include <QDebug>
#include <QStyle>



MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow),
	guiInteractiveController(this)
{
	ui->setupUi(this);
	this->setWindowTitle("Awkward Goslings");
    grabKeyboard(); // https://doc.qt.io/qt-6/qwidget.html#grabKeyboard

    // releaseKeyboard();  when entering main menu
    // again call grabKeyboard() when exiting main menu (by triggering event or exiting without clicking any menu button)

    // can we check this via some FocusEvent? Just check when the focus is changed?
    // https://stackoverflow.com/questions/321656/get-a-notification-event-signal-when-a-qt-widget-gets-focus
    // https://doc.qt.io/qt-6/qfocusevent.html#details

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionshowSVG_triggered()
{
	if constexpr (true) {


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

		QXmlStreamReader xml;
		xml.addData(example_svg_string);

		QSvgRenderer* svgRenderer = new QSvgRenderer(&xml);
		QGraphicsSvgItem* item = new QGraphicsSvgItem();
		QGraphicsScene* scene = new QGraphicsScene();

		item->setSharedRenderer(svgRenderer);
		scene->addItem(item);
		ui->graphicsView->setScene(scene);
		ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
		ui->graphicsView->show();

		//auto foo = ui->centralwidget->children().size();
		//this->ui->graphicsView.
	}

}

void MainWindow::on_actionAbout_triggered()
{
	qDebug() << "QLocale: " << QLocale().name();
	QMessageBox msgBox;
	msgBox.setText(QString("Qt Version used:   ") + qVersion());
	msgBox.exec();
}

void MainWindow::on_actionNewGame_triggered() {
	guiInteractiveController.startGame();
    statusBar()->showMessage("Game started.");
}

void MainWindow::on_actionStopGame_triggered() {
	guiInteractiveController.stopGame();
    statusBar()->showMessage("Game stopped.");
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
    static QStringListModel *model{nullptr};

    if (model == nullptr){
        model     = new QStringListModel();
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

