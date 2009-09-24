#ifndef _CMODEL_H_
#define _CMODEL_H_

#include "main.hpp"

using namespace std;

class Model
{
public:
    Model(string fileShader, string fileMesh)
        : m_dl(0)
    {
        readShader( fileShader );
        readMesh( fileMesh );
    }

    ~Model()
    {
        glDeleteTextures (1, &m_shaderTexture[0]);   // Delete The Shader Texture
        delete [] m_polyData;                        // Delete The Polygon Data
    }

    void draw(bool use_dl=true);

//private:
    POLYGON *m_polyData;        // Polygon Data
    int      m_polyNum;         // Number Of Polygons
    GLuint   m_shaderTexture[1]; // Storage For One Texture
    float    m_shaderData[32][3]; // Storate For The 96 Shader Values
    GLuint   m_dl;

    void readShader(string filename);
    void readMesh(string filename);
};


#endif /* _CMODEL_H_ */
