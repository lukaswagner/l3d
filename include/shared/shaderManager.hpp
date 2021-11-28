#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <glbinding/gl/gl.h>

using namespace gl;

using FileList = std::vector<std::string>;
using SourceMap = std::map<std::string, std::string>;
using CompiledMap = std::map<std::string, GLuint>;

class ShaderManager
{
protected:
    std::map<GLenum, std::filesystem::path> m_path;
    std::map<GLenum, std::string> m_ext = {
        {GL_VERTEX_SHADER, ".vert"}, {GL_FRAGMENT_SHADER, ".frag"}};
    std::map<GLenum, FileList> m_available;
    std::map<GLenum, SourceMap> m_source = {
        {GL_VERTEX_SHADER, {}}, {GL_FRAGMENT_SHADER, {}}};
    std::map<GLenum, CompiledMap> m_compiled = {
        {GL_VERTEX_SHADER, {}}, {GL_FRAGMENT_SHADER, {}}};

    std::filesystem::path m_base;
    std::vector<std::string> availableShaders(GLenum type);
    std::string source(GLenum type, std::string name);
    GLuint compile(GLenum type, std::string name);

public:
    ShaderManager(std::filesystem::path base);
    std::vector<std::string> availableVertexShaders();
    std::string vertexSource(std::string name);
    GLuint compileVertex(std::string name);
    std::vector<std::string> availableFragmentShaders();
    std::string fragmentSource(std::string name);
    GLuint compileFragment(std::string name);
};
