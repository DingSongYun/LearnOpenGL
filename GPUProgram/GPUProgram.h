#pragma once
#include "../LearnShader/glew.h"
#include <stack>

class GPUProgram
{
public:
        GLuint mProgram;
        std::stack<GLuint> mAttachShaders;
public:
    GPUProgram();
    void AttachShader(GLenum shaderType, const char* shaderPath);
    void Link();
    GLuint CompileShader(const char* shaderPath, GLenum shaderType);
};