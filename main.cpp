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

bool   outlineDraw   = true;
bool   outlineSmooth = true;
float  outlineWidth  = 3.0f;
VECTOR lightAngle    = { 1.0f, -1.0f, 0.0f };
#if defined(USE_SHADERS_CG) || defined(USE_SHADERS_ARB)
bool   enableCg      = true;
#endif

list<Bullet*> bullets;
list<Enemy*>  enemies;

Scene  *scene = new Scene;
Model  *model;
Player *player;
Camera *camera;

#if defined(USE_SHADERS_CG)
CGcontext   cgContext;
CGprogram   cgProgram;
CGprofile   cgVertexProfile;
CGparameter position, color, modelViewMatrix, wave, path;
#elif defined(USE_SHADERS_ARB)
GLuint      program_wave, program_cel;
#endif

void quit()
{
#ifdef USE_SHADERS_CG
    cgDestroyContext( cgContext );
#endif
    SDL_Quit();
}

bool load_ARB_program(GLenum target, GLuint *program, const char *filename)
{
    glGenProgramsARB(1, program);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, *program);

    ifstream file(filename, ios::in | ios::binary);
    if(file.fail())
    {
        printf("Unable to open vertex program\n");
        return false;
    }

    file.seekg(0, ios::end);
    int size=file.tellg();
    file.seekg(0, ios::beg);

    unsigned char * program_code=new unsigned char[size];
    file.read(reinterpret_cast<char *>(program_code), size);
    file.close();

    glProgramStringARB(target, GL_PROGRAM_FORMAT_ASCII_ARB, size, program_code);

    delete [] program_code;

    const GLubyte * programErrorString=glGetString(GL_PROGRAM_ERROR_STRING_ARB);

    int errorPos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errorPos);
    if(errorPos!=-1)
    {
        fprintf( stderr, "Program error at position %d in %s\n", errorPos, filename );
        fprintf( stderr, "Error String:\n%s\n", programErrorString );
        return false;
    }

    return true;
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
}

void initGL()
{
    resizeWindow( SCREEN_WIDTH, SCREEN_HEIGHT );

    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Realy Nice perspective calculations

    glClearColor (0.7f, 0.7f, 0.7f, 0.0f);               // Light Grey Background
    glClearDepth (1.0f);                                 // Depth Buffer Setup

    glEnable (GL_DEPTH_TEST);                            // Enable Depth Testing
    glDepthFunc (GL_LESS);                               // The Type Of Depth Test To Do

    glShadeModel (GL_SMOOTH);                            // Enables Smooth Color Shading
    glDisable (GL_LINE_SMOOTH);                          // Initially Disable Line Smoothing

    glEnable(GL_CULL_FACE);

    glDisable(GL_LIGHTING);


    /* Shaders */
#if defined(USE_SHADERS_CG)

    cgContext = cgCreateContext();
    if (cgContext == 0)
    {
        fprintf( stderr, "Failed To Create Cg Context\n" );
        exit(1);
    }
    cgVertexProfile = cgGLGetLatestProfile( CG_GL_VERTEX );
    if (cgVertexProfile == CG_PROFILE_UNKNOWN)
    {
        fprintf( stderr, "Invalid profile type\n" );
        exit(1);
    }
    cgGLSetOptimalOptions( cgVertexProfile );
    cgProgram = cgCreateProgramFromFile( cgContext, CG_SOURCE, "./cg/wave.cg", cgVertexProfile, "main", 0 );
    if (cgProgram == 0)
    {
        fprintf( stderr, "%s \n", cgGetErrorString(cgGetError()) );
        exit(1);
    }
    cgGLLoadProgram(cgProgram);
    position        = cgGetNamedParameter( cgProgram, "IN.position" );
    color           = cgGetNamedParameter( cgProgram, "IN.color" );
    wave            = cgGetNamedParameter( cgProgram, "IN.wave" );
    path            = cgGetNamedParameter( cgProgram, "IN.path" );
    modelViewMatrix = cgGetNamedParameter( cgProgram, "ModelViewProj" );

#elif defined(USE_SHADERS_ARB)

    glEnable(GL_VERTEX_PROGRAM_ARB);

    load_ARB_program(GL_VERTEX_PROGRAM_ARB, &program_wave, "./cg/wave.asm");
    load_ARB_program(GL_VERTEX_PROGRAM_ARB, &program_cel, "./cg/cel.asm");

    glDisable(GL_VERTEX_PROGRAM_ARB);

#endif

    /* Fog */
    GLfloat fogColor[4]= {0.5f, 0.5f, 0.5f, 1.0f};
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

void drawScene()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Clear The Buffers
    glLoadIdentity ();                                           // Reset The Matrix

    if (outlineSmooth)                                           // Check To See If We Want Anti-Aliased Lines
    {
        glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);                 // Use The Good Calculations
        glEnable (GL_LINE_SMOOTH);                               // Enable Anti-Aliasing
    }
    else                                                         // We Don't Want Smooth Lines
        glDisable (GL_LINE_SMOOTH);                              // Disable Anti-Aliasing

    camera->update();
    camera->look();

    player->getInput();
    player->update();

#if defined(USE_SHADERS_CG) || defined(USE_SHADERS_ARB)

    if ( enableCg )
    {
# if defined(USE_SHADERS_CG)
        cgGLSetStateMatrixParameter( modelViewMatrix,
                                     CG_GL_MODELVIEW_PROJECTION_MATRIX,
                                     CG_GL_MATRIX_IDENTITY );
        cgGLEnableProfile( cgVertexProfile );
        cgGLBindProgram( cgProgram );
        cgGLSetParameter4f( color, 0.5f, 1.0f, 0.5f, 1.0f );
# elif defined(USE_SHADERS_ARB)
        glEnable(GL_VERTEX_PROGRAM_ARB);
        glBindProgramARB(GL_VERTEX_PROGRAM_ARB, program_wave);
# endif
    }

#define SIZE 64
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDisable(GL_CULL_FACE);
    //glEnable(GL_LIGHTING);
    MATRIX TmpMatrix;
    glGetFloatv (GL_MODELVIEW_MATRIX, TmpMatrix.Data);
    for (int x = -(SIZE/2); x < (SIZE/2); x++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int z = player->m_p.z-(SIZE); z < player->m_p.z+(SIZE); z++)
        {
# if defined(USE_SHADERS_CG)
            cgGLSetParameter3f(wave, SDL_GetTicks()/1000.0f, 1.0f, 1.0f);
            cgGLSetParameter3f(path,
                               scene->margins[0],
                               scene->margins[1],
                               /*(z-player->m_p.z)*x*LULZFACTOR/100.0f-2.0f*/-4.0f);
# elif defined(USE_SHADERS_ARB)
            glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0,
                                         SDL_GetTicks()/1000.0f, 0.0f, 0.0f, 0.0f);
# endif
            glVertex3f( x+1, -1.0f, z );
            glVertex3f( x,   -1.0f, z );
        }
        glEnd();
    }
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glEnable(GL_CULL_FACE);
    //glDisable(GL_LIGHTING);

    if ( enableCg )
    {
# if defined(USE_SHADERS_CG)
        cgGLDisableProfile( cgVertexProfile );
# elif defined(USE_SHADERS_ARB)
        glDisable(GL_VERTEX_PROGRAM_ARB);
# endif
    }

#endif

    scene->draw();
    player->draw();

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

    /* Draw it to the screen */
    SDL_GL_SwapBuffers();
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
    case SDLK_2:
        outlineSmooth = !outlineSmooth;
        break;
#if defined(USE_SHADERS_CG) || defined(USE_SHADERS_ARB)
    case SDLK_c:
        enableCg = !enableCg;
        break;
#endif
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
                fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError() );
                exit(1);
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
        glRotatef(90.0f,0.0f,0.0f,1.0f);
        drawScene();

        int delay = (1000.0f/fps) - (SDL_GetTicks() - ticks);
        SDL_Delay( delay > 0 ? delay : 1 );

    }
}

void initSDL()
{
    /* this holds some info about our display */
    const SDL_VideoInfo *videoInfo;

    /* initialize SDL */
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        fprintf( stderr, "Video initialization failed: %s\n",
                 SDL_GetError( ) );
        exit(1);
    }

    /* Fetch the video info */
    videoInfo = SDL_GetVideoInfo( );

    if ( !videoInfo )
    {
        fprintf( stderr, "Video query failed: %s\n",
                 SDL_GetError( ) );
        exit(1);
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
        exit(1);
    }
}


int main(int argc, char **argv)
{
    atexit( quit );

    srandom(time(NULL));
    Normalize (lightAngle);

    initSDL();
    initGL();

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
