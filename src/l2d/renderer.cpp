#include "renderer.hpp"

#include <algorithm>
#include <regex>

#include <logger.hpp>
#include <macros.hpp>

#include "data.hpp"
#include "dataSource/time.hpp"
#include "logUtil.hpp"
#include "program.hpp"
#include "uniforms.hpp"
INIT_CTX

using namespace gl;
using namespace std::chrono_literals;

float Renderer::s_vertices[] = {-1, -1, -1, 3, 3, -1};

ProgramIt Renderer::nextProgram(ProgramIt start)
{
    auto next = std::next(start);
    if (next == m_programs.end())
    {
        next = m_programs.begin();
    }
    return next;
}

ProgramIt Renderer::nextValidProgram(ProgramIt start)
{
    auto next = start;
    do
    {
        next = nextProgram(start);
    } while (!next->second.valid && next != start);
    return next;
}

Renderer::Renderer(
    glm::uvec2 resolution, std::filesystem::path shaderPath,
    std::chrono::milliseconds fadeDuration)
    : m_resolution(resolution)
    , m_shader(shaderPath)
    , m_fadeDuration(fadeDuration)
{
    // geometry
    glGenBuffers(1, &m_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, m_geometry);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);

    // shared shader
    m_shader.addVirtual("passthrough", passthrough_vert);

    // passes
    m_effectPasses[0] = EffectPass(resolution, m_geometry);
    m_effectPasses[1] = EffectPass(resolution, m_geometry);
    m_fadePass = FadePass(
        resolution, m_geometry, m_effectPasses[0].texture(),
        m_effectPasses[1].texture(), m_shader);

    // shaders
    const auto shaders = m_shader.shaders();
    for (const auto& shader : shaders)
    {
        auto program = createProgram(
            m_shader.compileVirtual("passthrough", GL_VERTEX_SHADER),
            m_shader.compile(shader));
        auto vertexLoc = glGetAttribLocation(program, "a_vertex");
        glEnableVertexAttribArray(vertexLoc);
        glVertexAttribPointer(vertexLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        Uniforms uniforms(program, m_shader.source(shader));
        auto res = uniforms.matchComment("RESOLUTION");
        if (!res.empty())
            logger::info(CTX) << "Resolution uniform location: "
                              << uniforms.uniform(res[0].name).location;

        m_programs[shader] = {program, program != 0, uniforms};
    }

    auto numValid = std::count_if(
        m_programs.begin(), m_programs.end(),
        [](auto p)
        {
            return p.second.valid;
        });
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

    // data sources

    m_dataSources.emplace_back(std::make_unique<Time>());

    for (auto&& p : m_programs)
    {
        for (auto&& s : m_dataSources)
        {
            s->addEffect(p.second);
        }
    }

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

    for (auto&& s : m_dataSources)
    {
        s->update();
    }

    glViewport(0, 0, m_resolution.x, m_resolution.y);
    auto cur = m_currentProgram->second.handle;
    glUseProgram(cur);
    for (auto&& s : m_dataSources)
    {
        s->apply(cur);
    }
    m_effectPasses[0].frame();

    if (fading)
    {
        auto next = m_nextProgram->second.handle;
        glUseProgram(next);
        for (auto&& s : m_dataSources)
        {
            s->apply(next);
        }
        m_effectPasses[1].frame();
    }
    glUseProgram(0);

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
