#include "main.hpp"
#include "CScene.hpp"

extern Player *player;
extern VECTOR lightAngle;

void Scene::draw()
{
#define derp(a,b,c)                                                     \
    do {                                                                \
        VECTOR TmpNormal = {a,b,c+i-player->m_p.z};                     \
        VECTOR TmpLighta = {unfoldVector(-( /*player->m_v +*/ Vec( 3.0f,-3.0f,0.1f)))}; \
        VECTOR TmpLightb = {unfoldVector(-( /*player->m_v +*/ Vec(-3.0f,-3.0f,0.1f)))}; \
        Normalize (TmpLighta);                                          \
        Normalize (TmpLightb);                                          \
        VECTOR TmpVector  = RotateVectorIn (TmpMatrix, TmpNormal);      \
        Normalize (TmpVector);                                          \
        float TmpShadea = DotProduct (TmpVector, TmpLighta);            \
        if (TmpShadea < 0.0f) TmpShadea = 0.0f;                         \
        float TmpShadeb = DotProduct (TmpVector, TmpLightb);            \
        if (TmpShadeb < 0.0f) TmpShadeb = 0.0f;                         \
        float TmpShade = max(TmpShadea, TmpShadeb);                     \
        glTexCoord1f (TmpShadeb);                                       \
        glVertex3f (a, b, c);                                           \
    } while (0)
    MATRIX TmpMatrix;
    glGetFloatv (GL_MODELVIEW_MATRIX, TmpMatrix.Data);

    const float
        length = 0.3f,
        height = 0.2f;
    int z = player->m_p.z;

    glEnable (GL_TEXTURE_1D);
    glBindTexture (GL_TEXTURE_1D, player->m_model->m_shaderTexture[0]);
    glPushMatrix();
    {
        glTranslatef (0.0f, -1.0f, 0.0f);
        glRotatef ((z-player->m_p.z)*2.0f, 0.0f, 0.0f, 1.0f);//lulz correction

        for (int i=-50; i<50; i++)
        {
            glPushMatrix();
            {
                glTranslatef (0.0f, 0.0f, i);
                glRotatef (i*LULZFACTOR, 0.0f, 0.0f, 1.0f);//lulz
                glBegin(GL_QUADS);
                {
                    glColor3f (1.0f, 0.0f, 0.0f);
                    derp (margins[0],  height, z-length);
                    derp (margins[1],  height, z-length);
                    derp (margins[1], -height, z-length);
                    derp (margins[0], -height, z-length);

                    glColor3f (0.0f, 1.0f, 0.0f);
                    derp (margins[1],  height, z-length);
                    derp (margins[0],  height, z-length);
                    derp (margins[0],  height, z+length);
                    derp (margins[1],  height, z+length);

                    glColor3f (0.0f, 0.0f, 1.0f);
                    derp (margins[0],  height, z+length);
                    derp (margins[0], -height, z+length);
                    derp (margins[1], -height, z+length);
                    derp (margins[1],  height, z+length);
                }
                glEnd();
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
    glDisable (GL_TEXTURE_1D);
}
