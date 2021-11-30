#include "options.hpp"

#include <tuple>
#include <vector>

Options g_options;

void defineOptions(CLI::App& app)
{
    app.allow_windows_style_options();
    app.option_defaults()->always_capture_default();

    app.add_option(
           "--shaderDir,-d", g_options.shaderDir,
           "Directory containing effect shaders.")
        ->check(CLI::ExistingDirectory);

    std::vector<std::pair<std::string, logger::Level>> level{
        {"Off", logger::Off},         {"Error", logger::Error},
        {"Warning", logger::Warning}, {"Info", logger::Info},
        {"Debug", logger::Debug},     {"Verbose", logger::Verbose}};

    app.add_option(
           "--logLevel,--ll,-l", g_options.logLevel,
           "Minimum priority for logging to console.")
        ->transform(CLI::CheckedTransformer(level, CLI::ignore_case));
    app.add_option(
           "--fileLevel,--fl", g_options.fileLevel,
           "Minimum priority for logging to file.")
        ->transform(CLI::CheckedTransformer(level, CLI::ignore_case));

    app.add_option(
        "--framerate,--fps", g_options.framerate, "Target framerate.");

    app.add_option(
        "--fadeInterval,--fi", g_options.fadeIntervalS,
        "Fade to next shader every x seconds.");

    app.add_option(
        "--fadeDuration,--fd", g_options.fadeDurationMs,
        "Duration of fade to next shader in milliseconds.");

    app.add_option(
        "--fpsLogInterval", g_options.fpsLogIntervalS,
        "Log average FPS to console every x seconds.");
}
