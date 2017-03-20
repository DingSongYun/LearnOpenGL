#pragma once
#include "../LearnShader/glew.h"

char* LoadFileContent(const char*path);

unsigned char* LoadBMP(const char*path, int &width, int &height);

GLuint CreateTextureFromFile(const char* filePath);