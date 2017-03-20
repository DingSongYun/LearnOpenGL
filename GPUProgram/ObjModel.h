#pragma once
#include "../LearnShader/glew.h"

struct VertexData
{
    float position[3];
    float texcoord[2];
    float normal[3];
};
class ObjModel
{
public:
    GLuint mVBO, mIBO;
    unsigned int mIndexCount;
public:
    void Init(const char* modelFilePath);
    void BindModel(GLint posLoc, GLint texcoordLoc, GLint normalLoc);
    void Draw();
};