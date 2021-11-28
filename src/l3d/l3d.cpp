#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <logger.hpp>

#include "ctx.hpp"
INIT_CTX

int main(int argc, char const* argv[])
{
    logger::enable();
    logger::setContextLength(15);
    logger::info(CTX) << "Initializing OpenGL context...";
    if (!glfwInit())
    {
        logger::error(CTX) << "Could not initialize OpenGL context.";
        return 1;
    }
    logger::info(CTX) << "OpenGL context init successful.";
    return 0;
}
