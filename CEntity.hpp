#ifndef _CENTITY_H_
#define _CENTITY_H_

#include "main.hpp"
#include "CEntity.hpp"
#include "CModel.hpp"

class Entity
{
public:
    Vec   m_p;                  // position
    Vec   m_v;                  // velocity
    Vec   m_v_max;
    Vec   m_a;                  // acceleration
    float m_f;                  // friction
    Entity(Vec p=Vec(0.0f, 0.0f, 0.0f),
           Vec v=Vec(0.0f, 0.0f, 0.0f))
        : m_p(p), m_v(v)
    { init(); }

    Entity* setPos(Vec p) { m_p = p; return this; }

    virtual void draw() {};
    virtual void update() {};

private:
    void init()
    {
        m_v_max = {0.3f, 0.3f, 0.3f};
        m_a = {0.1f, 0.1f, 0.1f};
        m_f = 0.03f;
    }
};

class Camera : public Entity
{
public:
    Camera(Entity *follow) : Entity(), m_follow(follow)
    {
        m_a = {0.055f, 0.035f, 0.035f};
    }
    Camera* setPos(Vec p) { m_p = p; return this; }

    void look()
    {
        gluLookAt( unfoldVector(m_p),
                   unfoldVector(m_follow->m_p),
                   0.0f, 0.0f, 1.0f );
    }
    void update();
    void draw() {};

public:
    Entity *m_follow;
};

class Object : public Entity
{
public:
    Object(Vec p=Vec(0.0f, 0.0f, 0.0f),
           Vec v=Vec(0.0f, 0.0f, 0.0f)) : Entity(p,v) {}
    Object* setModel(Model *model) { m_model = model; return this; }
    Object* setPos(Vec p) { m_p = p; return this; }
    void draw();
protected:
    Model *m_model;
};

class Player : public Object
{
public:
    Player(Vec p=Vec(0.0f, 0.0f, 0.0f),
           Vec v=Vec(0.0f, 0.0f, 0.0f)) : Object(p,v) {}

    Player* setModel(Model *model) { m_model = model; return this; }
    Player* setPos(Vec p) { m_p = p; return this; }
    void getInput();
    void shoot();
    void update();
    void draw();
};

class Enemy : public Object
{
public:
    Enemy(Vec p=Vec(0.0f, 0.0f, 0.0f),
          Vec v=Vec(0.0f, 0.0f, 0.0f)) : Object(p,v) {}
    Enemy* setModel(Model *model) { m_model = model; return this; }
    Enemy* setPos(Vec p) { m_p = p; return this; }
    void update();
    void draw();
};

class Bullet : public Entity
{
public:
    Bullet(Vec p=Vec(0.0f, 0.0f, 0.0f),
           Vec v=Vec(0.0f, 0.0f, 0.0f)) : Entity(p,v) {}
    void update();
    void draw();
};

#endif /* _CENTITY_H_ */
