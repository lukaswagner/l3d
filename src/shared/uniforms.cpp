#include "uniforms.hpp"

#include <algorithm>

#include <glbinding-aux/meta.h>

#include "logUtil.hpp"
INIT_CTX

std::regex uniformArrayRegex("(.*)\\[.*\\]");

std::string uniformCommentRegexStart =
    // comment between // (with optional space) and newline
    // uniform def starts with optional layout spec, keyword and type
    "\\/\\/\\s?(.*)\n"
    "(?:.*uniform.*";
std::string uniformCommentRegexEnd =
    // ends with optional brackets and semicolon
    "(?:\\[.\\d*\\])?;)";

Uniforms::Uniforms(GLuint program, std::string source)
{
    std::vector<GLuint> shaders(2);
    int numShaders;
    glGetAttachedShaders(program, 2, &numShaders, shaders.data());

    if (!ASSERT_EQ(numShaders, 2)) return;

    int numUniforms;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

    logger::info(CTX) << "Program " << program << " has " << numUniforms
                      << " active uniform(s)";

    if (numUniforms == 0) return;

    GLsizei bufSize = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufSize);

    std::vector<char> nameBuf(bufSize);
    GLsizei nameLength;

    for (auto i = 0; i < numUniforms; ++i)
    {
        GLenum type;
        GLint size;
        glGetActiveUniform(
            program, i, bufSize, &nameLength, &size, &type, nameBuf.data());

        std::string name(nameBuf.data());

        GLuint location = glGetUniformLocation(program, name.c_str());

        std::smatch match;
        bool isArray = false;
        if (std::regex_match(name, match, uniformArrayRegex))
        {
            isArray = true;
            name = match[1];
        }

        std::regex commentRegex(
            uniformCommentRegexStart + name + uniformCommentRegexEnd);
        std::string comment = "";
        if (std::regex_search(source, match, commentRegex))
        {
            comment = match[1];
        }

        auto line = logger::info(CTX);
        line << "(location = " << location << ") "
             << glbinding::aux::Meta::getString(type) << " " << name;

        if (isArray) line << "[" << size << "]";
        if (comment.size() > 0) line << " (comment: \"" << comment << "\")";

        m_uniforms.push_back({name, comment, type, isArray, size, location});
    }
}

std::optional<Uniform> Uniforms::uniform(std::string name)
{
    std::optional<Uniform> result;
    auto findResult = std::find_if(
        m_uniforms.begin(), m_uniforms.end(),
        [&](Uniform u) { return u.name == name; });
    if (findResult != m_uniforms.end()) result = *findResult;
    return result;
}

std::optional<std::string> Uniforms::matchComment(
    std::regex regex, std::smatch& match)
{
    std::optional<std::string> result;
    auto findResult = std::find_if(
        m_uniforms.begin(), m_uniforms.end(),
        [&](Uniform u) { return std::regex_search(u.comment, match, regex); });
    if (findResult != m_uniforms.end()) result = findResult->name;
    return result;
}

std::optional<std::string> Uniforms::matchComment(
    std::string regex, std::smatch& match)
{
    return matchComment(std::regex(regex), match);
}
