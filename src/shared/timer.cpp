#include "timer.hpp"

#include <thread>

#include <logger.hpp>

#include "ctx.hpp"
INIT_CTX

using Clock = std::chrono::steady_clock;
using Duration = Clock::duration;
using Timestamp = std::chrono::time_point<Clock>;
using namespace std::chrono_literals;

Elapsed::Elapsed()
    : m_last(Clock::now())
{
}

Duration Elapsed::elapsed() { return Clock::now() - m_last; }
void Elapsed::reset() { m_last = Clock::now(); }

template <typename T>
Timer<T>::Timer(Duration interval)
    : Elapsed()
    , m_interval(interval)
{
    logger::info(CTX) << "created timer: " << m_interval.count();
}

template <typename T>
Timer<T>::Timer(int fps)
    : Timer(Duration(1s) / fps)
{
}

template <typename T>
void Timer<T>::next()
{
    m_last += m_interval;
}

Interval::Interval(Duration interval)
    : Timer(interval)
{
}

Interval::Interval(int fps)
    : Timer(fps)
{
}

bool Interval::step()
{
    auto result = elapsed() > m_interval;
    if (result)
    {
        next();
    }
    return result;
}

Limiter::Limiter(Duration interval)
    : Timer(interval)
{
}

Limiter::Limiter(int fps)
    : Timer(fps)
{
}

void Limiter::step()
{
    auto delta = m_interval - elapsed();
    if (delta > 0ms)
    {
        std::this_thread::sleep_for(delta);
    }
    next();
}

Delta::Delta()
    : Elapsed()
{
}

Duration Delta::step()
{
    auto result = elapsed();
    reset();
    return result;
}
