#include "viewer.hpp"

#include <glbinding/gl/gl.h>
#include <iostream>
#include <logger.hpp>
#include <macros.hpp>

#include "ctx.hpp"
INIT_CTX

using namespace gl;

Viewer::Viewer(
    GLuint rbo, glm::uvec2 inputResolution, glm::uvec2 outputResolution,
    std::string title, GLFWwindow* context)
    : m_inputResolution(inputResolution)
    , m_outputResolution(outputResolution)
    , m_title(title)
{
    m_window = glfwCreateWindow(
        m_outputResolution.x, m_outputResolution.y, m_title.c_str(), nullptr,
        context);
    glfwMakeContextCurrent(m_window);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        logger::error(CTX) << "Could not initialize framebuffer.";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Viewer::frame()
{
    logger::info(CTX) << "frame";
    glfwMakeContextCurrent(m_window);
    glViewport(0, 0, m_outputResolution.x, m_outputResolution.y);
    DEBUG_EXPR(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo));
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    DEBUG_EXPR(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    glDrawBuffer(GL_BACK);

    glBlitFramebuffer(
        0, 0, m_inputResolution.x, m_inputResolution.y, 0, 0,
        m_outputResolution.x, m_outputResolution.y, GL_COLOR_BUFFER_BIT,
        GL_NEAREST);

    DEBUG_EXPR(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));

    glfwSwapBuffers(m_window);
    logger::info(CTX) << "end frame";

    // DEBUG_EXPR(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo));
    // std::vector<float> buf(m_inputResolution.x * m_inputResolution.y * 4);
    // glReadPixels(
    //     0, 0, m_inputResolution.x, m_inputResolution.y, GL_RGBA, GL_FLOAT,
    //     buf.data());
    // DEBUG_EXPR(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
    // auto line = logger::info(CTX);
    // for (const auto pixel : buf)
    // {
    //     line << pixel << " ";
    // }
    // glFinish();
    // std::cout << std::flush;
}
