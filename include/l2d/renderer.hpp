#pragma once

#define GLFW_INCLUDE_NONE

#include <chrono>
#include <filesystem>
#include <map>
#include <string>

#include <GLFW/glfw3.h>
#include <glbinding/gl/gl.h>
#include <glm/vec2.hpp>

#include "shaderManager.hpp"

using namespace gl;

struct Program
{
    GLuint handle;
    bool valid;
};

using ProgramMap = std::map<std::string, Program>;
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

    GLuint m_renderTextures[2];
    GLuint m_resultRenderbuffer;
    GLuint m_framebuffers[3];

    GLuint m_vertexBuffer;

    std::string m_vertexSource;
    ProgramMap m_programs;
    ProgramIt m_currentProgram;
    ProgramIt m_nextProgram;

    GLuint m_fadeProgram;
    FadeUniforms m_fadeUniforms;
    std::chrono::milliseconds m_fadeDuration;
    std::chrono::milliseconds m_currentFade;

    GLuint createProgram(std::string source);
    ProgramIt nextProgram(ProgramIt start);
    ProgramIt nextValidProgram(ProgramIt start);

public:
    Renderer(glm::uvec2 resolution, std::filesystem::path shaderPath);
    bool ready();
    void frame(std::chrono::milliseconds deltaT);
    void startFade();
    GLuint resultFramebuffer();
    GLuint resultRanderbuffer();
};
