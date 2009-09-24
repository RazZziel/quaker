#ifndef _MAIN_H_
#define _MAIN_H_

#define USE_SHADERS_CG_
#define USE_SHADERS_ARB

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <math.h>
#include <SDL.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef USE_SHADERS_CG
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

#define LULZFACTOR ( 2 * sin(SDL_GetTicks()/1000.0f) )

#define TRUE  1
#define FALSE 0

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP    32

//VECTOR TmpLight = {unfoldVector(-( /*player->m_v +*/ Vec( 3.0f,-3.0f,0.1f)))};
#define derp(a,b,c,n)                                                   \
    do {                                                                \
        VECTOR TmpLight = {unfoldVector(-( /*player->m_v +*/ Vec(-3.0f,-3.0f,0.1f)))}; \
        Normalize(TmpLight);                                            \
        VECTOR TmpVector = RotateVectorIn(TmpMatrix, n);                \
        Normalize(TmpVector);                                           \
        float TmpShade = DotProduct(TmpVector, TmpLight);               \
        if (TmpShade < 0.0f) TmpShade = 0.0f;                           \
        glTexCoord1f(TmpShade);                                         \
        glVertex3f((a), (b), (c));                                      \
    } while (0)
#define derp2(a,b,c,i)                                          \
    do {                                                        \
        VECTOR TmpNormal = {(a),(b),(c)+(i)-player->m_p.z};     \
        derp(a,b,c,TmpNormal);                                  \
    } while (0)

#endif /* _MAIN_H_ */
