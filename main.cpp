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

#include "main.hpp"


using namespace std;

SDL_Surface *surface;

int videoFlags;
int w_width, w_height;

bool   outlineDraw   = true;
float  outlineWidth  = 3.0f;
Vec    lightAngle(0.0f, 0.0f, 0.0f);
bool   doShaders     = true;

list<Bullet*> bullets;
list<Enemy*>  enemies;

Scene  *scene = new Scene;
Model  *model;
Player *player;
Camera *camera;

#if USE_SHADERS == CG
CGShader    *program_wave;
#elif USE_SHADERS == ARB
ARBShader   *program_wave;
ARBShader   *program_cel;
#elif USE_SHADERS == GLSL
GLSLShader  *program_wave;
GLSLShader  *program_cel;
#endif

void quit()
{
    SDL_Quit();
}

void resizeWindow(int width, int height)
{
    if (height==0)
        height = 1;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);

    w_width = width;
    w_height = height;
}

void initGL()
{
    resizeWindow( SCREEN_WIDTH, SCREEN_HEIGHT );

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Realy Nice perspective calculations

    glClearColor(0.7f, 0.7f, 0.7f, 0.0f);                // Light Grey Background
    glClearDepth(1.0f);                                  // Depth Buffer Setup

    glEnable(GL_DEPTH_TEST);                             // Enable Depth Testing
    glDepthFunc(GL_LESS);                                // The Type Of Depth Test To Do

    glShadeModel(GL_SMOOTH);                             // Enables Smooth Color Shading

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);

    glEnable(GL_CULL_FACE);

    glDisable(GL_LIGHTING);


    /* Shaders */
#if USE_SHADERS == CG
    program_wave = new CGShader( "cg/wave.cg", "main", CG_GL_VERTEX );
#elif USE_SHADERS == ARB
    //program_wave = new ARBShader( "cg/wave.asm", GL_VERTEX_PROGRAM_ARB );
    program_wave = new ARBShader( "cg/wave.asm2", GL_VERTEX_PROGRAM_ARB );
    //program_wave = new ARBShader( "cg/wave.arb", GL_VERTEX_PROGRAM_ARB );
    program_cel = new ARBShader( "cg/cel.asm", GL_VERTEX_PROGRAM_ARB );
#elif USE_SHADERS == GLSL
    program_wave = new GLSLShader( "cg/wave_vertex.glsl", NULL );
    // program_cel = new GLSLShader( "cg/cel_vertex.glsl", "cg/cel_fragment.glsl" );
    program_cel = new GLSLShader( NULL, "cg/blur.glsl" );
#endif

    /* Fog */
    GLfloat fogColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
    glClearColor(0.5f,0.5f,0.5f,1.0f);
    glFogi(GL_FOG_MODE, GL_LINEAR);     // Fog mode
    glFogfv(GL_FOG_COLOR, fogColor);    // Set fog color
    glFogf(GL_FOG_DENSITY, 0.35f);      // Fog density
    glHint(GL_FOG_HINT, GL_DONT_CARE);  // Fog hint value
    glFogf(GL_FOG_START, 5.0f);         // Fog start depth
    glFogf(GL_FOG_END, 30.0f);          // Fog end depth
    glEnable(GL_FOG);
}


void add_enemy()
{
    enemies.push_back( (new Enemy(
                            { ((float)random() / (float)RAND_MAX)*scene->margins[1]*2.0f - scene->margins[1],
                                    player->m_p.y,
                                    player->m_p.z + 10.0f + (((float)random()/(float)RAND_MAX)*50.0f) },
                            {0.0f, 0.0f, -0.2f+(float)random()/(float)RAND_MAX*-0.8f} ))->setModel(model) );
}

void drawWaves()
{
    static GLuint dl = 0;
    const int size = 64;

    glPushMatrix();
    {
        glTranslatef( 0.0f, 0.0f, player->m_p.z );

        if ( doShaders )
        {
#if USE_SHADERS == CG or USE_SHADERS == ARB or USE_SHADERS == GLSL
            program_wave->Enable();
# if USE_SHADERS == CG
            float color[] = { 0.5f, 1.0f, 0.5f, 1.0f };
            float wave[] = { SDL_GetTicks()/1000.0f };
            float path[] = { scene->margins[0], scene->margins[1], -4.0f };
            program_wave->Bind( "IN.color", color, 4 );
            program_wave->Bind( "IN.wave", wave, 1 );
            program_wave->Bind( "IN.path", path, 3 );
# elif USE_SHADERS == ARB
            // program_wave->Bind( 0, (float[4]) {0.5f, 1.0f, 0.5f, 1.0f} );
            // program_wave->Bind( 1, (float[4]) {SDL_GetTicks()/1000.0f} );
            // program_wave->Bind( 2, (float[4]) {scene->margins[0], scene->margins[1], -4.0f} );
            float p0[] = { SDL_GetTicks()/1000.0f,
                           sinf(SDL_GetTicks()/1000.0f),
                           cosf(SDL_GetTicks()/1000.0f),
                           tanf(SDL_GetTicks()/1000.0f) };
            float p1[] = { scene->margins[0], scene->margins[1], -4.0f };
            program_wave->Bind( 0, p0 );
            program_wave->Bind( 1, p1 );
#elif USE_SHADERS == GLSL
            float wave[] = { SDL_GetTicks()/1000.0f };
            float path[] = { scene->margins[0], scene->margins[1], -4.0f };
            program_wave->Bind( "wave", wave, 1 );
            program_wave->Bind( "path", path, 3 );
# endif
#endif
        }

        if (!dl)
        {
            dl = glGenLists(1);
            glNewList( dl, GL_COMPILE );
            // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            // glDisable(GL_CULL_FACE);
            // glEnable(GL_LIGHTING);

            for (int x = -(size/2); x < (size/2); x++)
            {
                glBegin(GL_TRIANGLE_STRIP);
                for (float z = -(size); z < (size); z++)
                {
                    glVertex3f( x+1, -1.0f, z );
                    glVertex3f( x,   -1.0f, z );
                }
                glEnd();
            }

            // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            // glEnable(GL_CULL_FACE);
            // glDisable(GL_LIGHTING);

            glEndList();
        }

        glCallList( dl );

        if ( doShaders )
        {
#if USE_SHADERS == CG or USE_SHADERS == ARB
            program_wave->Disable();
#endif
        }

    }
    glPopMatrix();
}


void drawFPS(float fps)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w_width, 0, w_height);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(1.0f,1.0f);
        char s[32], *c=s;
        sprintf(s, "%.2f FPS", fps);
        while (*c)
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, *c++ );
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void drawScene()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity ();

    camera->update();
    camera->look();

    player->getInput();
    player->update();
    lightAngle = player->m_p + Vec(3.0f, 3.0f, -0.1f);

    drawWaves();
    scene->draw();
    player->draw();

    checkGlError();

    for ( list<Bullet*>::iterator it = bullets.begin();
          it != bullets.end(); )
    {
        if ( (*it)->m_p.z > (player->m_p.z + 20.0f) )
        {
            it = bullets.erase(it);
        }
        else
        {
            (*it)->update();
            (*it)->draw();

            for ( list<Enemy*>::iterator it2 = enemies.begin();
                  it2 != enemies.end(); )
            {
                if ( ((*it)->m_p - (*it2)->m_p).module() < 0.5f )
                {
                    it = bullets.erase(it);
                    it2 = enemies.erase(it2);
                    add_enemy();
                }
                else
                {
                    ++it2;
                }
            }
            if (it != bullets.end())
                ++it;
        }
    }

    for ( list<Enemy*>::iterator it = enemies.begin();
          it != enemies.end(); )
    {
        if ( (*it)->m_p.z < (player->m_p.z - 4.0f) )
        {
            it = enemies.erase(it);
            add_enemy();
        }
        else
        {
            (*it)->update();
            (*it)->draw();
            ++it;
        }
    }

    checkGlError();
}

void handleKeyPress( SDL_keysym *keysym )
{
    switch ( keysym->sym )
    {
    case SDLK_ESCAPE:
        exit(0);
        break;
    case SDLK_F1:
        SDL_WM_ToggleFullScreen( surface );
        break;
    case SDLK_1:
        outlineDraw = !outlineDraw;
        break;
    case SDLK_c:
        doShaders = !doShaders;
        break;
    default:
        break;
    }

    return;
}

void handleEvents()
{
    SDL_Event event;

    while ( SDL_PollEvent( &event ) )
    {
        switch( event.type )
        {
        case SDL_VIDEORESIZE:
            surface = SDL_SetVideoMode( event.resize.w,
                                        event.resize.h,
                                        16, videoFlags );
            if ( !surface )
            {
                die( "Could not get a surface after resize: %s", SDL_GetError() );
            }
            resizeWindow( event.resize.w, event.resize.h );
            break;
        case SDL_KEYDOWN:
            handleKeyPress( &event.key.keysym );
            break;
        case SDL_QUIT:
            exit(0);
            break;
        default:
            break;
        }
    }
}

void main_loop()
{
    Uint32 const fps = 25;
    Uint32 ticks;

    while( true )
    {
        ticks = SDL_GetTicks();

        handleEvents();

        drawScene();
        int delay = (1000.0f/fps) - (SDL_GetTicks() - ticks);
        if (delay>0)
            SDL_Delay( delay );

        drawFPS(1000.0f/((SDL_GetTicks() - ticks)?:1));
        SDL_GL_SwapBuffers();
    }
}

void initSDL()
{
    /* this holds some info about our display */
    const SDL_VideoInfo *videoInfo;

    /* initialize SDL */
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        die( "Video initialization failed: %s", SDL_GetError() );
    }

    /* Fetch the video info */
    videoInfo = SDL_GetVideoInfo( );

    if ( !videoInfo )
    {
        die( "Video query failed: %s", SDL_GetError() );
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
        die( "Video mode set failed: %s", SDL_GetError() );
    }
}

int main(int argc, char **argv)
{
    atexit( quit );

    srandom(time(NULL));

    initSDL();
    initGL();
    glutInit(&argc, argv);

    model = new Model("Data/Shader.txt", "Data/Model.txt");
    player = (new Player())->setModel(model);
    camera = (new Camera(player))->setPos(Vec(0.0f, 4.0f, 0.0f));

    for (int i=0; i<20; i++)
    {
        add_enemy();
    }

    main_loop();

    return 0;
}
