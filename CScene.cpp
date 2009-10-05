#include "main.hpp"
#include "CScene.hpp"

extern Player *player;
extern Vec     lightAngle;
extern bool    doShaders;

#if USE_SHADERS == ARB
extern ARBShader  *program_cel;
#elif USE_SHADERS == GLSL
extern GLSLShader *program_cel;
#endif

void Scene::draw()
{
    const float
        length = 0.3f,
        height = 0.2f;
    int z = rintf(player->m_p.z);

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glEnable (GL_TEXTURE_1D);
    glBindTexture (GL_TEXTURE_1D, player->m_model->m_shaderTexture[0]);

    MATRIX TmpMatrix;
    glGetFloatv(GL_MODELVIEW_MATRIX, TmpMatrix.Data);

    if (doShaders)
    {
#if USE_SHADERS == ARB or USE_SHADERS == GLSL
        program_cel->Enable();

# if USE_SHADERS == ARB
        program_cel->Bind( 0, (float[4]) {unfoldVector(lightAngle), 0.0f} );
# define derpderp(a,b,c,n) glVertex3f(a,b,c)
# elif USE_SHADERS == GLSL
        program_cel->Bind( "light_position", lightAngle );
//# define derpderp derp3
# define derpderp(a,b,c,n) glVertex3f(a,b,c)
# endif

#else
# define derpderp derp2
#endif
    }

    glPushMatrix();
    {
        glTranslatef(0.0f, -1.0f, 0.0f);
        glRotatef(player->m_p.z-z, 0.0f, 0.0f, 1.0f);//lulz correction

        for (int i=-50; i<50; i++)
        {
            glPushMatrix();
            {
                glRotatef(i*LULZFACTOR, 0.0f, 0.0f, 1.0f);//lulz
                glBegin(GL_QUADS);
                {
                    /* Front */
                    glColor3f( 1.0f, 0.0f, 0.0f );
                    glNormal3f( 0.0f, 0.0f, -1.0f );
                    derpderp( margins[0],  height, i+z-length, i );
                    derpderp( margins[1],  height, i+z-length, i );
                    derpderp( margins[1], -height, i+z-length, i );
                    derpderp( margins[0], -height, i+z-length, i );

                    /* Top */
                    glColor3f( 0.0f, 1.0f, 0.0f );
                    glNormal3f( 0.0f, 1.0f, 0.0f );
                    derpderp( margins[1],  height, i+z-length, i );
                    derpderp( margins[0],  height, i+z-length, i );
                    derpderp( margins[0],  height, i+z+length, i );
                    derpderp( margins[1],  height, i+z+length, i );

                    /* Back */
                    glColor3f( 0.0f, 0.0f, 1.0f );
                    glNormal3f( 0.0f, 0.0f, 1.0f );
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

    if (doShaders)
    {
#if USE_SHADERS == ARB
        program_cel->Disable();
#endif
    }

    glDisable (GL_TEXTURE_1D);
    glPopAttrib();
}
