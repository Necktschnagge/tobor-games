#include "predefined.h"

#include "mainwindow.h"
#include "debug_utils.h"

#include <QApplication>

#include <clocale>

#include <iostream>

// TERMINOLOGY

/*
[GAME] BOARD            :       the entire world
CELL                    :       atomic unit of a BOARD
TARGET CELL             :       the CELL which is marked as goal
TARGET PIECE            :       PIECE which must be moved to the TARGET CELL
PIECE                   :       one robot / piece


*/


int main(int argc, char* argv[])
{
#ifdef _WIN32
//#pragma warning(push)
//#define _CRT_SECURE_NO_WARNINGS
	if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
		//freopen_s("CONOUT$", "w", stdout);
		//freopen_s("CONOUT$", "w", stderr);
		//freopen_s("CONIN$", "r", stdin);
	}
//#pragma warning(pop)
#endif
	std::ios_base::sync_with_stdio();

	std::cout << "test";

	init_logger();
	standard_logger()->info("test");

	QApplication qt_app(argc, argv);
	std::setlocale(LC_NUMERIC, "C");
	MainWindow main_window;
	main_window.show();

#ifdef DEBUG_EVENT_LOGGER_ENABLE

	DebugEventLogger del(&main_window);
	qt_app.installEventFilter(&del);

#endif // DEBUG_EVENT_LOGGER_ENABLE

	return qt_app.exec();
}
