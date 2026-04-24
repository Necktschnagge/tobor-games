#include "predefined.h"

#include "logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {
	inline constexpr std::string_view STANDARD_LOGGER_NAME = "main";

	std::string make_timestamp_based_file_name_for_log() {
		auto now         = std::chrono::system_clock::now();
		auto now_seconds = std::chrono::floor<std::chrono::seconds>(now);
		return std::format("{:%Y-%m-%d_%H-%M-%S}.log", now_seconds);
	}

	void setup_default_logger(const std::vector<spdlog::sink_ptr>& sinks) {
		auto logger = std::make_shared<spdlog::logger>(STANDARD_LOGGER_NAME.data(), sinks.cbegin(), sinks.cend());

		spdlog::set_default_logger(logger);
		spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
		spdlog::set_level(spdlog::level::debug);
		spdlog::flush_on(spdlog::level::info);
	}

	std::vector<spdlog::sink_ptr> create_sinks(bool use_console_sink, bool use_file_sink) {
		std::vector<spdlog::sink_ptr> sinks;

		if (use_console_sink) { sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>()); }
		if (use_file_sink) { sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(make_timestamp_based_file_name_for_log(), true)); }

		return sinks;
	}

} // namespace

void init_logger(bool use_console_sink, bool use_file_sink) {
	auto sinks = create_sinks(use_console_sink, use_file_sink);
	setup_default_logger(sinks);
}
