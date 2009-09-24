#include "CEntity.hpp"

using namespace std;

extern Scene *scene;
extern Player *player;
extern list<Bullet*> bullets;
extern list<Enemy*> enemies;

void Camera::update()
{
    Vec dp = m_follow->m_p - m_p;

    /* x */

    if (fabs(dp.x) > m_f)
    {
        m_v.x += copysign(m_a.x, dp.x);

        if (fabs(m_v.x) > m_v_max.x)
            m_v.x = copysignf(m_v_max.x, m_v.x);

        if (fabs(m_v.x) > m_f)
            m_v.x -= copysignf(m_f, m_v.x);
        else
            m_v.x = 0.0f;

        m_p.x += m_v.x;
    }
    else
    {
        m_p.x = m_follow->m_p.x;
    }

    /* z */

    if (fabs(dp.z) > m_f)
    {
        // TODO
        if ( dp.module() >= m_maxDistance )
        {
            m_v.z = m_follow->m_v.z;
        }
        else
        {
            //m_v.z += copysign(m_a.z, dp.z);

            if (fabs(m_v.z) > m_f)
                m_v.z -= copysignf(m_f, m_v.z);
            else
                m_v.z = 0.0f;
        }

        m_p.z += m_v.z;
    }
    else
    {
        m_p.z = m_follow->m_p.z;
    }
}

void Camera::look()
{
#if 0
    gluLookAt( 15.0f, player->m_p.y, player->m_p.z,
               unfoldVector(m_follow->m_p),
               0.0f, 1.0f, 0.0f );
#else
    gluLookAt( unfoldVector(m_p),
               unfoldVector(m_follow->m_p),
               0.0f, 0.0f, 1.0f );
#endif
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

    if ( keystate[SDLK_UP] || keystate[SDLK_k] )
        m_v.z += m_a.z;
    if ( keystate[SDLK_DOWN] || keystate[SDLK_j] )
        m_v.z -= m_a.z;
    if ( keystate[SDLK_RIGHT] || keystate[SDLK_l] )
        m_v.x += m_a.x;
    if ( keystate[SDLK_LEFT] || keystate[SDLK_h] )
        m_v.x -= m_a.x;

    if ( keystate[SDLK_SPACE] || keystate[SDLK_z] )
        shoot();
}

void Object::draw(bool use_dl)
{
    m_model->draw(use_dl);
}

void Enemy::draw()
{
    glPushMatrix();
    {
        glRotatef ((m_p.z-player->m_p.z)*LULZFACTOR, 0.0f, 0.0f, 1.0f);//lulz
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
        glRotatef (sin(SDL_GetTicks()/800.0f)*10.0f, 0.0f, 0.0f, 1.0f);
        glRotatef (cos(SDL_GetTicks()/800.0f)*10.0f, 1.0f, 0.0f, 0.0f);
        Object::draw(false);
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
        glRotatef ((m_p.z-player->m_p.z)*LULZFACTOR, 0.0f, 0.0f, 1.0f);//lulz
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
