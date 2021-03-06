#pragma once

#include <vector>

#include <glbinding/gl/gl.h>

#include "effectProgram.hpp"

using namespace gl;

class DataSource
{
public:
    virtual void addEffect(EffectProgram program) = 0;
    virtual void update() = 0;
    virtual void apply(GLuint program) = 0;
};
