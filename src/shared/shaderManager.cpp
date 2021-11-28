#include "shaderManager.hpp"

#include <fstream>
#include <sstream>

#include <logger.hpp>

#include "ctx.hpp"
INIT_CTX

ShaderManager::ShaderManager(std::filesystem::path base)
    : m_base(base)
    , m_path{
          {GL_VERTEX_SHADER, base / "vert"},
          {GL_FRAGMENT_SHADER, base / "frag"}}
{
}

std::vector<std::string> ShaderManager::availableShaders(GLenum type)
{
    if (m_available.find(type) != m_available.end())
    {
        return m_available.at(type);
    }

    const auto path = m_path.at(type);
    const auto ext = m_ext.at(type);
    std::vector<std::string> result;
    for (auto const& file : std::filesystem::directory_iterator{path})
    {
        const auto path = file.path();
        if (path.extension().string() == ext)
        {
            result.push_back(path.stem().string());
        }
    }

    m_available[type] = result;
    return result;
}

std::string ShaderManager::source(GLenum type, std::string name)
{
    auto map = m_source[type];
    if (map.find(name) != map.end())
    {
        return map.at(name);
    }

    const auto path = m_path.at(type);
    const auto ext = m_ext.at(type);
    try
    {
        std::ifstream file((path / name).replace_extension(ext));
        std::stringstream buffer;
        buffer << file.rdbuf();
        auto str = buffer.str();
        map[name] = str;
        return str;
    }
    catch (const std::exception& e)
    {
        logger::error(CTX) << "Error loading shader source: " << e.what();
        return "";
    }
}

void logShaderError(GLuint shader, GLenum type)
{
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> infoLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
    // -2 to avoid final lf and null byte
    auto log = std::string(infoLog.begin(), infoLog.end() - 2);
    auto typeString = type == GL_VERTEX_SHADER ? "Vertex" : "Fragment";
    logger::warning(CTX) << typeString << " shader compilation error:\n" << log;
}

GLuint ShaderManager::compile(GLenum type, std::string name)
{
    auto map = m_compiled[type];
    if (map.find(name) != map.end())
    {
        return map.at(name);
    }

    const auto sourceString = source(type, name);
    if (sourceString.length() == 0)
    {
        return 0;
    }
    const auto sourceCString = sourceString.c_str();
    auto shader = gl::glCreateShader(type);
    glShaderSource(shader, 1, &sourceCString, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        logShaderError(shader, type);
        glDeleteShader(shader);
        return 0;
    }

    map[name] = shader;
    return shader;
}

std::vector<std::string> ShaderManager::availableVertexShaders()
{
    return availableShaders(GL_VERTEX_SHADER);
}

std::string ShaderManager::vertexSource(std::string name)
{
    return source(GL_VERTEX_SHADER, name);
}

GLuint ShaderManager::compileVertex(std::string name)
{
    return compile(GL_VERTEX_SHADER, name);
}

std::vector<std::string> ShaderManager::availableFragmentShaders()
{
    return availableShaders(GL_FRAGMENT_SHADER);
}

std::string ShaderManager::fragmentSource(std::string name)
{
    return source(GL_FRAGMENT_SHADER, name);
}

GLuint ShaderManager::compileFragment(std::string name)
{
    return compile(GL_FRAGMENT_SHADER, name);
}
