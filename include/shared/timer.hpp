#pragma once

#include <chrono>

using Clock = std::chrono::steady_clock;
using Duration = Clock::duration;
using Timestamp = std::chrono::time_point<Clock>;

class Elapsed
{
protected:
    Timestamp m_last;

    Elapsed();
    Duration elapsed();
    void reset();
};

template <typename T>
class Timer : public Elapsed
{
protected:
    Duration m_interval;

    void next();

public:
    Timer(Duration interval);
    Timer(int fps);
    virtual T step() = 0;
};

class Interval : public Timer<bool>
{
public:
    Interval(Duration interval);
    Interval(int fps);
    bool step() override;
};

class Limiter : public Timer<void>
{
public:
    Limiter(Duration interval);
    Limiter(int fps);
    void step() override;
};

class Delta : public Elapsed
{
public:
    Delta();
    Duration step();
};
