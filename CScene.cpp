#include "main.hpp"
#include "CScene.hpp"

extern Player *player;
extern VECTOR lightAngle;
extern GLuint program_cel;

void Scene::draw()
{
    const float
        length = 0.3f,
        height = 0.2f;
    int z = player->m_p.z;

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glEnable (GL_TEXTURE_1D);
    glBindTexture (GL_TEXTURE_1D, player->m_model->m_shaderTexture[0]);

#if defined(USE_SHADERS_ARB)
    glEnable(GL_VERTEX_PROGRAM_ARB);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, program_cel);
    glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, unfoldVector(lightAngle), 0.0f);
# define derpderp(a,b,c,n) glVertex3f(a,b,c)
#else
    MATRIX TmpMatrix;
    glGetFloatv(GL_MODELVIEW_MATRIX, TmpMatrix.Data);
# define derpderp derp2
#endif
    glPushMatrix();
    {
        glTranslatef (0.0f, -1.0f, 0.0f);
        glRotatef ((z-player->m_p.z)*2.0f, 0.0f, 0.0f, 1.0f);//lulz correction

        for (int i=-50; i<50; i++)
        {
            glPushMatrix();
            {
                glRotatef (i*LULZFACTOR, 0.0f, 0.0f, 1.0f);//lulz
                glBegin(GL_QUADS);
                {
                    glColor3f( 1.0f, 0.0f, 0.0f );
                    derpderp( margins[0],  height, i+z-length, i );
                    derpderp( margins[1],  height, i+z-length, i );
                    derpderp( margins[1], -height, i+z-length, i );
                    derpderp( margins[0], -height, i+z-length, i );

                    glColor3f( 0.0f, 1.0f, 0.0f );
                    derpderp( margins[1],  height, i+z-length, i );
                    derpderp( margins[0],  height, i+z-length, i );
                    derpderp( margins[0],  height, i+z+length, i );
                    derpderp( margins[1],  height, i+z+length, i );

                    glColor3f( 0.0f, 0.0f, 1.0f );
                    derpderp( margins[0],  height, i+z+length, i );
                    derpderp( margins[0], -height, i+z+length, i );
                    derpderp( margins[1], -height, i+z+length, i );
                    derpderp( margins[1],  height, i+z+length, i );
                }
                glEnd();
            }
            glPopMatrix();
        }

    }
    glPopMatrix();

#if defined(USE_SHADERS_ARB)
    glDisable(GL_VERTEX_PROGRAM_ARB);
#endif

    glDisable (GL_TEXTURE_1D);
    glPopAttrib();
}
