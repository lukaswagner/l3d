#include "shader.hpp"

#include <fstream>
#include <sstream>

#include <glbinding/gl/gl.h>
#include <logger.hpp>

#include "logUtil.hpp"
INIT_CTX

using namespace gl;

Shader::Shader(std::filesystem::path path)
    : m_path(path)
{
}

std::string Shader::source(std::string name)
{
    if (m_sources.find(name) != m_sources.end())
    {
        return m_sources.at(name);
    }

    logger::info(CTX) << "Adding shader source " << name;

    try
    {
        std::ifstream file(m_path / name);
        std::stringstream buffer;
        buffer << file.rdbuf();
        auto str = buffer.str();
        m_sources[name] = str;
        return str;
    }
    catch (const std::exception& e)
    {
        logger::error(CTX) << "Error loading shader source: " << e.what();
        return "";
    }
}

void logShaderError(GLuint shader, std::string name, GLenum type)
{
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> infoLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
    // -2 to avoid final lf and null byte
    auto log = std::string(infoLog.begin(), infoLog.end() - 2);
    auto typeString = type == GL_VERTEX_SHADER ? "Vertex" : "Fragment";
    logger::warning(CTX) << typeString << " shader " << name
                         << " compilation error:\n"
                         << log;
}

GLuint Shader::compile(std::string name, std::string source, GLenum type)
{
    logger::info(CTX) << "Compiling shader " << name;

    const auto str = source.c_str();
    auto shader = gl::glCreateShader(type);
    glShaderSource(shader, 1, &str, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        logShaderError(shader, name, type);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

FileList& Shader::shaders()
{
    if (!m_files.empty())
    {
        return m_files;
    }

    std::vector<std::string> result;
    for (auto const& file :
         std::filesystem::recursive_directory_iterator{m_path})
    {
        result.push_back(file.path().lexically_relative(m_path).string());
    }

    m_files = result;
    return m_files;
}

GLuint Shader::compile(std::string name, GLenum type)
{
    if (m_compiled.find(name) != m_compiled.end())
    {
        return m_compiled.at(name);
    }

    auto shader = compile(name, source(name), type);
    m_compiled[name] = shader;
    return shader;
}

void Shader::addVirtual(std::string name, std::string source)
{
    logger::info(CTX) << "Adding virtual shader source " << name;

    if (m_virtualSources.find(name) != m_virtualSources.end())
    {
        logger::warning(CTX)
            << "Source for shader " << name << " already exists. Overriding.";
    }

    m_virtualSources[name] = source;
}

GLuint Shader::compileVirtual(std::string name, GLenum type)
{
    if (m_virtualCompiled.find(name) != m_virtualCompiled.end())
    {
        logger::info(CTX) << "Cached " << name << " - "
                          << m_virtualCompiled.at(name);
        return m_virtualCompiled.at(name);
    }

    if (m_virtualSources.find(name) == m_virtualSources.end())
    {
        logger::error(CTX) << "Missing source for shader " << name;
        return 0;
    }

    auto shader = compile(name, m_virtualSources[name], type);
    m_virtualCompiled[name] = shader;
    return shader;
}
