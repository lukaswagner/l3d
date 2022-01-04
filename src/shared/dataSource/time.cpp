#include "dataSource/time.hpp"

#include <algorithm>
#include <cctype>
#include <string>

#include "logUtil.hpp"
#include "uniforms.hpp"
INIT_CTX

std::string toLower(std::string s)
{
    std::transform(
        s.begin(), s.end(), s.begin(),
        [](unsigned char c)
        {
            return std::tolower(c);
        });
    return s;
}

template <typename T>
uint64_t Time::getElapsed()
{
    return std::chrono::duration_cast<T>(m_elapsed).count();
}

Time::Time()
{
    auto nano = std::bind(&Time::getElapsed<std::chrono::nanoseconds>, this);
    m_getElapsed.emplace("nanoseconds", nano);
    m_getElapsed.emplace("nano", nano);
    m_getElapsed.emplace("ns", nano);

    auto micro = std::bind(&Time::getElapsed<std::chrono::microseconds>, this);
    m_getElapsed.emplace("microseconds", micro);
    m_getElapsed.emplace("micro", micro);
    m_getElapsed.emplace("us", micro);

    auto milli = std::bind(&Time::getElapsed<std::chrono::milliseconds>, this);
    m_getElapsed.emplace("milliseconds", milli);
    m_getElapsed.emplace("milli", milli);
    m_getElapsed.emplace("ms", milli);

    auto seconds = std::bind(&Time::getElapsed<std::chrono::seconds>, this);
    m_getElapsed.emplace("seconds", seconds);
    m_getElapsed.emplace("s", seconds);

    auto minutes = std::bind(&Time::getElapsed<std::chrono::minutes>, this);
    m_getElapsed.emplace("minutes", minutes);
    m_getElapsed.emplace("m", minutes);

    auto hours = std::bind(&Time::getElapsed<std::chrono::hours>, this);
    m_getElapsed.emplace("hours", hours);
    m_getElapsed.emplace("h", hours);

    m_start = std::chrono::steady_clock::now();
}

void Time::addEffect(EffectProgram program)
{
    logger::info(CTX) << "Searching for TIME uniforms in program "
                      << program.handle;
    std::vector<handler> handlers;

    auto frame = program.uniforms.matchComment("TIME.FRAME");
    auto elapsed = program.uniforms.matchComment("TIME.ELAPSED\\s(\\w+)");
    auto nowMatches = program.uniforms.matchComment("TIME.NOW\\s(\\w)");

    for (auto m : frame)
    {
        auto uniform = program.uniforms.uniform(m.name);
        auto location = uniform.location;
        auto type = uniform.type;
        logger::info(CTX) << "TIME.FRAME at location " << location << ", type "
                          << glbinding::aux::Meta::getString(type);
        if (!ASSERT(type == GL_INT || type == GL_UNSIGNED_INT)) continue;
        if (type == GL_INT)
        {
            handlers.push_back(
                [location, this]()
                {
                    glUniform1i(location, this->m_frame);
                });
        }
        else
        {
            handlers.push_back(
                [location, this]()
                {
                    glUniform1ui(location, this->m_frame);
                });
        }
    }

    for (auto m : elapsed)
    {
        auto location = program.uniforms.uniform(m.name).location;
        logger::info(CTX) << "TIME.ELAPSED at location " << location;
        std::string duration = "millis";
        if (m.match.size() > 1) duration = toLower(m.match[1].str());

        auto it = m_getElapsed.find(duration);
        if (it == m_getElapsed.end())
        {
            logger::warning(CTX) << "Unknown duration " << duration;
            continue;
        };

        auto getter = it->second;
        handlers.push_back(
            [location, getter]()
            {
                glUniform1i(location, getter());
            });
    }

    // TODO: TIME.NOW

    m_handlers.emplace(program.handle, handlers);
}

void Time::update()
{
    m_frame++;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = now - m_start;

    m_now = std::chrono::system_clock::now();
}

void Time::apply(GLuint program)
{
    auto handlers = m_handlers.at(program);
    for (auto handler : handlers)
        handler();
}
