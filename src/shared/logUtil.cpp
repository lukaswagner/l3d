#include "logUtil.hpp"

#include <ostream>

#include <glbinding-aux/meta.h>

using namespace gl;

std::map<GLenum, std::function<logger::Line(const std::string& context)>>
    g_glLogMap = {
        {GL_DEBUG_SEVERITY_HIGH, logger::error},
        {GL_DEBUG_SEVERITY_MEDIUM, logger::warning},
        {GL_DEBUG_SEVERITY_LOW, logger::info},
        {GL_DEBUG_SEVERITY_NOTIFICATION, logger::debug}};

void oglLogCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam)
{
    auto line = g_glLogMap.at(severity)("GL ERROR");
    line << glbinding::aux::Meta::getString(source) << ": "
         << glbinding::aux::Meta::getString(type) << "\n>>> " << message;
}

void enableOglLogging()
{
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(oglLogCallback, 0);
}
