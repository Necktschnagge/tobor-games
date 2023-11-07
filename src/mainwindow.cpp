
#include "mainwindow.h"

#include "./ui_mainwindow.h"

#include "gui_interactive_controller.h"


#include "solver.h"
#include "tobor_svg.h"


#include <QMessageBox>
#include <QDebug>
#include <QStyle>



MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow),
	guiInteractiveController(this)
{
	QWidget::grabKeyboard();
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionshowSVG_triggered()
{
	if constexpr (false) {


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

	else {

		auto tobor_world = tobor::v1_0::tobor_world(16, 16);
		tobor_world.block_center_cells(2, 2);

		auto& w{ tobor_world };

		w.west_wall_by_id(w.coordinates_to_cell_id(6, 0)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(12, 0)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(2, 1)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(10, 1)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(14, 3)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(1, 4)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(11, 4)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(13, 5)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(4, 6)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(12, 9)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(7, 10)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(14, 10)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(3, 11)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(7, 13)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(10, 13)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(1, 14)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(13, 14)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(4, 15)) = true;
		w.west_wall_by_id(w.coordinates_to_cell_id(12, 15)) = true;

		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(0, 2)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(0, 10)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(1, 4)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(1, 14)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(2, 2)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(2, 11)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(3, 7)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(6, 3)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(6, 14)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(7, 11)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(9, 2)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(9, 13)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(10, 4)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(11, 10)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(13, 6)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(13, 15)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(14, 3)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(14, 10)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(15, 7)) = true;
		w.south_wall_by_transposed_id(w.coordinates_to_transposed_cell_id(15, 12)) = true;

		std::string example_svg_string = draw_tobor_world(tobor_world);


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
	}

}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox msgBox;
	msgBox.setText(QString("Qt Version used:   ") + qVersion());
	msgBox.exec();
}

void MainWindow::on_actionNewGame_triggered() {
	guiInteractiveController.startGame();
}

void MainWindow::on_actionStopGame_triggered() {
	//guiInteractiveController.stopGame();
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
	// see: https://doc.qt.io/qt-6/qt.html#Key-enum

	switch (e->key()) {
	case Qt::Key_Up:
		qDebug() << "Key_Up";
		break;

	case Qt::Key_Down:
		qDebug() << "Key_Down";
		break;

	case Qt::Key_Left:
		qDebug() << "Key_Left";
		break;

	case Qt::Key_Right:
		qDebug() << "Key_Right";
		break;
	}
}

