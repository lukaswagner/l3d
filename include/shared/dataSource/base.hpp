#pragma once

#include <vector>

#include <glbinding/gl/gl.h>

#include "effectProgram.hpp"

using namespace gl;

class DataSource
{
public:
    virtual void addEffect(EffectProgram program){};
    virtual void update(){};
    virtual void apply(GLuint program){};
};
