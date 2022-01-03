#include "fadePass.hpp"

#include <logger.hpp>

#include "data.hpp"
#include "logUtil.hpp"
#include "program.hpp"
INIT_CTX

FadePass::FadePass(
    glm::uvec2 resolution, GLuint geometry, GLuint texture0, GLuint texture1,
    Shader& shader)
    : m_resolution(resolution)
    , m_geometry(geometry)
    , m_texture0(texture0)
    , m_texture1(texture1)
{
    glGenRenderbuffers(1, &m_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_RGBA8, m_resolution.x, m_resolution.y);

    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderbuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        logger::error(CTX) << "Could not initialize framebuffer.";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    shader.addVirtual("fade", fade_frag);
    auto program = createProgram(
        shader.compileVirtual("passthrough", GL_VERTEX_SHADER),
        shader.compileVirtual("fade", GL_FRAGMENT_SHADER));
    if (program == 0)
    {
        logger::error(CTX) << "Fade program invalid.";
        return;
    }
    auto vertexLoc = glGetAttribLocation(program, "a_vertex");
    glEnableVertexAttribArray(vertexLoc);
    glVertexAttribPointer(vertexLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    m_program = {
        program, program != 0, glGetUniformLocation(program, "u_texture0"),
        glGetUniformLocation(program, "u_texture1"),
        glGetUniformLocation(program, "u_mixFactor")};

    glUseProgram(m_program.handle);
    glUniform1i(m_program.u_texture0, 0);
    glUniform1i(m_program.u_texture1, 1);
    glUseProgram(0);
}

bool FadePass::ready() { return m_ready; }

void FadePass::frame(float mixFactor)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glUseProgram(m_program.handle);
    glUniform1f(m_program.u_mixFactor, mixFactor);
    glBindBuffer(GL_ARRAY_BUFFER, m_geometry);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture1);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FadePass::framebuffer() { return m_framebuffer; }
GLuint FadePass::renderbuffer() { return m_renderbuffer; }
