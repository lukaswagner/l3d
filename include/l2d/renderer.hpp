#pragma once

#define GLFW_INCLUDE_NONE

#include <filesystem>
#include <string>

#include <GLFW/glfw3.h>
#include <glbinding/gl/gl.h>
#include <glm/vec2.hpp>

#include "shaderManager.hpp"

using namespace gl;

class Renderer
{
protected:
    static float s_vertices[];
    bool m_ready = false;
    glm::uvec2 m_resolution;
    GLuint m_fbo;
    GLuint m_rbo;
    ShaderManager m_shaderManager;
    std::string m_vertexSource;
    GLuint m_program;
    GLuint m_vbo;

    GLuint createProgram(std::string source);

public:
    Renderer(glm::uvec2 resolution, std::filesystem::path shaderPath);
    bool ready();
    void frame();
    GLuint fbo();
    GLuint rbo();
};
