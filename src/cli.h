#pragma once
/*

TODO: check / implement correct handling of each cli_config member
*/

#include <CLI/CLI.hpp>

#include <iostream>
#include <string>

/**
 * @brief startup config to be parsed from command-line arguments
 */
struct cli_config {
	bool        no_gui{ false };      ///< runs in gui-less mode
	bool        log_console{ false }; ///< enables logging to console window
	bool        log_file{ false };     ///< enables logging to file
	std::string log_file_path{};      ///< path to log file where to write log or empty in case a default file location should be used
};

/**
 * @brief Parses the command-line arguments as a #cli_config
 *
 * @return the #cli_config
 */
cli_config parse_cli(int argc, char** argv);
