#include "main.hpp"
#include "CModel.hpp"

extern VECTOR lightAngle;
extern bool outlineDraw;
extern bool outlineWidth;

void Model::draw()
{
    float  TmpShade;       // Temporary Shader Value
    VECTOR TmpVector;      // Temporary Vector Structures
    MATRIX TmpMatrix;      // Temporary MATRIX Structure

#ifdef USE_GL_DL
    if (!m_dl)
    {
        m_dl = glGenLists(1);
        glNewList( m_dl, GL_COMPILE );
#endif

        glGetFloatv (GL_MODELVIEW_MATRIX, TmpMatrix.Data);           // Get The Generated Matrix

        /* Cel-Shading Code */

        glEnable (GL_TEXTURE_1D);                                    // Enable 1D Texturing
        glBindTexture (GL_TEXTURE_1D, m_shaderTexture[0]);           // Bind Our Texture

        glColor3f (1.0f, 1.0f, 1.0f);                                // Set The Color Of The Model

        glBegin (GL_TRIANGLES);                                      // Tell OpenGL That We're Drawing Triangles
        {
            for (int i = 0; i < m_polyNum; i++)                      // Loop Through Each Polygon
            {
                for (int j = 0; j < 3; j++)                          // Loop Through Each Vertex
                {
                    RotateVector (TmpMatrix, m_polyData[i].Verts[j].Nor, TmpVector);  // Rotate This By The Matrix

                    Normalize (TmpVector);                           // Normalize The New Normal

                    TmpShade = DotProduct (TmpVector, lightAngle);   // Calculate The Shade Value

                    if (TmpShade < 0.0f)
                        TmpShade = 0.0f;                             // Clamp The Value to 0 If Negative

                    glTexCoord1f (TmpShade);                         // Set The Texture Co-ordinate As The Shade Value
                    glVertex3fv (&m_polyData[i].Verts[j].Pos.x);     // Send The Vertex Position
                }
            }
        }
        glEnd();


        glDisable (GL_TEXTURE_1D);                                   // Disable 1D Textures

        /* Outline */

        float outlineColor[3] = { 0.0f, 0.0f, 0.0f };                // Color Of The Lines

        if (outlineDraw)                                             // Check To See If We Want To Draw The Outline
        {
            glEnable (GL_BLEND);                                     // Enable Blending
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);        // Set The Blend Mode

            glPolygonMode (GL_BACK, GL_LINE);                        // Draw Backfacing Polygons As Wireframes
            glLineWidth (outlineWidth);                              // Set The Line Width

            glCullFace (GL_FRONT);                                   // Don't Draw Any Front-Facing Polygons
            glDepthFunc (GL_LEQUAL);                                 // Change The Depth Mode
            glColor3fv (&outlineColor[0]);                           // Set The Outline Color

            glBegin (GL_TRIANGLES);                                  // Tell OpenGL What We Want To Draw
            {
                for (int i = 0; i < m_polyNum; i++)                  // Loop Through Each Polygon
                {
                    for (int j = 0; j < 3; j++)                      // Loop Through Each Vertex
                    {
                        glVertex3fv (&m_polyData[i].Verts[j].Pos.x); // Send The Vertex Position
                    }
                }
            }
            glEnd ();                                                // Tell OpenGL We've Finished

            glDepthFunc (GL_LESS);                                   // Reset The Depth-Testing Mode
            glCullFace (GL_BACK);                                    // Reset The Face To Be Culled
            glPolygonMode (GL_BACK, GL_FILL);                        // Reset Back-Facing Polygon Drawing Mode
            glDisable (GL_BLEND);                                    // Disable Blending
        }

#ifdef USE_GL_DL
        glEndList();
    }

    glCallList( m_dl );
#endif
}


void Model::readShader(string filename)
{
    cout << "Loading " << filename << "..." << endl;

    FILE *In = fopen (filename.c_str(), "r");
    char  Line[255];

    if (In)
    {
        for (int i = 0; i < 32; i++)                    // Loop Though The 32 Greyscale Values
        {
            if (feof (In))
                break;

            fgets (Line, 255, In);                      // Get The Current Line

            m_shaderData[i][0] =
                m_shaderData[i][1] =
                m_shaderData[i][2] = float(atof(Line));  // Copy Over The Value
        }

        fclose (In);                                             // Close The File
    }

    glGenTextures (1, &m_shaderTexture[0]);                        // Get A Free Texture ID

    glBindTexture (GL_TEXTURE_1D, m_shaderTexture[0]);             // Bind This Texture. From Now On It Will Be 1D

    // For Crying Out Loud Don't Let OpenGL Use Bi/Trilinear Filtering!
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage1D (GL_TEXTURE_1D, 0, GL_RGB, 32, 0, GL_RGB , GL_FLOAT, m_shaderData);      // Upload
}

void Model::readMesh(string filename)
{
    cout << "Loading " << filename << "..." << endl;

    FILE *In = fopen (filename.c_str(), "rb");

    if (In)
    {
        fread (&m_polyNum, sizeof (int), 1, In);
        m_polyData = new POLYGON [m_polyNum];
        fread (&m_polyData[0], sizeof (POLYGON) * m_polyNum, 1, In);
        fclose (In);
    }
}
