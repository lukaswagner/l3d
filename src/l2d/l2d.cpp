#define GLFW_INCLUDE_NONE

#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>
#include <vector>

#include <CLI/CLI.hpp>
#include <GLFW/glfw3.h>
#include <glbinding-aux/debug.h>
#include <glbinding/glbinding.h>
#include <glm/vec2.hpp>
#include <logger.hpp>
#include <macros.hpp>

#include "glfwError.hpp"
#include "logUtil.hpp"
#include "options.hpp"
#include "renderer.hpp"
#include "timer.hpp"
#include "viewer.hpp"
INIT_CTX

using namespace std::chrono_literals;

int main(int argc, char const* argv[])
{
    // get configuration
    CLI::App app{"2D GLSL shader renderer", "l2d"};
    defineOptions(app);
    CLI11_PARSE(app, argc, argv);

    // init logger
    logger::enable(g_options.logLevel, g_options.logLevel);
    logger::setContextLength(20);

    logger::info(CTX) << g_options.shaderDir;

    // init glfw
    logger::info(CTX) << "Initializing OpenGL context...";
    if (!glfwInit())
    {
        logger::error(CTX) << "Could not initialize OpenGL context.";
        return 1;
    }
    logger::info(CTX) << "OpenGL context init successful.";
    glfwSetErrorCallback(logGlfwError);

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    auto window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
    glfwDefaultWindowHints();
    glfwMakeContextCurrent(window);

    // init glbinding
    glbinding::initialize(glfwGetProcAddress);
    glbinding::aux::enableGetErrorCallback();

    // init shader manager
    auto shaderDir = std::filesystem::path(g_options.shaderDir);
    shaderDir = shaderDir.lexically_normal().make_preferred();

    // init renderer
    glm::uvec2 renderResolution{10, 10};
    Renderer renderer(
        renderResolution, shaderDir,
        std::chrono::milliseconds(g_options.fadeDurationMs));
    if (!renderer.ready())
    {
        logger::error(CTX) << "Could not initialize renderer.";
        return 1;
    }
    logger::info(CTX) << "Renderer init successful.";

    // init viewer
    Viewer viewer(
        renderer.renderbuffer(), renderResolution, renderResolution * 50u,
        "Render output", window);
    logger::info(CTX) << "Viewer init successful.";

    renderer.startFade();

    unsigned int fpsCount = 0;
    Interval fpsLogInterval(std::chrono::seconds(g_options.fpsLogIntervalS));
    auto fpsLogIntervalInv = 1.0f / g_options.fpsLogIntervalS;
    Interval fadeInterval(std::chrono::seconds(g_options.fadeIntervalS));
    Limiter fpsLimiter(g_options.framerate);
    Delta lastFrameDelta;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        auto delta = lastFrameDelta.step();
        if (fadeInterval.step())
        {
            renderer.startFade();
        }

        glfwMakeContextCurrent(window);
        renderer.frame(
            std::chrono::duration_cast<std::chrono::milliseconds>(delta));

        viewer.frame();

        fpsCount++;
        if (fpsLogInterval.step())
        {
            logger::info(CTX) << "FPS: " << fpsCount * fpsLogIntervalInv;
            fpsCount = 0;
        }

        glfwPollEvents();
        fpsLimiter.step();
    }

    // clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
