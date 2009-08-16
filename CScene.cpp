#include "main.hpp"
#include "CScene.hpp"

void Scene::draw()
{
    glPushMatrix();
    {
        glTranslatef (0.0f, -1.0f, -10.0f);
        for (int i=0; i<100; i++)
        {
            glTranslatef (0.0f, 0.0f, 1.0f);
            glBegin(GL_QUADS);
            {
                glColor3f (1.0f, 0.0f, 0.0f);
                glVertex3f (margins[0],  0.2f, -0.3f);
                glVertex3f (margins[1],  0.2f, -0.3f);
                glVertex3f (margins[1], -0.2f, -0.3f);
                glVertex3f (margins[0], -0.2f, -0.3f);

                glColor3f (0.0f, 1.0f, 0.0f);
                glVertex3f (margins[0],  0.2f,  0.3f);
                glVertex3f (margins[1],  0.2f,  0.3f);
                glVertex3f (margins[1],  0.2f, -0.3f);
                glVertex3f (margins[0],  0.2f, -0.3f);

                glColor3f (0.0f, 0.0f, 1.0f);
                glVertex3f (margins[0],  0.2f,  0.3f);
                glVertex3f (margins[0], -0.2f,  0.3f);
                glVertex3f (margins[1], -0.2f,  0.3f);
                glVertex3f (margins[1],  0.2f,  0.3f);
            }
            glEnd();
        }
    }
    glPopMatrix();
}
