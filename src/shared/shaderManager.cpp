#include "shaderManager.hpp"

#include <fstream>
#include <sstream>

#include <logger.hpp>

#include "ctx.hpp"
INIT_CTX

ShaderManager::ShaderManager(std::filesystem::path base)
    : m_base(base)
    , m_vert(base / "vert")
    , m_frag(base / "frag")
{
}

std::vector<std::string> ShaderManager::availableShaders(
    std::filesystem::path path, std::string ext)
{
    std::vector<std::string> result;
    for (auto const& file : std::filesystem::directory_iterator{path})
    {
        const auto path = file.path();
        if (path.extension().string() == ext)
        {
            result.push_back(path.stem().string());
        }
    }
    return result;
}

std::string ShaderManager::source(
    std::filesystem::path path, std::string ext, std::string name)
{
    std::ifstream file((path / name).replace_extension(ext));
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> ShaderManager::availableVertexShaders()
{
    return availableShaders(m_vert, ".vert");
}

std::string ShaderManager::vertexSource(std::string name)
{
    return source(m_vert, ".vert", name);
}

std::vector<std::string> ShaderManager::availableFragmentShaders()
{
    return availableShaders(m_frag, ".frag");
}

std::string ShaderManager::fragmentSource(std::string name)
{
    return source(m_frag, ".frag", name);
}
