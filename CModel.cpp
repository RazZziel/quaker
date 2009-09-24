#include "main.hpp"
#include "CModel.hpp"

extern bool outlineDraw;
extern bool outlineWidth;
extern VECTOR lightAngle;
extern GLuint program_cel;

void Model::draw(bool use_dl)
{
    if (!use_dl || !m_dl)
    {
        if (use_dl)
        {
            m_dl = glGenLists(1);
            glNewList( m_dl, GL_COMPILE );
        }

        glPushAttrib(GL_ALL_ATTRIB_BITS);

        glEnable (GL_TEXTURE_1D);
        glBindTexture (GL_TEXTURE_1D, m_shaderTexture[0]);

#if defined(USE_SHADERS_ARB)
        glEnable(GL_VERTEX_PROGRAM_ARB);
        glBindProgramARB(GL_VERTEX_PROGRAM_ARB, program_cel);
        glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, unfoldVector(lightAngle), 0.0f);
# define derpderp(a,b,c,n) glVertex3f(a,b,c)
#else
        MATRIX TmpMatrix;
        glGetFloatv(GL_MODELVIEW_MATRIX, TmpMatrix.Data);
# define derpderp derp
#endif
        glColor3f (1.0f, 1.0f, 1.0f);

        glBegin (GL_TRIANGLES);
        {
            for (int i = 0; i < m_polyNum; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    // derpderp( unfoldVector(m_polyData[i].Verts[j].Pos),
                    //           m_polyData[i].Verts[j].Nor );
                    derpderp( m_polyData[i].Verts[j].Pos.x,
                              m_polyData[i].Verts[j].Pos.y,
                              m_polyData[i].Verts[j].Pos.z,
                              m_polyData[i].Verts[j].Nor );
                }
            }
        }
        glEnd();

#if defined(USE_SHADERS_ARB)
        glDisable(GL_VERTEX_PROGRAM_ARB);
#endif

        glDisable (GL_TEXTURE_1D);

        /* Outline */

        float outlineColor[3] = { 0.0f, 0.0f, 0.0f };

        if (outlineDraw)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

            glPolygonMode(GL_BACK, GL_LINE);
            glLineWidth(outlineWidth);

            glCullFace(GL_FRONT);
            glDepthFunc(GL_LEQUAL);
            glColor3fv(&outlineColor[0]);

#if 1
            glBegin (GL_TRIANGLES);
            {
                for (int i = 0; i < m_polyNum; i++)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        glVertex3f(unfoldVector(m_polyData[i].Verts[j].Pos));
                    }
                }
            }
            glEnd ();
#else
            glDrawElements(GL_TRIANGLES, m_polyNum*3, GL_UNSIGNED_INT, m_polyData);
#endif

            glDepthFunc (GL_LESS);                                   // Reset The Depth-Testing Mode
            glCullFace (GL_BACK);                                    // Reset The Face To Be Culled
            glPolygonMode (GL_BACK, GL_FILL);                        // Reset Back-Facing Polygon Drawing Mode
            glDisable (GL_BLEND);                                    // Disable Blending
            glPopAttrib();
        }

        if (use_dl)
        {
            glEndList();
        }
        else
        {
            return;
        }
    }

    glCallList( m_dl );
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
