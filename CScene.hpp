#ifndef _CSCENE_H_
#define _CSCENE_H_

class Scene
{
public:
    GLfloat margins[3];
    Scene() : margins{-3.0f, 3.0f, 0.0f} {}
    void draw();
};

#endif /* _CSCENE_H_ */
