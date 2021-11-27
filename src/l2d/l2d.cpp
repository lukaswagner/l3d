#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glbinding/glbinding.h>
#include <logger.hpp>

#include "ctx.hpp"
INIT_CTX
#include "glfwError.hpp"
#include "renderer.hpp"

int main(int argc, char const* argv[])
{
    logger::enable();
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

    auto shaderDir = std::filesystem::path(__FILE__).remove_filename() /
                     std::filesystem::path("../../shader/2d");
    shaderDir = shaderDir.lexically_normal().make_preferred();
    Renderer renderer({10, 10}, shaderDir);
    if (!renderer.ready())
    {
        logger::error(CTX) << "Could not initialize renderer.";
        return 1;
    }
    logger::info(CTX) << "Renderer init successful.";

    renderer.frame();

    glfwTerminate();
    return 0;
}
