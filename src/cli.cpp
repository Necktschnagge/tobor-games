#include "cli.h"

cli_config parse_cli(int argc, char** argv) {

	CLI::App app{ "Tobor CLI" };

	cli_config config;

	// --no-gui
	app.add_flag("--no-gui", config.no_gui, "Disable GUI");

	// --log-console
	app.add_flag("--log-console", config.log_console, "Enable console logging");

	// --log-file  (boolean OR path)
	//
	// Case A: --log-file        enable file logging with default path
	// Case B: --log-file path   enable file logging with custom path
	//

	app.add_option("--log-file", config.log_file_path, "Enable file logging (optional custom path)")->expected(0, 1)->each([&](const std::string&) {
		config.log_file = true;
	});

	try {
		app.parse(argc, argv);
	} catch (const CLI::ParseError& e) {
		// Convert CLI11 error into exit code
		std::exit(app.exit(e));
	}

	return config;
}
