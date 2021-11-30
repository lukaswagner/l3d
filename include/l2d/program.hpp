#pragma once

#include <glbinding/gl/gl.h>

#include "shader.hpp"

using namespace gl;

GLuint createProgram(GLuint vert, GLuint frag);
