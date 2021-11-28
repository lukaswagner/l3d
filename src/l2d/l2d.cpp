#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <chrono>
#include <filesystem>
#include <glbinding-aux/debug.h>
#include <glbinding/glbinding.h>
#include <glm/vec2.hpp>
#include <iostream>
#include <logger.hpp>
#include <macros.hpp>
#include <thread>
#include <vector>

#include "ctx.hpp"
INIT_CTX
#include "glfwError.hpp"
#include "renderer.hpp"
#include "viewer.hpp"

int main(int argc, char const* argv[])
{
    logger::enable(logger::Level::Debug);
    logger::setContextLength(20);
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
    glbinding::initialize(glfwGetProcAddress);

    glbinding::aux::enableGetErrorCallback();

    auto shaderDir = std::filesystem::path(__FILE__).remove_filename() /
                     std::filesystem::path("../../shader/2d");
    shaderDir = shaderDir.lexically_normal().make_preferred();

    glm::uvec2 renderResolution{10, 10};
    Renderer renderer(renderResolution, shaderDir);
    if (!renderer.ready())
    {
        logger::error(CTX) << "Could not initialize renderer.";
        return 1;
    }
    logger::info(CTX) << "Renderer init successful.";

    Viewer viewer(
        renderer.rbo(), renderResolution, renderResolution * 50u,
        "Render output", window);
    logger::info(CTX) << "Viewer init successful.";

    while (true)
    {
        glfwMakeContextCurrent(window);
        renderer.frame();

        // DEBUG_EXPR(glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer.fbo()));
        // std::vector<float> buf(renderResolution.x * renderResolution.y * 4);
        // glReadPixels(
        //     0, 0, renderResolution.x, renderResolution.y, GL_RGBA, GL_FLOAT,
        //     buf.data());
        // DEBUG_EXPR(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
        // auto line = logger::info(CTX);
        // for (const auto pixel : buf)
        // {
        //     line << pixel << " ";
        // }
        // glFinish();
        // std::cout << std::flush;

        viewer.frame();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    glfwTerminate();
    return 0;
}
