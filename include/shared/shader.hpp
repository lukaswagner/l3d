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

class Shader
{
protected:
    std::filesystem::path m_path;

    FileList m_files;
    SourceMap m_sources;
    CompiledMap m_compiled;

    SourceMap m_virtualSources;
    CompiledMap m_virtualCompiled;

    GLuint compile(std::string name, std::string source, GLenum type);

public:
    Shader(std::filesystem::path path);

    FileList& shaders();
    std::string source(std::string name);
    GLuint compile(std::string name, GLenum type = GL_FRAGMENT_SHADER);

    void addVirtual(std::string name, std::string source);
    GLuint compileVirtual(std::string name, GLenum type);
};
