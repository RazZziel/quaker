#include "CEntity.hpp"

using namespace std;

extern Scene *scene;
extern list<Bullet*> bullets;
extern list<Enemy*> enemies;

void Camera::update()
{
    /* x */

    if (fabs(m_p.x-m_follow->m_p.x) > m_f)
    {
        m_v.x += copysign(m_a.x, m_follow->m_p.x - m_p.x);

        if (fabs(m_v.x) > m_v_max.x)
            m_v.x = copysignf(m_v_max.x, m_v.x);

        if (fabs(m_v.x) > m_f)
            m_v.x -= m_f * copysignf(1.0f, m_v.x);
        else
            m_v.x = 0.0f;

        m_p.x += m_v.x;
    }
    else
        m_p.x = m_follow->m_p.x;

    /* z */

    if (fabs(m_p.z-m_follow->m_p.z) > m_f)
    {
        m_v.z += copysign(m_a.z, m_follow->m_p.z - m_p.z);

        if (fabs(m_v.z) > m_v_max.z)
            m_v.z = copysignf(m_v_max.z, m_v.z);

        if (fabs(m_v.z) > m_f)
            m_v.z -= m_f * copysignf(1.0f, m_v.z);
        else
            m_v.z = 0.0f;

        m_p.z += m_v.z;
    }
    else
        m_p.z = m_follow->m_p.z;
}

void Enemy::update()
{
    m_p.z += m_v.z;
}

void Player::update()
{
    /* x */

    if (fabs(m_v.x) > m_v_max.x)
        m_v.x = copysignf(m_v_max.x, m_v.x);

    if (fabs(m_v.x) > m_f)
        m_v.x -= m_f * copysignf(1.0f, m_v.x);
    else
        m_v.x = 0.0f;

    if (m_p.x-m_v.x > scene->margins[0] &&
        m_p.x-m_v.x < scene->margins[1] )
        m_p.x -= m_v.x;

    /* z */

    if (fabs(m_v.z) > m_v_max.z)
        m_v.z = copysignf(m_v_max.z, m_v.z);

    if (fabs(m_v.z) > m_f)
        m_v.z -= m_f * copysignf(1.0f, m_v.z);
    else
        m_v.z = 0.0f;

    m_p.z += m_v.z;
}

void Player::getInput()
{
    Uint8 *keystate = SDL_GetKeyState(NULL);

    if ( keystate[SDLK_UP] )
        m_v.z += m_a.z;
    if ( keystate[SDLK_DOWN] )
        m_v.z -= m_a.z;
    if ( keystate[SDLK_RIGHT] )
        m_v.x += m_a.x;
    if ( keystate[SDLK_LEFT] )
        m_v.x -= m_a.x;

    if ( keystate[SDLK_SPACE] || keystate[SDLK_z] )
        shoot();
}

void Object::draw()
{
    m_model->draw();
}

void Enemy::draw()
{
    glPushMatrix();
    {
        glTranslatef (unfoldVector(m_p));
        glScalef (0.5f, 0.5f, 0.5f);
        glRotatef (180, 0.0f, 1.0f, 0.0f);
        glRotatef (m_v.x*180, 0.0f, 0.0f, 1.0f);
        glRotatef (m_v.z*90, 1.0f, 0.0f, 0.0f);
        Object::draw();
    }
    glPopMatrix();
}

void Player::draw()
{
    glPushMatrix();
    {
        glTranslatef (unfoldVector(m_p));
        glRotatef (m_v.x*180, 0.0f, 0.0f, 1.0f);
        glRotatef (m_v.z*90, 1.0f, 0.0f, 0.0f);
        Object::draw();
    }
    glPopMatrix();
}

void Player::shoot()
{
    bullets.push_back(new Bullet(m_p-Vec(-0.2f,0.0f,0.0f), {0.0f, 0.0f, 0.7f}));
    bullets.push_back(new Bullet(m_p+Vec(-0.2f,0.0f,0.0f), {0.0f, 0.0f, 0.7f}));
}

void Bullet::update()
{
    m_p.z += m_v.z;
}

void Bullet::draw()
{
    glPushMatrix();
    {
        glTranslatef (m_p.x, m_p.y, m_p.z+0.6f);
        glBegin(GL_QUADS);
        {
            glColor3f (0.0f, 0.0f, 0.0f);
            glVertex3f (-0.02f,  0.0f,  0.1f);
            glVertex3f ( 0.02f,  0.0f,  0.1f);
            glVertex3f ( 0.02f,  0.0f, -0.1f);
            glVertex3f (-0.02f,  0.0f, -0.1f);
        }
        glEnd();
    }
    glPopMatrix();
}
