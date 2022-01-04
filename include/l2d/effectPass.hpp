#pragma once

#include <glbinding/gl/gl.h>
#include <glm/vec2.hpp>

#include "effectProgram.hpp"
#include "uniforms.hpp"

using namespace gl;

class EffectPass
{
protected:
    bool m_ready = false;
    glm::uvec2 m_resolution;
    GLuint m_geometry;
    GLuint m_framebuffer;
    GLuint m_texture;

public:
    EffectPass(glm::uvec2 resolution, GLuint geometry);
    EffectPass() = default;
    bool ready();
    void frame();
    GLuint texture();
};
