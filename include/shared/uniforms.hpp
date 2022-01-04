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

struct Match
{
    std::string name;
    std::smatch match;
};

class Uniforms
{
protected:
    std::vector<Uniform> m_uniforms;

public:
    Uniforms(){};
    Uniforms(GLuint program, std::string source);
    Uniform uniform(std::string name);
    std::vector<Match> matchComment(std::regex regex);
    std::vector<Match> matchComment(std::string regex);
};
