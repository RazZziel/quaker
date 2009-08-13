/***************************************************************************
 *   Copyright (C) 2006 by Ismael Barros (Raziel)                          *
 *   razielmine@gmail.com                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* Based on Linux/SDL example on NeHe's lesson 37 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL.h"

#include <iostream>
#include <list>
#include <algorithm>

#include "CVector.h"

using namespace std;

/* screen width, height, and bit depth */
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP    32

/* Set up some booleans */
#define TRUE  1
#define FALSE 0

/* This is our SDL surface */
SDL_Surface *surface;

// User Defined Structures
typedef struct tagMATRIX        // A Structure To Hold An OpenGL Matrix
{
    float Data[16];             // We Use [16] Due To OpenGL's Matrix Format
} MATRIX;

typedef struct tagVECTOR                    // A Structure To Hold A Single VECTOR
{                          
    float x, y, z;              // The Components Of The VECTOR
    //VECTOR(_x,_y,_z) : x(_x), y(_y), z(_z) {};
} VECTOR;
#define unfoldVector(v) v.x, v.y, v.z

typedef struct tagVERTEX        // A Structure To Hold A Single Vertex
{
    VECTOR Nor;                 // Vertex Normal
    VECTOR Pos;                 // Vertex Position
} VERTEX;

typedef struct tagPOLYGON       // A Structure To Hold A Single Polygon
{
    VERTEX Verts[3];            // Array Of 3 VERTEX Structures
} POLYGON;

// User Defined Variables
bool  outlineDraw       = true; // Flag To Draw The Outline
bool  outlineSmooth     = false; // Flag To Anti-Alias The Lines
float outlineColor[3]   = { 0.0f, 0.0f, 0.0f }; // Color Of The Lines
float outlineWidth      = 3.0f; // Width Of The Lines

VECTOR lightAngle;              // The Direction Of The Light
bool   lightRotate = false;     // Flag To See If We Rotate The Light

POLYGON *polyData = NULL;       // Polygon Data
int      polyNum  = 0;          // Number Of Polygons

GLuint shaderTexture[1];        // Storage For One Texture

typedef Vector Vec;

class Scene
{
public:
    GLfloat margins[3];
    Scene() : margins({-2.0f, 2.0f, 0.0f}) {}
    void draw();
};

class Entity
{
public:
    Vec m_p;  // position
    Vec m_v;  // velocity
    Vec m_v_max;
    Vec m_a;  // acceleration
    float m_f;   // friction
    Entity() { init(); }
    Entity(Vec p, Vec v)
    {
        init();
        m_p = p;
        m_v = v;
    };

    virtual void draw() {};
    virtual void update() {};

private:
    void init()
    {
        m_p = {0.0f, 4.0f, 0.0f};
        m_v = {0.0f, 0.0f, 0.0f};
        m_v_max = {0.3f, 0.3f, 0.3f};
        m_a = {0.1f, 0.1f, 0.1f};
        m_f = 0.03f;
    }
};

class Camera : public Entity
{
public:
    Entity *m_follow;
    Camera(Entity *follow) : m_follow(follow)
    {
        m_a = {0.055f, 0.035f, 0.035f};
    }
    void look()
    {
        gluLookAt( unfoldVector(m_p),
                   unfoldVector(m_follow->m_p),
                   0.0f, 0.0f, 1.0f );
    }
    void update();
    void draw() {};
};

class Player : public Entity
{
public:
    Player()
    {
        m_p = {0.0f, 0.0f, 0.0f};
    }
    void getInput();
    void shoot();
    void draw();
    void update();
};

class Enemy : public Entity
{
public:
    Enemy(Vec p, Vec v) : Entity(p,v) {}
    void draw();
    void update();
};

class Bullet : public Entity
{
public:
    Bullet(Vec p, Vec v) : Entity(p,v) {}
    void update();
    void draw();
};

list<Entity*> objects;

Scene *scene = new Scene;
Player *player = new Player;
Camera *camera = new Camera(player);

/* function to release/destroy our resources and restoring the old desktop */
void Quit( int returnCode )
{
    /* clean up the window */
    SDL_Quit( );

    /* and exit appropriately */
    exit( returnCode );
}

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
                glVertex3f (-2.0f,  0.2f, -0.3f);
                glVertex3f ( 2.0f,  0.2f, -0.3f);
                glVertex3f ( 2.0f, -0.2f, -0.3f);
                glVertex3f (-2.0f, -0.2f, -0.3f);

                glColor3f (0.0f, 1.0f, 0.0f);
                glVertex3f (-2.0f,  0.2f,  0.3f);
                glVertex3f ( 2.0f,  0.2f,  0.3f);
                glVertex3f ( 2.0f,  0.2f, -0.3f);
                glVertex3f (-2.0f,  0.2f, -0.3f);

                glColor3f (0.0f, 0.0f, 1.0f);
                glVertex3f (-2.0f,  0.2f,  0.3f);
                glVertex3f (-2.0f, -0.2f,  0.3f);
                glVertex3f ( 2.0f, -0.2f,  0.3f);
                glVertex3f ( 2.0f,  0.2f,  0.3f);
            }
            glEnd();
        }
    }
    glPopMatrix();
}

bool ReadMesh(void)                                              // Reads The Contents Of The "model.txt" File
{
    FILE *In = fopen ("Data/Model.txt", "rb");                   // Open The File

    if (!In)
        return FALSE;                                            // Return FALSE If File Not Opened

    fread (&polyNum, sizeof (int), 1, In);                       // Read The Header (i.e. Number Of Polygons)

    polyData = new POLYGON [polyNum];                            // Allocate The Memory

    fread (&polyData[0], sizeof (POLYGON) * polyNum, 1, In);     // Read In All Polygon Data

    fclose (In);                                                 // Close The File

    return TRUE;                                                 // It Worked
}

// Math Functions
inline float DotProduct (VECTOR &V1, VECTOR &V2)                 // Calculate The Angle Between The 2 VECTORs
{
    return V1.x * V2.x + V1.y * V2.y + V1.z * V2.z;              // Return The Angle
}

inline float Magnitude (VECTOR &V)                               // Calculate The Length Of The VECTOR
{
    return sqrtf (V.x * V.x + V.y * V.y + V.z * V.z);            // Return The Length Of The VECTOR
}

void Normalize (VECTOR &V)                                       // Creates A VECTOR With A Unit Length Of 1
{
    float M = Magnitude (V);                                     // Calculate The Length Of The VECTOR 

    if (M != 0.0f)                                               // Make Sure We Don't Divide By 0 
    {
        V.x /= M;                                                // Normalize The 3 Components 
        V.y /= M;
        V.z /= M;
    }
}

void RotateVector (MATRIX &M, VECTOR &V, VECTOR &D)                     // Rotate A Vector Using The Supplied Matrix
{
    D.x = (M.Data[0] * V.x) + (M.Data[4] * V.y) + (M.Data[8]  * V.z);   // Rotate Around The X Axis
    D.y = (M.Data[1] * V.x) + (M.Data[5] * V.y) + (M.Data[9]  * V.z);   // Rotate Around The y Axis
    D.z = (M.Data[2] * V.x) + (M.Data[6] * V.y) + (M.Data[10] * V.z);   // Rotate Around The Z Axis
}
/* A general OpenGL initialization function.  Sets all of the initial parameters. */
bool initGL()           // We call this right after our OpenGL window is created.
{
    int   i;                                                     // Looping Variable
    char  Line[255];                                             // Storage For 255 Characters
    float shaderData[32][3];                                     // Storate For The 96 Shader Values
  
    FILE *In = NULL;                                             // File Pointer

    //g_window = window;
    //g_keys   = keys;

    // Start Of User Initialization
    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // Realy Nice perspective calculations
        
    glClearColor (0.7f, 0.7f, 0.7f, 0.0f);                       // Light Grey Background
    glClearDepth (1.0f);                                         // Depth Buffer Setup

    glEnable (GL_DEPTH_TEST);                                    // Enable Depth Testing
    glDepthFunc (GL_LESS);                                       // The Type Of Depth Test To Do

    glShadeModel (GL_SMOOTH);                                    // Enables Smooth Color Shading
    glDisable (GL_LINE_SMOOTH);                                  // Initially Disable Line Smoothing

    glEnable (GL_CULL_FACE);                                     // Enable OpenGL Face Culling

    glDisable (GL_LIGHTING);                                     // Disable OpenGL Lighting

    In = fopen ("Data/Shader.txt", "r");                         // Open The Shader File

    if (In)                                                      // Check To See If The File Opened
    {
        for (i = 0; i < 32; i++)                                 // Loop Though The 32 Greyscale Values
        {
            if (feof (In))                                       // Check For The End Of The File
                break;

            fgets (Line, 255, In);                               // Get The Current Line

            shaderData[i][0] = shaderData[i][1] = shaderData[i][2] = float(atof(Line)); // Copy Over The Value
        }

        fclose (In);                                             // Close The File
    }

    else
        return FALSE;                                            // It Went Horribly Horribly Wrong

    glGenTextures (1, &shaderTexture[0]);                        // Get A Free Texture ID

    glBindTexture (GL_TEXTURE_1D, shaderTexture[0]);             // Bind This Texture. From Now On It Will Be 1D

    // For Crying Out Loud Don't Let OpenGL Use Bi/Trilinear Filtering!
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage1D (GL_TEXTURE_1D, 0, GL_RGB, 32, 0, GL_RGB , GL_FLOAT, shaderData);      // Upload

    lightAngle.x = 0.0f;                                         // Set The X Direction
    lightAngle.y = 0.0f;                                         // Set The y Direction
    lightAngle.z = 1.0f;                                         // Set The Z Direction

    Normalize (lightAngle);                                      // Normalize The Light Direction
    return ReadMesh ();
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
void resizeWindow(int Width, int Height)
{
    if (Height==0)                              // Prevent A Divide By Zero If The Window Is Too Small
        Height = 1;

    glViewport(0, 0, Width, Height);            // Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}

/* The main drawing function. */
void drawGLScene()
{
        
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Clear The Buffers
    glLoadIdentity ();                                           // Reset The Matrix

    camera->look();

    if (outlineSmooth)                                           // Check To See If We Want Anti-Aliased Lines
    {
        glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);                 // Use The Good Calculations
        glEnable (GL_LINE_SMOOTH);                               // Enable Anti-Aliasing
    }
    else                                                         // We Don't Want Smooth Lines
        glDisable (GL_LINE_SMOOTH);                              // Disable Anti-Aliasing
    
    player->getInput();
    player->update();
    camera->update();

    scene->draw();
    player->draw();

    for (list<Entity*>::iterator it = objects.begin();
         it != objects.end(); )
    {
        if ((*it)->m_p.z < (player->m_p - Vec(0.0f, 0.0f, 4.0f)).z)
        {
            it = objects.erase(it);
            objects.push_back(new Enemy( player->m_p - Vec(((float)random()/(float)RAND_MAX)*4.0f-2.0f,
                                                           0.0f,
                                                           -10.0f-(((float)random()/(float)RAND_MAX)*50.0f)),
                                         {0.0f, 0.0f, (float)random()/(float)RAND_MAX*-0.5f}) );
        }
        else
        {
            (*it)->update();
            (*it)->draw();
            ++it;
        }
    }

    /* Draw it to the screen */
    SDL_GL_SwapBuffers( );


    SDL_Delay(100);
}

void Deinitialize (void)                                         // Any User DeInitialization Goes Here
{
    glDeleteTextures (1, &shaderTexture[0]);                     // Delete The Shader Texture

    delete [] polyData;                                          // Delete The Polygon Data
}

/* function to handle key press events */
void handleKeyPress( SDL_keysym *keysym )
{
    switch ( keysym->sym )
    {
    case SDLK_ESCAPE:
        Quit( 0 );
        break;
    case SDLK_F1:
        SDL_WM_ToggleFullScreen( surface );
        break;
    case SDLK_1:
        outlineDraw = !outlineDraw;
        break;
    case SDLK_2:
        outlineSmooth = !outlineSmooth; 
        break;
    default:
        break;
    }

    return;
}


int main(int argc, char **argv) 
{
    /* Flags to pass to SDL_SetVideoMode */
    int                  videoFlags;
    /* main loop variable */
    int                  done     = FALSE;
    /* used to collect events */
    SDL_Event            event;
    /* this holds some info about our display */
    const SDL_VideoInfo *videoInfo;
    /* whether or not the window is active */
    int                  isActive = TRUE;

    /* initialize SDL */
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        fprintf( stderr, "Video initialization failed: %s\n",
                 SDL_GetError( ) );
        Quit( 1 );
    }

    /* Fetch the video info */
    videoInfo = SDL_GetVideoInfo( );

    if ( !videoInfo )
    {
        fprintf( stderr, "Video query failed: %s\n",
                 SDL_GetError( ) );
        Quit( 1 );
    }

    /* the flags to pass to SDL_SetVideoMode */
    videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

    /* This checks to see if surfaces can be stored in memory */
    if ( videoInfo->hw_available )
        videoFlags |= SDL_HWSURFACE;
    else
        videoFlags |= SDL_SWSURFACE;

    /* This checks if hardware blits can be done */
    if ( videoInfo->blit_hw )
        videoFlags |= SDL_HWACCEL;

    /* Sets up OpenGL double buffering */
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    /* get a SDL surface */
    surface = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
                                videoFlags );

    /* Verify there is a surface */
    if ( !surface )
    {
        fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
        Quit( 1 );
    }

    /* initialize OpenGL */
    initGL( );

    /* resize the initial window */
    resizeWindow( SCREEN_WIDTH, SCREEN_HEIGHT );

    srandom(time(NULL));
    for (int i=0; i<20; i++)
    {
        objects.push_back(new Enemy( player->m_p - Vec(((float)random()/(float)RAND_MAX)*4.0f-2.0f,
                                                       0.0f,
                                                       -10.0f-(((float)random()/(float)RAND_MAX)*50.0f)),
                                     {0.0f, 0.0f, -0.2f+(float)random()/(float)RAND_MAX*-0.8f}) );
    }

    /* wait for events */
    while ( !done )
    {
        /* handle the events in the queue */

        while ( SDL_PollEvent( &event ) )
        {
            switch( event.type )
            {
            case SDL_ACTIVEEVENT:
                /* Something's happend with our focus
                 * If we lost focus or we are iconified, we
                 * shouldn't draw the screen
                 */
                if ( event.active.gain == 0 )
                    isActive = FALSE;
                else
                    isActive = TRUE;
                break;                      
            case SDL_VIDEORESIZE:
                /* handle resize event */
                surface      = SDL_SetVideoMode( event.resize.w,
                                                 event.resize.h,
                                                 16, videoFlags );
                if ( !surface )
                {
                    fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
                    Quit( 1 );
                }
                resizeWindow( event.resize.w, event.resize.h );
                break;
            case SDL_KEYDOWN:
                /* handle key presses */
                handleKeyPress( &event.key.keysym );
                break;
            case SDL_QUIT:
                /* handle quit requests */
                done = TRUE;
                break;
            default:
                break;
            }
        }
    
        /* draw the scene */
        if ( isActive )
            drawGLScene( );
    }
  
    /* clean ourselves up and exit */
    Quit( 0 );

    /* Should never get here */
    return( 0 );
}

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

void Enemy::draw()
{
    float  TmpShade;               // Temporary Shader Value
    VECTOR TmpVector, TmpNormal;   // Temporary Vector Structures
    MATRIX TmpMatrix;              // Temporary MATRIX Structure


    glPushMatrix();
    {
        glTranslatef (unfoldVector(m_p));
        glScalef ( 0.5f, 0.5f, 0.5f );
        glRotatef (180, 0.0f, 1.0f, 0.0f);
        glRotatef (m_v.x*180, 0.0f, 0.0f, 1.0f);
        glRotatef (m_v.z*90, 1.0f, 0.0f, 0.0f);

        glGetFloatv (GL_MODELVIEW_MATRIX, TmpMatrix.Data);           // Get The Generated Matrix

        // Cel-Shading Code //
        glEnable (GL_TEXTURE_1D);                                    // Enable 1D TexturingW )
        glBindTexture (GL_TEXTURE_1D, shaderTexture[0]);             // Bind Our Texture

        glColor3f (1.0f, 1.0f, 1.0f);                                // Set The Color Of The Model

        glBegin (GL_TRIANGLES);                                      // Tell OpenGL That We're Drawing Triangles
        {
            for (int i = 0; i < polyNum; i++)                            // Loop Through Each Polygon
            {
                for (int j = 0; j < 3; j++)                              // Loop Through Each Vertex
                {
                    TmpNormal.x = polyData[i].Verts[j].Nor.x;            // Fill Up The TmpNormal Structure With
                    TmpNormal.y = polyData[i].Verts[j].Nor.y;            // The Current Vertices' Normal Values
                    TmpNormal.z = polyData[i].Verts[j].Nor.z;
                    
                    RotateVector (TmpMatrix, TmpNormal, TmpVector);      // Rotate This By The Matrix
                    
                    Normalize (TmpVector);                               // Normalize The New Normal
                    
                    TmpShade = DotProduct (TmpVector, lightAngle);       // Calculate The Shade Value
                    
                    if (TmpShade < 0.0f)
                        TmpShade = 0.0f;                                 // Clamp The Value to 0 If Negative
                    
                    glTexCoord1f (TmpShade);                             // Set The Texture Co-ordinate As The Shade Value
                    glVertex3fv (&polyData[i].Verts[j].Pos.x);           // Send The Vertex Position
                }
            }
        }
        glEnd();


        glDisable (GL_TEXTURE_1D);                                   // Disable 1D Textures

        // Outline Code //
        if (outlineDraw)                                             // Check To See If We Want To Draw The Outline
        {
            glEnable (GL_BLEND);                                     // Enable Blending
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);        // Set The Blend Mode

            glPolygonMode (GL_BACK, GL_LINE);                        // Draw Backfacing Polygons As Wireframes
            glLineWidth (outlineWidth);                              // Set The Line Width

            glCullFace (GL_FRONT);                                   // Don't Draw Any Front-Facing Polygons

            glDepthFunc (GL_LEQUAL);                                 // Change The Depth Mode

            glColor3fv (&outlineColor[0]);                           // Set The Outline Color

            glBegin (GL_TRIANGLES);                                  // Tell OpenGL What We Want To Draw
            {
                for (int i = 0; i < polyNum; i++)                    // Loop Through Each Polygon
                {
                    for (int j = 0; j < 3; j++)                      // Loop Through Each Vertex
                    {
                        glVertex3fv (&polyData[i].Verts[j].Pos.x);   // Send The Vertex Position
                    }
                }
            }
            glEnd ();                                                // Tell OpenGL We've Finished

            glDepthFunc (GL_LESS);                                   // Reset The Depth-Testing Mode

            glCullFace (GL_BACK);                                    // Reset The Face To Be Culled

            glPolygonMode (GL_BACK, GL_FILL);                        // Reset Back-Facing Polygon Drawing Mode

            glDisable (GL_BLEND);                                    // Disable Blending
        }


    }
    glPopMatrix();    
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

void Player::draw()
{
    float  TmpShade;               // Temporary Shader Value
    VECTOR TmpVector, TmpNormal;   // Temporary Vector Structures
    MATRIX TmpMatrix;              // Temporary MATRIX Structure


    glPushMatrix();
    {
        glTranslatef (unfoldVector(m_p));
        glRotatef (m_v.x*180, 0.0f, 0.0f, 1.0f);
        glRotatef (m_v.z*90, 1.0f, 0.0f, 0.0f);

        glGetFloatv (GL_MODELVIEW_MATRIX, TmpMatrix.Data);           // Get The Generated Matrix

        // Cel-Shading Code //
        glEnable (GL_TEXTURE_1D);                                    // Enable 1D TexturingW )
        glBindTexture (GL_TEXTURE_1D, shaderTexture[0]);             // Bind Our Texture

        glColor3f (1.0f, 1.0f, 1.0f);                                // Set The Color Of The Model

        glBegin (GL_TRIANGLES);                                      // Tell OpenGL That We're Drawing Triangles
        {
            for (int i = 0; i < polyNum; i++)                            // Loop Through Each Polygon
            {
                for (int j = 0; j < 3; j++)                              // Loop Through Each Vertex
                {
                    TmpNormal.x = polyData[i].Verts[j].Nor.x;            // Fill Up The TmpNormal Structure With
                    TmpNormal.y = polyData[i].Verts[j].Nor.y;            // The Current Vertices' Normal Values
                    TmpNormal.z = polyData[i].Verts[j].Nor.z;
                    
                    RotateVector (TmpMatrix, TmpNormal, TmpVector);      // Rotate This By The Matrix
                    
                    Normalize (TmpVector);                               // Normalize The New Normal
                    
                    TmpShade = DotProduct (TmpVector, lightAngle);       // Calculate The Shade Value
                    
                    if (TmpShade < 0.0f)
                        TmpShade = 0.0f;                                 // Clamp The Value to 0 If Negative
                    
                    glTexCoord1f (TmpShade);                             // Set The Texture Co-ordinate As The Shade Value
                    glVertex3fv (&polyData[i].Verts[j].Pos.x);           // Send The Vertex Position
                }
            }
        }
        glEnd();


        glDisable (GL_TEXTURE_1D);                                   // Disable 1D Textures

        // Outline Code //
        if (outlineDraw)                                             // Check To See If We Want To Draw The Outline
        {
            glEnable (GL_BLEND);                                     // Enable Blending
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);        // Set The Blend Mode

            glPolygonMode (GL_BACK, GL_LINE);                        // Draw Backfacing Polygons As Wireframes
            glLineWidth (outlineWidth);                              // Set The Line Width

            glCullFace (GL_FRONT);                                   // Don't Draw Any Front-Facing Polygons

            glDepthFunc (GL_LEQUAL);                                 // Change The Depth Mode

            glColor3fv (&outlineColor[0]);                           // Set The Outline Color

            glBegin (GL_TRIANGLES);                                  // Tell OpenGL What We Want To Draw
            {
                for (int i = 0; i < polyNum; i++)                    // Loop Through Each Polygon
                {
                    for (int j = 0; j < 3; j++)                      // Loop Through Each Vertex
                    {
                        glVertex3fv (&polyData[i].Verts[j].Pos.x);   // Send The Vertex Position
                    }
                }
            }
            glEnd ();                                                // Tell OpenGL We've Finished

            glDepthFunc (GL_LESS);                                   // Reset The Depth-Testing Mode

            glCullFace (GL_BACK);                                    // Reset The Face To Be Culled

            glPolygonMode (GL_BACK, GL_FILL);                        // Reset Back-Facing Polygon Drawing Mode

            glDisable (GL_BLEND);                                    // Disable Blending
        }


    }
    glPopMatrix();
}

void Player::shoot()
{
    objects.push_back(new Bullet(m_p-Vec(-0.2f,0.0f,0.0f), {0.0f, 0.0f, 0.7f}));
    objects.push_back(new Bullet(m_p+Vec(-0.2f,0.0f,0.0f), {0.0f, 0.0f, 0.7f}));
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
