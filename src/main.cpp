#include "predefined.h"
#include "logger.h"
#include "solver.h"
#include "tobor_svg.h"


//#include "mainwindow.h"

//#include <QApplication>

// TERMINOLOGY


/*
[GAME] BOARD            :       the entire world
CELL                    :       atomic unit of a BOARD
TARGET CELL             :       the CELL which is marked as goal
TARGET PIECE            :       PIECE which must be moved to the TARGET CELL
PIECE                   :       one robot / piece 
>>>>>>> feature-tobor-solver-engine-single-thread


*/


int main(int argc, char *argv[])
{


	init_logger();	

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

	draw_tobor_world(tobor_world);
	return 0;
	(void)argc;
	(void)argv;
	/*
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
	*/
}
