#pragma once

#include <filesystem>
#include <string>
#include <vector>

class ShaderManager
{
protected:
    std::filesystem::path m_base;
    std::filesystem::path m_vert;
    std::filesystem::path m_frag;
    std::vector<std::string> availableShaders(
        std::filesystem::path path, std::string ext);
    std::string source(
        std::filesystem::path path, std::string ext, std::string name);

public:
    ShaderManager(std::filesystem::path base);
    std::vector<std::string> availableVertexShaders();
    std::string vertexSource(std::string name);
    std::vector<std::string> availableFragmentShaders();
    std::string fragmentSource(std::string name);
};
