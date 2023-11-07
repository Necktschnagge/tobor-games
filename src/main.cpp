
#include "logger.h"



#include "mainwindow.h"

#include <QApplication>

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

//	return 0;
//	(void)argc;
//	(void)argv;

    QApplication qt_app(argc, argv);
    MainWindow main_window;
	main_window.show();
    return qt_app.exec();
}
