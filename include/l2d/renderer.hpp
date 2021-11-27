#pragma once

#include "shaderManager.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glbinding/gl/gl.h>
#include <glm/vec2.hpp>
#include <optional>
#include <string>

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
};
