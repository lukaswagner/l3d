#pragma once

#include <glbinding/gl/gl.h>

#include "shaderManager.hpp"

using namespace gl;

GLuint createProgram(
    ShaderManager& shaders, std::string vert, std::string frag);
