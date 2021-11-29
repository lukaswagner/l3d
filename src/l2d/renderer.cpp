#include "renderer.hpp"

#include <algorithm>
#include <regex>

#include <logger.hpp>
#include <macros.hpp>

#include "ctx.hpp"
INIT_CTX

using namespace gl;
using namespace std::chrono_literals;

float Renderer::s_vertices[] = {-1, -1, -1, 3, 3, -1};

ProgramIt Renderer::nextProgram(ProgramIt start)
{
    auto next = start++;
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

Renderer::Renderer(glm::uvec2 resolution, std::filesystem::path shaderPath)
    : m_resolution(resolution)
    , m_shaderManager(shaderPath)
    , m_vertexSource("passthrough")
{
    // render storage
    glGenTextures(2, m_renderTextures);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_renderTextures[0]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_resolution.x, m_resolution.y);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_renderTextures[1]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_resolution.x, m_resolution.y);

    glGenRenderbuffers(1, &m_resultRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_resultRenderbuffer);
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_RGBA8, m_resolution.x, m_resolution.y);

    // framebuffers
    glGenFramebuffers(3, m_framebuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffers[0]);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        m_renderTextures[0], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        logger::error(CTX) << "Could not initialize framebuffer.";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffers[1]);
    glActiveTexture(GL_TEXTURE0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        m_renderTextures[1], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        logger::error(CTX) << "Could not initialize framebuffer.";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffers[2]);
    glActiveTexture(GL_TEXTURE1);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
        m_resultRenderbuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        logger::error(CTX) << "Could not initialize framebuffer.";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    // geometry
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);

    // shaders
    const auto shaders = m_shaderManager.availableFragmentShaders();
    std::regex regex("^effects[\\/\\\\]?");
    for (const auto& shader : shaders)
    {
        if (!std::regex_search(shader, regex))
        {
            continue;
        }

        auto program = createProgram(shader);
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

    m_currentProgram = nextValidProgram(m_programs.end()--);
    m_nextProgram = nextValidProgram(m_currentProgram);

    m_fadeProgram = createProgram("fade");
    if (m_fadeProgram == 0)
    {
        logger::error(CTX) << "Fade program invalid.";
        return;
    }
    auto fVertexLoc = glGetAttribLocation(m_fadeProgram, "a_vertex");
    glEnableVertexAttribArray(fVertexLoc);
    glVertexAttribPointer(fVertexLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    m_fadeUniforms = {
        glGetUniformLocation(m_fadeProgram, "u_texture0"),
        glGetUniformLocation(m_fadeProgram, "u_texture1"),
        glGetUniformLocation(m_fadeProgram, "u_mixFactor")};

    glUseProgram(m_fadeProgram);
    glUniform1i(m_fadeUniforms.texture0, 0);
    glUniform1i(m_fadeUniforms.texture1, 1);
    glUseProgram(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_ready = true;
}

GLuint Renderer::createProgram(std::string fragmentName)
{
    auto vert = m_shaderManager.compileVertex(m_vertexSource);
    auto frag = m_shaderManager.compileFragment(fragmentName);

    if (vert == 0 || frag == 0)
    {
        return 0;
    }

    auto program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        auto log = std::string(infoLog.begin(), infoLog.end() - 2);
        logger::warning(CTX) << "Program linking error:\n" << log;

        glDeleteProgram(program);
        return 0;
    }

    return program;
}

bool Renderer::ready() { return m_ready; }

void Renderer::frame(std::chrono::milliseconds deltaT)
{
    if (m_currentFade > 0ms)
    {
        m_currentFade -= deltaT;
        if (m_currentFade <= 0ms)
        {
            m_currentFade = 0ms;
            m_currentProgram = m_nextProgram;
            m_nextProgram = nextValidProgram(m_currentProgram);
        }
    }

    glViewport(0, 0, m_resolution.x, m_resolution.y);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffers[0]);
    glUseProgram(m_currentProgram->second.handle);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    if (m_currentFade > 0ms)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffers[1]);
        glUseProgram(m_nextProgram->second.handle);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffers[2]);
    glUseProgram(m_fadeProgram);
    glUniform1f(m_fadeUniforms.mixFactor, 0.5f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_renderTextures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_renderTextures[1]);

    // this line throws an error
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint Renderer::resultFramebuffer() { return m_framebuffers[2]; }
GLuint Renderer::resultRanderbuffer() { return m_resultRenderbuffer; }
