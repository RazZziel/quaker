#ifndef _MAIN_H_
#define _MAIN_H_

#define NONE 0
#define CG   1
#define ARB  2
#define GLSL 3
#define USE_SHADERS ARB

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <math.h>
#include <SDL.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#if USE_SHADERS == CG
# include <Cg/cg.h>
# include <Cg/cgGL.h>
#endif

#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>

#include "vector.h"

#include "CVector.hpp"

#include "CModel.hpp"
#include "CScene.hpp"
#include "CEntity.hpp"
#include "CShader.hpp"

#define die2(fmt, ...)                                   \
    do {                                                 \
        fprintf( stderr, fmt "\n", __VA_ARGS__ );        \
        exit(1);                                         \
    } while (0)

#define die(fmt, ...)                                   \
    do {                                                \
        die2( "%s:%d: error: " fmt "\n",                \
              __FILE__, __LINE__, __VA_ARGS__ );        \
    } while (0)

//TODO: function lol
#define die3(fmt)                                       \
    do {                                                \
        die2( "%s:%d: error: " fmt "\n",                \
              __FILE__, __LINE__ );                     \
    } while (0)

#define checkGlError()                          \
    do {                                        \
        GLenum error = glGetError();            \
        if (error)                              \
        {                                       \
            die( "%s", gluErrorString(error) ); \
        }                                       \
    } while (0)

#define LULZFACTOR ( 2 * sin(SDL_GetTicks()/1000.0f) )

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP    32

#define derp(a,b,c,n)                                           \
    do {                                                        \
        VECTOR TmpLight = {3.0f,3.0f,-0.1f};                    \
        Normalize(TmpLight);                                    \
        VECTOR TmpVector = RotateVectorIn(TmpMatrix, n);        \
        Normalize(TmpVector);                                   \
        float TmpShade = DotProduct(TmpVector, TmpLight);       \
        if (TmpShade < 0.0f) TmpShade = 0.0f;                   \
        glTexCoord1f(TmpShade);                                 \
        glVertex3f((a), (b), (c));                              \
    } while (0)
#define derp2(a,b,c,i)                                          \
    do {                                                        \
        VECTOR TmpNormal = {(a),(b),(c)+(i)-player->m_p.z};     \
        derp(a,b,c,TmpNormal);                                  \
    } while (0)
#define derp3(a,b,c,n)                                          \
    do {                                                        \
        /*VECTOR TmpNormal = {(a),(b),(c)+(i)-player->m_p.z};*/ \
        VECTOR TmpNormal = {(a),(b),(n.z)+(i)};                 \
        TmpNormal = RotateVectorIn(TmpMatrix, TmpNormal);       \
        Normalize(TmpNormal);                                   \
        glNormal3f( unfoldVector(TmpNormal) );                  \
        glVertex3f( (a), (b), (c) );                            \
    } while (0)


#endif /* _MAIN_H_ */
