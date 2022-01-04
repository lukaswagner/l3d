#pragma once

#include <glbinding/gl/gl.h>

#include "uniforms.hpp"

using namespace gl;

struct EffectProgram
{
    GLuint handle;
    bool valid;
    Uniforms uniforms;
};
