#pragma once

#include <filesystem>
#include <string>

#define FILENAME std::filesystem::path(__FILE__).filename().string()
#define INIT_CTX const std::string g_ctx = FILENAME;
#define STRINGIFY2(ARG) #ARG
#define STRINGIFY(ARG) STRINGIFY2(ARG)
#define CTX g_ctx + ":" STRINGIFY(__LINE__)
