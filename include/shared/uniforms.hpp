#pragma once

#include <optional>
#include <regex>
#include <string>
#include <vector>

#include <glbinding/gl/gl.h>

using namespace gl;

struct Uniform
{
    std::string name;
    std::string comment;
    GLenum typ;
    bool isArray;
    GLint size;
    GLuint location;
};

class Uniforms
{
protected:
    std::vector<Uniform> m_uniforms;

public:
    Uniforms(){};
    Uniforms(GLuint program, std::string source);
    std::optional<Uniform> uniform(std::string name);
    std::optional<std::string> matchComment(
        std::regex regex, std::smatch& match = std::smatch());
    std::optional<std::string> matchComment(
        std::string regex, std::smatch& match = std::smatch());
};
