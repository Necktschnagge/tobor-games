#include "predefined.h"

#include "mainwindow.h"

#include "./ui_mainwindow.h"

#include "models.h"
#include "tobor_svg.h"


#include <QXmlStreamReader>
#include <QGraphicsSvgItem>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setWindowTitle("Awkward Goslings");
}

MainWindow::~MainWindow()
{
	delete ui;
}


std::string helper_for_demo_board() {

	auto w = tobor::v1_0::default_world(16, 16);

	w.block_center_cells(2, 2);

	// add vertical walls row by row, starting left
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

	// specify initial state
	auto target_piece = tobor::v1_0::default_cell_id::create_by_coordinates(6, 9, w);
	auto green_robot = tobor::v1_0::default_cell_id::create_by_coordinates(12, 7, w);
	auto red_robot = tobor::v1_0::default_cell_id::create_by_coordinates(12, 12, w);
	auto yellow_robot = tobor::v1_0::default_cell_id::create_by_coordinates(6, 14, w);

	std::array<tobor::v1_0::default_cell_id, 3> other_robots{ green_robot, red_robot, yellow_robot };
	std::array<tobor::v1_0::default_cell_id, 1> target_robots{ target_piece };

	//auto initial_state = tobor::v1_0::positions_of_pieces<3>(target_piece, std::move(other_robots));

	// specify target field
	auto target = tobor::v1_0::default_cell_id::create_by_coordinates(9, 1, w);

	using pop_type = tobor::v1_0::positions_of_pieces<tobor::v1_0::default_pieces_quantity, tobor::v1_0::default_cell_id, false, false>;

	auto svg_string = tobor::v1_0::tobor_graphics<pop_type>::draw_tobor_world(
		w,
		pop_type(
			target_robots,
			other_robots
		),
		target,
		tobor::v1_0::tobor_graphics<pop_type>::coloring("red", "green", "blue", "yellow")
	);

	return (svg_string);
}

void MainWindow::on_actionshowSVG_triggered()
{
	if constexpr (true) {
		viewSvgInMainView(QString::fromStdString(helper_for_demo_board()));

	}
	else {


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
}


void MainWindow::on_actionAbout_triggered()
{
	QMessageBox msgBox;
	msgBox.setText(QString("Qt Version used:   ") + qVersion());
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

