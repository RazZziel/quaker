#include "main.hpp"
#include "CModel.hpp"

extern bool  outlineDraw;
extern float outlineWidth;
extern Vec   lightAngle;
extern bool  doShaders;

#if USE_SHADERS == ARB
extern ARBShader  *program_cel;
#elif USE_SHADERS == GLSL
extern GLSLShader *program_cel;
#endif

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

        glEnable(GL_TEXTURE_1D);
#if 1//REMOVE THE FUCK OUT
        unsigned int g_id_cel_brightness = 0;
        unsigned char BrightnessData[16] = { 127, 127, 127, 191,
                                             191, 191, 191, 191,
                                             255, 255, 255, 255,
                                             255, 255, 255, 255 };
        glGenTextures(1, &g_id_cel_brightness);
        glBindTexture(GL_TEXTURE_1D, g_id_cel_brightness);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 16, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, BrightnessData);
#else
        //glBindTexture(GL_TEXTURE_1D, m_shaderTexture[0]);
#endif

        MATRIX TmpMatrix;
        glGetFloatv(GL_MODELVIEW_MATRIX, TmpMatrix.Data);

        if (doShaders)
        {
#if USE_SHADERS == ARB or USE_SHADERS == GLSL
            program_cel->Enable();
# define derpderp(a,b,c,n) glVertex3f(a,b,c)

# if USE_SHADERS == ARB
            program_cel->Bind( 0, (float[4]) {unfoldVector(lightAngle), 0.0f} );
# elif USE_SHADERS == GLSL
            program_cel->Bind( "light_position", lightAngle );
# endif

#else
# define derpderp derp
#endif
        }

        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin (GL_TRIANGLES);
        {
            for (int i = 0; i < m_polyNum; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    glNormal3f(unfoldVector( m_polyData[i].Verts[j].Nor ));
                    derpderp( m_polyData[i].Verts[j].Pos.x,
                              m_polyData[i].Verts[j].Pos.y,
                              m_polyData[i].Verts[j].Pos.z,
                              m_polyData[i].Verts[j].Nor );
                }
            }
        }
        glEnd();

        if (doShaders)
        {
#if USE_SHADERS == ARB or USE_SHADERS == GLSL
            program_cel->Disable();
#endif
        }

        glDisable (GL_TEXTURE_1D);

        /* Outline */
#if 1
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

            // glutSolidTeapot(0.75);

            glDepthFunc(GL_LESS);
            glCullFace(GL_BACK);
            glPolygonMode(GL_BACK, GL_FILL);
            glDisable(GL_BLEND);
        }

        glPopAttrib();
#endif

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

            fgets(Line, 255, In);                      // Get The Current Line

            m_shaderData[i][0] =
                m_shaderData[i][1] =
                m_shaderData[i][2] = float(atof(Line));  // Copy Over The Value
        }

        fclose(In);                                             // Close The File
    }

    glGenTextures(1, &m_shaderTexture[0]);                        // Get A Free Texture ID

    glBindTexture(GL_TEXTURE_1D, m_shaderTexture[0]);             // Bind This Texture. From Now On It Will Be 1D

    // For Crying Out Loud Don't Let OpenGL Use Bi/Trilinear Filtering!
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 32, 0, GL_RGB , GL_FLOAT, m_shaderData);      // Upload
}

void Model::readMesh(string filename)
{
    cout << "Loading " << filename << "..." << endl;

    FILE *In = fopen (filename.c_str(), "rb");

    if (In)
    {
        fread(&m_polyNum, sizeof(int), 1, In);
        m_polyData = new POLYGON[m_polyNum];
        fread(&m_polyData[0], sizeof(POLYGON) * m_polyNum, 1, In);
        fclose(In);
    }
}
