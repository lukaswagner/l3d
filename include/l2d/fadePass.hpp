#pragma once

#include <glbinding/gl/gl.h>
#include <glm/vec2.hpp>

#include "shader.hpp"

using namespace gl;

struct FadeProgram
{
    GLuint handle;
    bool valid;
    GLint u_texture0;
    GLint u_texture1;
    GLint u_mixFactor;
};

class FadePass
{
protected:
    bool m_ready = false;
    glm::uvec2 m_resolution;
    GLuint m_geometry;
    GLuint m_texture0;
    GLuint m_texture1;
    GLuint m_framebuffer;
    GLuint m_renderbuffer;
    FadeProgram m_program;

public:
    FadePass(
        glm::uvec2 resolution, GLuint geometry, GLuint texture0,
        GLuint texture1, Shader& shader);
    FadePass() = default;
    bool ready();
    void frame(float mixFactor);
    GLuint framebuffer();
    GLuint renderbuffer();
};
