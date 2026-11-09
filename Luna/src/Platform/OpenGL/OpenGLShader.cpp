#include <lunapch.h>
#include "OpenGLShader.h"

#include "Luna/Core/Log.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>

#include <glad/glad.h>

namespace Luna {

    static GLenum ShaderTypeFromString(const std::string& type)
    {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;

        LUNA_CORE_ASSERT(false, "Unknown Shader Type!");
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string& pathToFile)
    {
        LUNA_PROFILE_FUNCTION();

        std::string source = ReadFile(pathToFile);
        auto shaderSources = PreProcess(source);
        Compile(shaderSources);

        // Extract name from file path
        auto lastSlash = pathToFile.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot = pathToFile.rfind('.');

        auto count = lastDot == std::string::npos ? pathToFile.size() - lastSlash : lastDot - lastSlash;
        m_Name = pathToFile.substr(lastSlash, count);
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
        : m_Name(name)
    {
        LUNA_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(sources);
    }

    OpenGLShader::~OpenGLShader()
    {
        glDeleteProgram(m_RendererID);
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
        GLuint program = glCreateProgram();
        LUNA_CORE_ASSERT(shaderSources.size() <= 2, "Only supports 2 shader per shader instance as of now");
        std::array<GLenum, 2> glShaderID;
        int glShaderIDIndex = 0;

        for (auto& kv : shaderSources)
        {
            GLenum type = kv.first;
            const std::string& source = kv.second;

            GLuint shader = glCreateShader(type);

            const GLchar *sourceCstr = source.c_str();
            glShaderSource(shader, 1, &sourceCstr, 0);

            // Compile the vertex shader
            glCompileShader(shader);

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if(isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
                
                glDeleteShader(shader);

                LUNA_CORE_CRITICAL("{0}", infoLog.data());
                LUNA_CORE_ASSERT(false, "Failed to compile shader");
                break;
            }
            glAttachShader(program, shader);
            glShaderID[glShaderIDIndex++] = shader;
        }
        glLinkProgram(program);

        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
            
            glDeleteProgram(program);
            
            for (auto id : glShaderID)
            {
                glDeleteShader(id);
            }

            LUNA_CORE_CRITICAL("{0}", infoLog.data());
            LUNA_CORE_ASSERT(false, "Failed to link program");
            return;
        }

        for (auto id : glShaderID)
        {
            glDetachShader(program, id);
            glDeleteShader(id);
        }

        m_RendererID = program;
    }

    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
    {
        LUNA_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> shaderSources;

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);
        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos);
            LUNA_CORE_ASSERT(eol != std::string::npos, "Syntax Error");
            size_t begin = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            LUNA_CORE_ASSERT(ShaderTypeFromString(type), "Invalid Shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            shaderSources[ShaderTypeFromString(type)] = source.substr(
                    nextLinePos,
                    pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos)
            );
        }

        return shaderSources;
    }

    std::string OpenGLShader::ReadFile(const std::string& filePath)
    {
        std::string result;
        std::ifstream in(filePath, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            result.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&result[0], result.size());
            in.close();
        }
        else
        {
            LUNA_CORE_WARN("Failed to open file at '{0}'", filePath);
        }
        return result;
    }

    void OpenGLShader::Bind() const
    {
        LUNA_PROFILE_FUNCTION();

        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const
    {
        LUNA_PROFILE_FUNCTION();

        glUseProgram(0);
    }

    void OpenGLShader::SetFloat4(const glm::vec4& value, const std::string& name) 
    {
        LUNA_PROFILE_FUNCTION();

        UploadUniformFloat4(value, name);
    }

    void OpenGLShader::SetFloat3(const glm::vec3& value, const std::string& name)
    { 
        LUNA_PROFILE_FUNCTION();

        UploadUniformFloat3(value, name);
    }

    void OpenGLShader::SetMat4(const glm::mat4& matrix, const std::string& name)
    {
        LUNA_PROFILE_FUNCTION();

        UploadUniformMat4(matrix, name);
    }

    void OpenGLShader::SetInt(const int value, const std::string& name)
    {
        LUNA_PROFILE_FUNCTION();

        UploadUniformInt(value, name);
    }

    void OpenGLShader::SetFloat(const float value, const std::string& name)
    {
        LUNA_PROFILE_FUNCTION();

        UploadUniformFloat(value, name);
    }

    void OpenGLShader::SetIntArray(int* values, uint32_t count, const std::string& name)
    {
        LUNA_PROFILE_FUNCTION();

        UploadUniformIntArray(values, count, name);
    }

    void OpenGLShader::UploadUniformIntArray(int* values, uint32_t count, const std::string& name)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1iv(location, count, values);
    }

    void OpenGLShader::UploadUniformMat4(const glm::mat4& matrix, const std::string& name)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformMat3(const glm::mat3& matrix, const std::string& name)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformFloat4(const glm::vec4& value, const std::string& name)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::UploadUniformFloat3(const glm::vec3& value, const std::string& name)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLShader::UploadUniformFloat2(const glm::vec2& value, const std::string& name)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLShader::UploadUniformFloat(const float& value, const std::string& name)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void OpenGLShader::UploadUniformBool(const bool& value, const std::string& name)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void OpenGLShader::UploadUniformInt(const int& value, const std::string& name)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }
}
