#include "program.hpp"

#include <glbinding/gl/gl.h>
#include <logger.hpp>

#include "ctx.hpp"
INIT_CTX

using namespace gl;

GLuint createProgram(GLuint vert, GLuint frag)
{
    logger::info(CTX) << vert << " " << frag;
    if (vert == 0 || frag == 0)
    {
        return 0;
    }

    auto program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        auto log = std::string(infoLog.begin(), infoLog.end() - 2);
        logger::warning(CTX) << "Program linking error:\n" << log;

        glDeleteProgram(program);
        return 0;
    }

    return program;
}
