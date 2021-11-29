#pragma once

#define GLFW_INCLUDE_NONE

#include <chrono>
#include <filesystem>
#include <map>
#include <string>

#include <GLFW/glfw3.h>
#include <glbinding/gl/gl.h>
#include <glm/vec2.hpp>

#include "effectPass.hpp"
#include "fadePass.hpp"
#include "shaderManager.hpp"

using namespace gl;

using ProgramMap = std::map<std::string, EffectProgram>;
using ProgramIt = ProgramMap::iterator;

struct FadeUniforms
{
    GLint texture0;
    GLint texture1;
    GLint mixFactor;
};

class Renderer
{
protected:
    static float s_vertices[];
    bool m_ready = false;
    glm::uvec2 m_resolution;
    ShaderManager m_shaderManager;

    GLuint m_geometry;

    EffectPass m_effectPasses[2];
    FadePass m_fadePass;

    ProgramMap m_programs;
    ProgramIt m_currentProgram;
    ProgramIt m_nextProgram;

    std::chrono::milliseconds m_fadeDuration;
    std::chrono::milliseconds m_currentFade;

    ProgramIt nextProgram(ProgramIt start);
    ProgramIt nextValidProgram(ProgramIt start);

public:
    Renderer(glm::uvec2 resolution, std::filesystem::path shaderPath);
    bool ready();
    void frame(std::chrono::milliseconds deltaT);
    void startFade();
    GLuint framebuffer();
    GLuint renderbuffer();
};
