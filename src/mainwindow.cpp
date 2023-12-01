#include "predefined.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}



void MainWindow::on_actionshowSVG_triggered()
{
	for (int i = 0; i < 300; ++i) {

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

		// https://stackoverflow.com/questions/36026593/how-to-add-children-to-qgraphicsitem
		//auto graphicsView = new QGraphicsView();

		q_svg_renderer = std::make_unique<QSvgRenderer>(&xml); // is this ok? when this function runs out of scope, xml is deleted, but does q_svg_renderer still use the xml?
		//q_svg_renderer = std::unique_ptr<QSvgRenderer>(new QSvgRenderer(&xml));

		/*
		q_graphics_svg_item = std::make_unique<QGraphicsSvgItem>(); // depends on q_svg_renderer -> destroy order
		q_graphics_svg_item->setSharedRenderer(q_svg_renderer.get()); // does not take ownership
		*/
		auto local_q_graphics_svg_item = new QGraphicsSvgItem(); // depends on q_svg_renderer -> destroy order
		local_q_graphics_svg_item->setSharedRenderer(q_svg_renderer.get()); // does not take ownership


		q_graphics_scene = std::make_unique<QGraphicsScene>();
		q_graphics_scene->addItem(local_q_graphics_svg_item); // takes ownership

		
		// todo: whenever setting a new Scene, do not reuse the exactly same unique_ptrs..
		// first build up the new bunch of objects,
		// then setScene
		// then destroy old objects in reverse order compared to construction...


		ui->graphicsView->setScene(q_graphics_scene.get());
		ui->graphicsView->fitInView(q_graphics_scene.get()->sceneRect(), Qt::KeepAspectRatio);
		ui->graphicsView->show();

		/*QSvgRenderer* svgRenderer = new QSvgRenderer(&xml);
		QGraphicsSvgItem *item = new QGraphicsSvgItem();
		QGraphicsScene *scene = new QGraphicsScene();

		item->setSharedRenderer(svgRenderer);
		scene->addItem(item);
		ui->graphicsView->setScene(scene);
		ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
		ui->graphicsView->show();*/

		//auto foo = ui->centralwidget->children().size();
		//this->ui->graphicsView.
	}
}


void MainWindow::on_actionAbout_triggered()
{
	QMessageBox msgBox;
	msgBox.setText(QString("Qt Version used:   ") + qVersion());
	msgBox.exec();
}

