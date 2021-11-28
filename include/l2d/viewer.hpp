#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glbinding/gl/gl.h>
#include <glm/vec2.hpp>

using namespace gl;

class Viewer
{
protected:
    GLuint m_fbo;
    glm::uvec2 m_inputResolution;
    glm::uvec2 m_outputResolution;
    std::string m_title;
    GLFWwindow* m_window;

public:
    Viewer(
        GLuint rbo, glm::uvec2 inputResolution, glm::uvec2 outputResolution,
        std::string title, GLFWwindow* context);
    void frame();
};
