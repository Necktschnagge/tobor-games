#include "predefined.h"

#include "cli.h"
#include "debug_utils.h"
#include "logger.h"
#include "mainwindow.h"

#include <QApplication>

#include <clocale>

#ifdef _WIN32
	#include <windows.h>
#endif

// TERMINOLOGY
/*
BOARD                   :       the entire game board normally consisting of M x N cells.
CELL                    :       atomic unit of a BOARD. It can be occupied by a piece or empty. It has borders to neighbour cells with each having a wall or not.
TARGET CELL             :       the CELL which is marked as goal
TARGET PIECE            :       PIECE which must be moved to the TARGET CELL
PIECE                   :       one pice of the game which is to be moved from one cell to another.
*/

namespace {

	inline void ensure_console() {
#ifdef _WIN32

		// If the process already has a console (launched from PowerShell/CMD)
		if (GetConsoleWindow() != nullptr) return;

		// Otherwise allocate a new one
		AllocConsole();

		// Redirect stdout, stderr, stdin to the new console
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		freopen_s(&fp, "CONIN$", "r", stdin);

		// Optional: make std::cout sync with C I/O
		std::ios::sync_with_stdio();

#else

		// Don't do anything for Linux / macOS

#endif
	}

	void interpret_cli_config(const cli_config& config) {
		if (config.log_console) { ensure_console(); }

		init_logger(config.log_console, config.log_file); // TODO pass custom log file path here.
	}

	int run_qt_app() {
		// for some reason of destruction order, he logger must not be owned outside MainWindow:
		auto ui_logger = spdlog::default_logger()->clone("ui");
		spdlog::register_logger(ui_logger);

		// Now start Qt without arguments
		int          qt_argc = 0;
		char**       qt_argv = nullptr;
		QApplication qt_app(qt_argc, qt_argv);

		std::setlocale(LC_NUMERIC, "C");

		MainWindow main_window;
		main_window.show();

#ifdef DEBUG_EVENT_LOGGER_ENABLE

		DebugEventLogger del(&main_window);
		qt_app.installEventFilter(&del);

#endif // DEBUG_EVENT_LOGGER_ENABLE

		auto result = qt_app.exec();
		return result;
	}

} // namespace

int main(int argc, char* argv[]) {

	// const
	cli_config config = parse_cli(argc, argv);

	// DEBUG ->
	config.log_console = true;
	// <- DEBUG

	interpret_cli_config(config);

	run_qt_app();
}
