#ifndef _MAIN_H_
#define _MAIN_H_

#define HAVE_CG_H
#define USE_GL_DL_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <math.h>
#include <SDL.h>

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef HAVE_CG_H
# include <Cg/cg.h>
# include <Cg/cgGL.h>
#endif

#include <iostream>
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

#define derp(a,b,c,i)                                                   \
    do {                                                                \
        VECTOR TmpNormal = {(a),(b),(c)+(i)-player->m_p.z};             \
        VECTOR TmpLighta = {unfoldVector(-( /*player->m_v +*/ Vec( 3.0f,-3.0f,0.1f)))}; \
        VECTOR TmpLightb = {unfoldVector(-( /*player->m_v +*/ Vec(-3.0f,-3.0f,0.1f)))}; \
        Normalize(TmpLighta);                                           \
        Normalize(TmpLightb);                                           \
        VECTOR TmpVector  = RotateVectorIn(TmpMatrix, TmpNormal);       \
        Normalize (TmpVector);                                          \
        float TmpShadea = DotProduct(TmpVector, TmpLighta);             \
        if (TmpShadea < 0.0f) TmpShadea = 0.0f;                         \
        float TmpShadeb = DotProduct(TmpVector, TmpLightb);             \
        if (TmpShadeb < 0.0f) TmpShadeb = 0.0f;                         \
        /*float TmpShade = max(TmpShadea, TmpShadeb);*/                 \
        glTexCoord1f(TmpShadeb);                                        \
        glVertex3f((a), (b), (c));                                      \
    } while (0)


#endif /* _MAIN_H_ */
