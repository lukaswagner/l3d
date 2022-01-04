#pragma once

#include <chrono>
#include <map>
#include <string>

#include "dataSource/base.hpp"

enum TimeType
{
    Frame,
    Elapsed,
    Now
};

using elapsed = std::function<uint64_t()>;
using handler = std::function<void()>;

class Time : public DataSource
{
protected:
    std::map<GLuint, std::vector<handler>> m_handlers;

    uint64_t m_frame;

    std::map<std::string, elapsed> m_getElapsed;
    std::chrono::steady_clock::time_point m_start;
    std::chrono::steady_clock::duration m_elapsed;

    std::chrono::system_clock::time_point m_now;

    template <typename T>
    uint64_t getElapsed();

public:
    Time();
    void addEffect(EffectProgram program) override;
    void update() override;
    void apply(GLuint program) override;
};
