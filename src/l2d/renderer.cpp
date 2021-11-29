#include "renderer.hpp"

#include <algorithm>
#include <regex>

#include <logger.hpp>
#include <macros.hpp>

#include "ctx.hpp"
#include "program.hpp"
INIT_CTX

using namespace gl;
using namespace std::chrono_literals;

float Renderer::s_vertices[] = {-1, -1, -1, 3, 3, -1};

ProgramIt Renderer::nextProgram(ProgramIt start)
{
    // logger::info(CTX) << "nextProgram start " << start->first;
    auto next = std::next(start);
    // logger::info(CTX) << "nextProgram next " << next->first;
    if (next == m_programs.end())
    {
        next = m_programs.begin();
    }
    // logger::info(CTX) << "nextProgram result " << next->first;
    return next;
}

ProgramIt Renderer::nextValidProgram(ProgramIt start)
{
    // logger::info(CTX) << "nextValidProgram start " << start->first;
    auto next = start;
    do
    {
        next = nextProgram(start);
        // logger::info(CTX) << "nextValidProgram next " << next->first;
    } while (!next->second.valid && next != start);
    // logger::info(CTX) << "nextValidProgram result " << next->first;
    return next;
}

Renderer::Renderer(glm::uvec2 resolution, std::filesystem::path shaderPath)
    : m_resolution(resolution)
    , m_shaderManager(shaderPath)
    , m_fadeDuration(1000ms)
{
    // geometry
    glGenBuffers(1, &m_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, m_geometry);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);

    // passes
    m_effectPasses[0] = EffectPass(resolution, m_geometry);
    m_effectPasses[1] = EffectPass(resolution, m_geometry);
    m_fadePass = FadePass(
        resolution, m_geometry, m_effectPasses[0].texture(),
        m_effectPasses[1].texture(), m_shaderManager);

    // shaders
    const auto shaders = m_shaderManager.availableFragmentShaders();
    std::regex regex("^effects[\\/\\\\]?");
    for (const auto& shader : shaders)
    {
        if (!std::regex_search(shader, regex))
        {
            continue;
        }

        auto program = createProgram(m_shaderManager, "passthrough", shader);
        auto vertexLoc = glGetAttribLocation(program, "a_vertex");
        glEnableVertexAttribArray(vertexLoc);
        glVertexAttribPointer(vertexLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        m_programs[shader] = {program, program != 0};
    }

    auto numValid = std::count_if(
        m_programs.begin(), m_programs.end(),
        [](auto p) { return p.second.valid; });
    if (numValid == 0)
    {
        logger::error(CTX) << "No valid shader program.";
        return;
    }
    logger::info(CTX) << "Valid effects: " << numValid << "/"
                      << m_programs.size();
    for (auto it = m_programs.begin(); it != m_programs.end(); it++)
    {
        logger::info(CTX) << "- " << it->first;
    }

    m_currentProgram = nextValidProgram(std::prev(m_programs.end()));
    m_nextProgram = nextValidProgram(m_currentProgram);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_ready = true;
}

bool Renderer::ready() { return m_ready; }

void Renderer::frame(std::chrono::milliseconds deltaT)
{
    bool fading = m_currentFade > 0ms;
    if (fading)
    {
        m_currentFade -= deltaT;
        if (m_currentFade <= 0ms)
        {
            logger::info(CTX) << "fade complete";
            m_currentFade = 0ms;
            m_currentProgram = m_nextProgram;
            m_nextProgram = nextValidProgram(m_currentProgram);
            fading = false;
            logger::info(CTX) << "current: " << m_currentProgram->first;
            logger::info(CTX) << "upcoming: " << m_nextProgram->first;
        }
    }

    glViewport(0, 0, m_resolution.x, m_resolution.y);
    m_effectPasses[0].frame(m_currentProgram->second);

    if (fading)
    {
        m_effectPasses[1].frame(m_nextProgram->second);
    }

    float mixFactor = fading ? (1.0f - (float)m_currentFade.count() /
                                           (float)m_fadeDuration.count())
                             : 0.0f;
    m_fadePass.frame(mixFactor);
}

void Renderer::startFade()
{
    logger::info(CTX) << "starting fade (" << m_fadeDuration.count() << "ms)";
    m_currentFade = m_fadeDuration;
}
GLuint Renderer::framebuffer() { return m_fadePass.framebuffer(); }
GLuint Renderer::renderbuffer() { return m_fadePass.renderbuffer(); }
