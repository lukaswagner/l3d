#include "renderer.hpp"

#include <logger.hpp>
#include <macros.hpp>

#include "ctx.hpp"
INIT_CTX

using namespace gl;

float Renderer::s_vertices[] = {-1, -1, -1, 3, 3, -1};

Renderer::Renderer(glm::uvec2 resolution, std::filesystem::path shaderPath)
    : m_resolution(resolution)
    , m_shaderManager(shaderPath)
    , m_vertexSource("passthrough")
{
    // rbo & fbo
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_RGBA8, m_resolution.x, m_resolution.y);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        logger::error(CTX) << "Could not initialize framebuffer.";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // shaders
    const auto frag = m_shaderManager.availableFragmentShaders()[0];
    m_program = createProgram(frag);
    if (m_program == 0)
    {
        logger::warning(CTX) << "Could not create program.";
        return;
    }

    // geometry
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);

    auto vertexLoc = glGetAttribLocation(m_program, "a_vertex");
    glEnableVertexAttribArray(vertexLoc);
    glVertexAttribPointer(vertexLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
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

void Renderer::frame()
{
    glViewport(0, 0, m_resolution.x, m_resolution.y);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glUseProgram(m_program);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Renderer::fbo() { return m_fbo; }
GLuint Renderer::rbo() { return m_rbo; }
