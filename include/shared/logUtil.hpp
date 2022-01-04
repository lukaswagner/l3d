#pragma once

#include <filesystem>
#include <map>
#include <string>

#include <glbinding/gl/gl.h>
#include <logger.hpp>

using namespace gl;

#define _FILENAME std::filesystem::path(__FILE__).filename().string()
#define INIT_CTX const std::string g_ctx = _FILENAME;
#define _STRINGIFY2(ARG) #ARG
#define _STRINGIFY(ARG) _STRINGIFY2(ARG)
#define CTX g_ctx + ":" _STRINGIFY(__LINE__)

#define _FAIL1(v1) logger::error(CTX) << "Assertion " #v1 " failed"
#define ASSERT(v1) ((v1) ? true : ((_FAIL1(v1)), false))

#define _FAIL3(op, v1, v2)                                                     \
    logger::error(CTX) << "Assertion " #v1 " " #op " " #v2                     \
                          " failed with values "                               \
                       << v1 << " and " << v2
#define _ASSERT3(op, v1, v2) ((v1 op v2) ? true : ((_FAIL3(op, v1, v2)), false))

#define ASSERT_EQ(v1, v2) _ASSERT3(==, v1, v2)
#define ASSERT_LT(v1, v2) _ASSERT3(<, v1, v2)
#define ASSERT_LE(v1, v2) _ASSERT3(<=, v1, v2)
#define ASSERT_GT(v1, v2) _ASSERT3(>, v1, v2)
#define ASSERT_GE(v1, v2) _ASSERT3(>=, v1, v2)
#define ASSERT_NE(v1, v2) _ASSERT3(!=, v1, v2)

extern std::map<GLenum, std::function<logger::Line(const std::string& context)>>
    g_glLogMap;

void oglLogCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam);

void enableOglLogging();
