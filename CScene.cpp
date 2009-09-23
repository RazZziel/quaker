#include "main.hpp"
#include "CScene.hpp"

extern Player *player;
extern VECTOR lightAngle;
extern CGparameter path;

void Scene::draw()
{
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
                glRotatef (i*LULZFACTOR, 0.0f, 0.0f, 1.0f);//lulz
                glBegin(GL_QUADS);
                {
                    glColor3f (1.0f, 0.0f, 0.0f);
                    derp (margins[0],  height, i+z-length, i);
                    derp (margins[1],  height, i+z-length, i);
                    derp (margins[1], -height, i+z-length, i);
                    derp (margins[0], -height, i+z-length, i);

                    glColor3f (0.0f, 1.0f, 0.0f);
                    derp (margins[1],  height, i+z-length, i);
                    derp (margins[0],  height, i+z-length, i);
                    derp (margins[0],  height, i+z+length, i);
                    derp (margins[1],  height, i+z+length, i);

                    glColor3f (0.0f, 0.0f, 1.0f);
                    derp (margins[0],  height, i+z+length, i);
                    derp (margins[0], -height, i+z+length, i);
                    derp (margins[1], -height, i+z+length, i);
                    derp (margins[1],  height, i+z+length, i);
                }
                glEnd();
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
    glDisable (GL_TEXTURE_1D);
}
