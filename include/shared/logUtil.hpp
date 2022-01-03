#pragma once

#include <filesystem>
#include <string>

#include <logger.hpp>

#define FILENAME std::filesystem::path(__FILE__).filename().string()
#define INIT_CTX const std::string g_ctx = FILENAME;
#define STRINGIFY2(ARG) #ARG
#define STRINGIFY(ARG) STRINGIFY2(ARG)
#define CTX g_ctx + ":" STRINGIFY(__LINE__)

#define FAIL(op, v1, v2)                                                       \
    logger::error(CTX) << "Assertion " #v1 " " #op " " #v2                     \
                          " failed with values "                               \
                       << v1 << " and " << v2

#define SUCC(op, v1, v2)                                                       \
    logger::error(CTX) << "Assertion " #v1 " " #op " " #v2                     \
                          " succeeded with values "                            \
                       << v1 << " and " << v2

#define ASSERT(op, v1, v2) ((v1 op v2) ? true : ((FAIL(op, v1, v2)), false))

#define ASSERT_EQ(v1, v2) ASSERT(==, v1, v2)
#define ASSERT_LT(v1, v2) ASSERT(<, v1, v2)
#define ASSERT_LE(v1, v2) ASSERT(<=, v1, v2)
#define ASSERT_GT(v1, v2) ASSERT(>, v1, v2)
#define ASSERT_GE(v1, v2) ASSERT(>=, v1, v2)
#define ASSERT_NE(v1, v2) ASSERT(!=, v1, v2)
