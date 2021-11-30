#pragma once

#include <string>

#include <CLI/CLI.hpp>
#include <logger.hpp>

struct Options
{
    std::string shaderDir = "./shader/2d";
    logger::Level logLevel = logger::Info;
    logger::Level fileLevel = logger::Off;
    unsigned int framerate = 30;
    unsigned int fadeIntervalS = 5;
    unsigned int fadeDurationMs = 500;
    unsigned int fpsLogIntervalS = 20;
};

extern Options g_options;

void defineOptions(CLI::App& app);
