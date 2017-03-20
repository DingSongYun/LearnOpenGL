#include "GPUProgram.h"
#include "Utils.h"
#include "stdio.h"

GPUProgram::GPUProgram()
{
    mProgram = glCreateProgram();
}

GLuint GPUProgram::CompileShader(const char* shaderPath, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    const char* code = LoadFileContent(shaderPath);

    if (code == nullptr)
    {
        printf("cannot load shader source from file %s\n", shaderPath);
        return 0;
    }
    glShaderSource(shader, 1, &code, nullptr); // ram => vram
    glCompileShader(shader);
    GLint nResult;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &nResult);
    if (nResult == GL_FALSE)
    {
        printf("compile shader %s fail\n", shaderPath);
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        char*log = new char[logLength];
        GLsizei writed = 0;
        glGetShaderInfoLog(shader, logLength, &writed, log);
        printf("%s\n", log);
        delete log;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

void GPUProgram::AttachShader(GLenum shaderType, const char* shaderPath)
{
    GLuint shader = this->CompileShader(shaderPath, shaderType);
    if (shader != 0)
    {
        glAttachShader(mProgram, shader);
        mAttachShaders.push(shader);
    }
}

void GPUProgram::Link()
{
    glLinkProgram(mProgram);
    GLint nResult;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &nResult);
    if (nResult == GL_FALSE)
    {
        printf("create gpu program fail,link error\n");
        GLint logLength;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &logLength);
        char*log = new char[logLength];
        GLsizei writed = 0;
        glGetProgramInfoLog(mProgram, logLength, &writed, log);
        printf("%s\n", log);
        delete log;
        glDeleteProgram(mProgram);
        mProgram = 0;
    }
    while (!mAttachShaders.empty())
    {
        GLuint shader = mAttachShaders.top();
        glDetachShader(mProgram, shader);
        glDeleteShader(shader);
        mAttachShaders.pop();
    }
}

void GPUProgram::DetectAttribute(const char* attributeName)
{
    GLint loc = glGetAttribLocation(mProgram, attributeName);

    if(loc != -1)
    {
        mLocations.insert(std::pair<std::string, GLint>(attributeName, loc));
    }
}

void GPUProgram::DetectUniform(const char* attributeName)
{
    GLint loc = glGetUniformLocation(mProgram, attributeName);

    if(loc != -1)
    {
        mLocations.insert(std::pair<std::string, GLint>(attributeName, loc));
    }
}

GLint GPUProgram::getLocation(const char* name)
{
    auto iter = mLocations.find(name);
    if (iter == mLocations.end())
    {
        return -1;
    }

    return iter->second;
}

