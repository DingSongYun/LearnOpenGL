#pragma once
#include "../LearnShader/glew.h"
#include <stack>
#include "unordered_map"

class GPUProgram
{
public:
        GLuint mProgram; // gpu resource id
        std::stack<GLuint> mAttachShaders;
        std::unordered_map<std::string, GLint> mLocations;
public:
    GPUProgram();
    void Link();
    void AttachShader(GLenum shaderType, const char* shaderPath);
    GLuint CompileShader(const char* shaderPath, GLenum shaderType);

    void DetectAttribute(const char* attributeName);
    void DetectUniform(const char* uniformName);
    GLint getLocation(const char* uniformName);
};