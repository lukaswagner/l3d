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
#include "viewer.hpp"
INIT_CTX

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
        renderer.resultRanderbuffer(), renderResolution, renderResolution * 50u,
        "Render output", window);
    logger::info(CTX) << "Viewer init successful.";

    // render loop
    while (true)
    {
        glfwMakeContextCurrent(window);
        renderer.frame(std::chrono::milliseconds(0));

        viewer.frame();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    // clean up
    glfwTerminate();
    return 0;
}
