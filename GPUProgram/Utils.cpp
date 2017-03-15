#include "Utils.h"
#include <stdio.h>

char* LoadFileContent(const char*path)
{
    char *pFileContent = NULL;
    FILE*pFile = fopen(path, "rb");
    if (pFile)
    {
        fseek(pFile, 0, SEEK_END);
        int nLen = ftell(pFile);
        if (nLen > 0)
        {
            rewind(pFile);
            pFileContent = new char[nLen + 1];
            fread(pFileContent, 1, nLen, pFile);
            pFileContent[nLen] = '\0';
        }
        fclose(pFile);
    }
    return pFileContent;
}