#include "ObjModel.h"
#include "Utils.h"
#include "stdio.h"
#include "sstream"
#include "string"
#include "vector"

struct VertexInfo
{
    float v[3];
    VertexInfo()
    {
        memset(v, 0, sizeof(float) * 3); // èâénâªêîéöv
    }
};

struct VertexDefine
{
    int positionIndex;
    int texcoordIndex;
    int normalIndex;
};

void ObjModel::Init(const char* modelFilePath)
{
    std::vector<VertexInfo> positions;
    std::vector<VertexInfo> texcoords;
    std::vector<VertexInfo> normals;

    std::vector<VertexDefine> vertices;
    std::vector<unsigned int> faces;

    // 1. load model from file
    char* fileContent = LoadFileContent(modelFilePath);

    // 2. deecode model
    std::stringstream ssFileContent(fileContent);
    char szOneLine[256];
    std::string tmp;
    while (!ssFileContent.eof())
    {
        memset(szOneLine, 0, 256);
        ssFileContent.getline(szOneLine, 256);
        if (strlen(szOneLine) > 0)
        {
            std::stringstream ssOneLine(szOneLine);
            if (szOneLine[0] == 'v')
            {
                if (szOneLine[1] == 't')
                {
                    // vt => vertex texcoord
                    VertexInfo vi;
                    ssOneLine >> tmp;
                    ssOneLine >> vi.v[0];
                    ssOneLine >> vi.v[1];
                    texcoords.push_back(vi);
                    printf("texcoord:%f, %f\n", vi.v[0], vi.v[1]);
                } 
                else if (szOneLine[1] == 'n')
                {
                    // vt => vertex normal
                    VertexInfo vi;
                    ssOneLine >> tmp;
                    ssOneLine >> vi.v[0];
                    ssOneLine >> vi.v[1];
                    ssOneLine >> vi.v[2];
                    normals.push_back(vi);
                    printf("normals:%f, %f, %f\n", vi.v[0], vi.v[1], vi.v[2]);
                }
                else
                {
                    // v => vertex coordinate
                    VertexInfo vi;
                    ssOneLine >> tmp;
                    ssOneLine >> vi.v[0];
                    ssOneLine >> vi.v[1];
                    ssOneLine >> vi.v[2];
                    positions.push_back(vi);
                    printf("positions:%f, %f, %f\n", vi.v[0], vi.v[1], vi.v[2]);
                }
            } else if (szOneLine[0] == 'f')
            {
                printf("face:%s\n", szOneLine);
                ssOneLine >> tmp;
                std::string vertexStr;
                for (int i = 0; i < 3; i++)
                {
                    ssOneLine >> vertexStr;
                    size_t verPos = vertexStr.find_first_of('/');
                    std::string positionIndexStr = vertexStr.substr(0, verPos);
                    size_t texPos = vertexStr.find_first_of(verPos, '/');
                    std::string texcoordIndexStr = vertexStr.substr(verPos + 1, texPos - verPos - 1);
                    std::string normalIndexStr = vertexStr.substr(texPos + 1, vertexStr.length() - texPos - 1);

                    VertexDefine vd;
                    vd.positionIndex = atoi(positionIndexStr.c_str()) - 1;
                    vd.texcoordIndex= atoi(texcoordIndexStr.c_str()) - 1;
                    vd.normalIndex= atoi(normalIndexStr.c_str()) - 1;

                    int nCurrentVertexIndex = -1;
                    size_t nCurrentVertexCount = vertices.size();
                    for (int j = 0; j < nCurrentVertexIndex; i++)
                    {
                        if (vertices[j].positionIndex == vd.positionIndex
                            && vertices[j].texcoordIndex == vd.texcoordIndex
                            && vertices[j].normalIndex == vd.normalIndex)
                        {
                            nCurrentVertexIndex = j;
                            break;
                        }
                    }

                    if(nCurrentVertexIndex == -1)
                    {
                        nCurrentVertexIndex = vertices.size();
                        vertices.push_back(vd);
                    }

                    faces.push_back(nCurrentVertexIndex);
                }
            }
        }
    }
    printf("face count %u\n", faces.size() / 3);

    // 3. convert to opengl vbo & ibo
    int vertexCount = vertices.size();
    VertexData  *vertexes = new VertexData[vertexCount];
    for (int i = 0; i < vertexCount; i ++)
    {
        memcpy(vertexes[i].position, positions[vertices[i].positionIndex].v, sizeof(float) * 3);
        memcpy(vertexes[i].texcoord, texcoords[vertices[i].texcoordIndex].v, sizeof(float) * 2);
        memcpy(vertexes[i].normal, normals[vertices[i].normalIndex].v, sizeof(float) * 3);
    }

    // create vbo
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertexCount, vertexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete vertexes;

    // create ibo
    mIndexCount = (int)faces.size();
    unsigned int *indexes = new unsigned int[mIndexCount];
    for (int i = 0; i < mIndexCount; i++)
    {
        indexes[i] = faces[i];
    }
    glGenBuffers(1, &mIBO);
    glBindBuffer(GL_ARRAY_BUFFER, mIBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * mIndexCount, indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete indexes;

    delete fileContent;
}

void ObjModel::BindModel(GLint posLoc, GLint texcoordLoc, GLint normalLoc)
{
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);

    glEnableVertexAttribArray(texcoordLoc);
    glVertexAttribPointer(texcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));

    glEnableVertexAttribArray(normalLoc);
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ObjModel::Draw()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
