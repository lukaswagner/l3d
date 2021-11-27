#pragma once

#include <filesystem>
#include <string>

#define FILENAME std::filesystem::path(__FILE__).filename().string()
#define INIT_CTX const std::string g_ctx = FILENAME;
#define CTX g_ctx + ':' + std::to_string(__LINE__)
