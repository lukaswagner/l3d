#pragma once

#include <logger.hpp>

void logGlfwError(int error, const char *description)
{
    logger::error("GLFW") << "Error " << error << ": " << description;
}
