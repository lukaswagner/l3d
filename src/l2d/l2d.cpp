#define GLFW_INCLUDE_NONE

#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>
#include <vector>

#include <GLFW/glfw3.h>
#include <glbinding-aux/debug.h>
#include <glbinding/glbinding.h>
#include <glm/vec2.hpp>
#include <logger.hpp>
#include <macros.hpp>

#include "ctx.hpp"
#include "glfwError.hpp"
#include "renderer.hpp"
#include "timer.hpp"
#include "viewer.hpp"
INIT_CTX

using namespace std::chrono_literals;

int main(int argc, char const* argv[])
{
    // init logger
    logger::enable(logger::Level::Debug);
    logger::setContextLength(20);

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
    auto shaderDir = std::filesystem::path(__FILE__).remove_filename() /
                     std::filesystem::path("../../shader/2d");
    shaderDir = shaderDir.lexically_normal().make_preferred();

    // init renderer
    glm::uvec2 renderResolution{10, 10};
    Renderer renderer(renderResolution, shaderDir);
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
    auto fpsLogTime = 5s;
    auto fpsLogTimeInv = 1.0f / fpsLogTime.count();
    Interval fpsLogInterval(fpsLogTime);
    Interval fadeInterval(5s);
    Limiter fpsLimiter(30);
    Delta lastFrameDelta;

    // render loop
    while (true)
    {
        auto delta = lastFrameDelta.step();
        // logger::info(CTX) << "delta " << delta.count();
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
            logger::info(CTX) << "FPS: " << fpsCount * fpsLogTimeInv;
            fpsCount = 0;
        }

        fpsLimiter.step();
    }

    // clean up
    glfwTerminate();
    return 0;
}
