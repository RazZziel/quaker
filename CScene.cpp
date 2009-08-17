#include "main.hpp"
#include "CScene.hpp"

extern Player *player;

void Scene::draw()
{
    const float
        length = 0.3f,
        height = 0.2f;
    int z = player->m_p.z;
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
                    glVertex3f (margins[0],  height, z-length);
                    glVertex3f (margins[1],  height, z-length);
                    glVertex3f (margins[1], -height, z-length);
                    glVertex3f (margins[0], -height, z-length);

                    glColor3f (0.0f, 1.0f, 0.0f);
                    glVertex3f (margins[1],  height, z-length);
                    glVertex3f (margins[0],  height, z-length);
                    glVertex3f (margins[0],  height, z+length);
                    glVertex3f (margins[1],  height, z+length);

                    glColor3f (0.0f, 0.0f, 1.0f);
                    glVertex3f (margins[0],  height, z+length);
                    glVertex3f (margins[0], -height, z+length);
                    glVertex3f (margins[1], -height, z+length);
                    glVertex3f (margins[1],  height, z+length);
                }
                glEnd();
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
}
