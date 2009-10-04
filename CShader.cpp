#include "main.hpp"
#include "CShader.hpp"


static char *get_file_string(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf( stderr, "Could not open '%s'\n", filename );
        return NULL;
    }

    char* data = NULL;
    struct stat filestats;
    if (stat(filename, &filestats) == 0)
    {
        data = new char[filestats.st_size];
        size_t bytes = fread(data, 1, filestats.st_size, file);
        data[bytes] = 0;
    }
    fclose(file);

    return data;
}

void GLSLShader::load_shader(const char* filename)
{
    char *data = get_file_string(filename);

    if (data == NULL)
    {
        fprintf( stderr, "Could not read '%s'\n", filename );
        return;
    }

    if (program == 0)
    {
        program = glCreateProgramObjectARB();
    }

    unsigned int object = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    glShaderSourceARB(object, 1, (const char**) &data, NULL);
    glCompileShaderARB(object);
    glAttachObjectARB(program, object);
    glDeleteObjectARB(object);

    delete [] data;

    int compiled = 0;
    glGetObjectParameterivARB(object, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

    if (compiled == 0)
    {
        char error[4096] = {0};
        glGetInfoLogARB(object, sizeof(error), NULL, error);
        fprintf( stderr, "Error loading '%s': %s\n", filename, error );
    }
}


GLSLShader::GLSLShader(const char* vertex_file,
                       const char* fragment_file)
{
    // program = 0;

    // load_shader(vertex_file);
    // load_shader(fragment_file);

    // if (program != 0)
    // {
    //     glLinkProgramARB(program);

    //     int linked = 0;
    //     glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &linked);

    //     if (linked == 0)
    //     {
    //         char error[4096] = {0};
    //         glGetInfoLogARB(program, sizeof(error), NULL, error);
    //         printf("Error linking shader: %s\n", error);
    //     }

    //     glUseProgramObjectARB(0);
    // }

    program = 0;

    char error[4096] = {0};
    char* vertex_data = NULL;
    FILE* file = fopen(vertex_file, "r");

    if (file != NULL) /* A technique for loading shader files by Kevin Harris. */
    {
        struct stat filestats;

        if (stat(vertex_file, &filestats) == 0)
        {
            vertex_data = new char[filestats.st_size];

            size_t bytes = fread(vertex_data, 1, filestats.st_size, file);

            vertex_data[bytes] = 0;
        }

        fclose(file);
    }
    else
    {
        if (vertex_file != NULL)
        {
            vertex_data = new char[strlen(vertex_file) + 1];
            strcpy(vertex_data, vertex_file);
        }
    }

    char* fragment_data = NULL;
    file = fopen(fragment_file, "r");

    if (file != NULL) /* A technique for loading shader files by Kevin Harris. */
    {
        struct stat filestats;

        if (stat(fragment_file, &filestats) == 0)
        {
            fragment_data = new char[filestats.st_size];

            size_t bytes = fread(fragment_data, 1, filestats.st_size, file);

            fragment_data[bytes] = 0;
        }

        fclose(file);
    }
    else
    {
        if (fragment_file != NULL)
        {
            fragment_data = new char[strlen(fragment_file) + 1];
            strcpy(fragment_data, fragment_file);
        }
    }

    if (vertex_data != NULL)
    {
        program = glCreateProgramObjectARB();

        unsigned int vertex_object = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        glShaderSourceARB(vertex_object, 1, (const char**) &vertex_data, NULL);
        glCompileShaderARB(vertex_object);
        glAttachObjectARB(program, vertex_object);
        glDeleteObjectARB(vertex_object);

        delete [] vertex_data;

        int compiled = 0;
        glGetObjectParameterivARB(vertex_object, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

        if (compiled == 0)
        {
            glGetInfoLogARB(vertex_object, sizeof(error), NULL, error);
            fprintf( stderr, "%s\n", error );
        }
    }

    if (fragment_data != NULL)
    {
        if (program == 0)
        {
            program = glCreateProgramObjectARB();
        }

        unsigned int fragment_object = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
        glShaderSourceARB(fragment_object, 1, (const char**) &fragment_data, NULL);
        glCompileShaderARB(fragment_object);
        glAttachObjectARB(program, fragment_object);
        glDeleteObjectARB(fragment_object);

        delete [] fragment_data;

        int compiled = 0;
        glGetObjectParameterivARB(fragment_object, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

        if (compiled == 0)
        {
            glGetInfoLogARB(fragment_object, sizeof(error), NULL, error);
            fprintf( stderr, "%s\n", error );
        }
    }

    if (program != 0)
    {
        glLinkProgramARB(program);
        int linked = 0;
        glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &linked);

        if (linked == 0)
        {
            glGetInfoLogARB(program, sizeof(error), NULL, error);
            fprintf( stderr, "%s\n", error );
        }

        glUseProgramObjectARB(0);
    }

}

GLSLShader::~GLSLShader()
{
    glDeleteObjectARB(program);
}

void GLSLShader::Enable()
{
    glUseProgramObjectARB(program);
}

void GLSLShader::Disable()
{
    glUseProgramObjectARB(0);
}

void GLSLShader::Bind(const char* name, const Vec& value)
{
    float v[] = { unfoldVector(value), 0.0f };
    Bind(name, v, 4);
}

void GLSLShader::Bind(const char* name, const float* value, size_t size)
{
    unsigned int location = glGetUniformLocationARB(program, name); /* This is EXPENSIVE
                                                                       and should be avoided. */

    switch (size)
    {
    case 1:  glUniform1fvARB(location, 1, value); break;
    case 2:  glUniform2fvARB(location, 1, value); break;
    case 3:  glUniform3fvARB(location, 1, value); break;
    case 4:  glUniform4fvARB(location, 1, value); break;
    case 9:  glUniformMatrix3fvARB(location, 1, false, value); break;
    case 16: glUniformMatrix4fvARB(location, 1, false, value); break;
    default: break;
    }
}

/* Parse GL_SHADING_LANGUAGE_VERSION and return the major and minor numbers in the supplied integers.
   If it fails for any reason, or GLSL is not supported, major and minor will be set to 0.
   Assumes a valid OpenGL context. This function has been ripped from 3Dlabs Inc. source code. */
void GLSLShader::GetVersion(int& major, int& minor)
{
    int gl_major = 0, gl_minor = 0;
    const char* verstr = (const char*) glGetString(GL_VERSION);

    if ((verstr == NULL) || (sscanf(verstr, "%d.%d", &gl_major, &gl_minor) != 2))
    {
        major = minor = 0; /* If we are here, the test has failed! */
        fprintf(stderr, "Invalid GL_VERSION format! Do you have a valid OpenGL context setup?\n");
    }
    else
    {
        if (gl_major == 1)
        {
            /* GL v1.x can only provide GLSL v1.00 as an extension. */
            const char* extstr = (const char*) glGetString(GL_EXTENSIONS);
            if ((extstr != NULL) && (strstr(extstr, "GL_ARB_shading_language_100") != NULL))
            {
                major = 1; /* If we get here, we are successful. */
                minor = 0;
            }
        }
        else if (gl_major > 1)
        {
            /* GL v2.0 and greater must parse the version string. */
            const char* verstr = (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
            if ((verstr == NULL) || (sscanf(verstr, "%d.%d", &major, &minor) != 2))
            {
                major = minor = 0; /* If we are here, the test has failed! */
                fprintf(stderr, "Invalid GL_SHADING_LANGUAGE_VERSION format!\n");
            }
        }
    }

    return;
}

ARBShader::ARBShader(const char* filename, GLenum target)
{
    char *data = get_file_string(filename);

    if (data == NULL)
    {
        fprintf( stderr, "Could not read '%s'\n", filename );
        return;
    }

    m_target = target;
    glGenProgramsARB(1, &program);
    glBindProgramARB(m_target, program);

    glProgramStringARB(m_target, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(data), data);

    delete [] data;

    const GLubyte * programErrorString=glGetString(GL_PROGRAM_ERROR_STRING_ARB);

    int errorPos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errorPos);
    if(errorPos!=-1)
    {
        die( "Program error on %s:%d: %s",
             filename, errorPos, programErrorString );
    }

    GLenum error = glGetError();
    if (error)
    {
        die( "%s", gluErrorString(error) );
    }
}

ARBShader::~ARBShader()
{
    glDeleteProgramsARB(1, &program);
}

void ARBShader::Enable()
{
    glEnable(m_target);
    glBindProgramARB(m_target, program);
}

void ARBShader::Disable()
{
    glBindProgramARB(m_target, 0);
    glDisable(m_target);
}

void ARBShader::Bind(const GLuint index, const float* value)
{
    glProgramLocalParameter4fvARB(m_target, index, value);
}



#if USE_SHADERS == CG

CGShader::CGShader(const char* filename, const char* function, CGGLenum target)
{
    cgContext = cgCreateContext();
    if (cgContext == 0)
    {
        fprintf( stderr, "Failed To Create Cg Context\n" );
        return;
    }
    cgProfile = cgGLGetLatestProfile( target );
    if (cgProfile == CG_PROFILE_UNKNOWN)
    {
        fprintf( stderr, "Invalid profile type\n" );
        return;
    }
    cgGLSetOptimalOptions( cgProfile );
    cgProgram = cgCreateProgramFromFile( cgContext, CG_SOURCE, filename, cgProfile, function, 0 );
    if (cgProgram == 0)
    {
        fprintf( stderr, "%s \n", cgGetErrorString(cgGetError()) );
        return;
    }
    cgGLLoadProgram(cgProgram);

    modelViewMatrix = cgGetNamedParameter( cgProgram, "ModelViewProj" );
}

CGShader::~CGShader()
{
    cgDestroyContext( cgContext );
}

void CGShader::Enable()
{
    cgGLEnableProfile( cgProfile );
    cgGLBindProgram( cgProgram );
    cgGLSetStateMatrixParameter( modelViewMatrix,
                                 CG_GL_MODELVIEW_PROJECTION_MATRIX,
                                 CG_GL_MATRIX_IDENTITY );
}

void CGShader::Disable()
{
    cgGLDisableProfile( cgProfile );
}


void CGShader::Bind(const char* name, const float* value, size_t size)
{
    CGparameter parameter = cgGetNamedParameter( cgProgram, name );

    switch (size)
    {
    case 1: cgGLSetParameter1fv(parameter, value); break;
    case 2: cgGLSetParameter2fv(parameter, value); break;
    case 3: cgGLSetParameter3fv(parameter, value); break;
    case 4: cgGLSetParameter4fv(parameter, value); break;
    default: break;
    }
}

#endif
