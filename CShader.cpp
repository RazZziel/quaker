#include "main.hpp"
#include "CShader.hpp"


static char *get_file_string(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        return NULL;
    }

    struct stat filestats;
    if (stat(filename, &filestats) != 0)
    {
        die( "Could not read '%s'", filename );
        fclose(file);
        return NULL;
    }

    char* data;
    data = new char[filestats.st_size+1];
    size_t bytes = fread(data, 1, filestats.st_size, file);
    data[bytes] = 0;

    fclose(file);
    return data;
}

void GLSLShader::load_shader(const char* filename, GLenum target)
{
    char *data = get_file_string(filename);

    if (data == NULL)
    {
        return;
    }

    if (program == 0)
    {
        program = glCreateProgramObjectARB();
    }

    unsigned int object = glCreateShaderObjectARB(target);
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
        die2( "%s:0 \n%s", filename, error );
    }
}


GLSLShader::GLSLShader(const char* vertex_file,
                       const char* fragment_file)
{
    program = 0;

    load_shader(vertex_file, GL_VERTEX_SHADER_ARB);
    load_shader(fragment_file, GL_FRAGMENT_SHADER_ARB);

    if (program != 0)
    {
        glLinkProgramARB(program);

        int linked = 0;
        glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &linked);

        if (linked == 0)
        {
            char error[4096] = {0};
            glGetInfoLogARB(program, sizeof(error), NULL, error);
            die( "Error linking shader: %s", error );
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
        die3( "Invalid GL_VERSION format! Do you have a valid OpenGL context setup?" );
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
                die3( "Invalid GL_SHADING_LANGUAGE_VERSION format!" );
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
        die( "Could not read '%s'", filename );
        return;
    }

    m_target = target;
    glGenProgramsARB(1, &program);
    glBindProgramARB(m_target, program);

    glProgramStringARB(m_target, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(data), data);

    delete [] data;

    const char* programErrorString = (char*) glGetString(GL_PROGRAM_ERROR_STRING_ARB);

    int errorPos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errorPos);
    if(errorPos!=-1)
    {
        int line = atoi(programErrorString+5);
        die2( "%s:%d: error: %s", filename, line, programErrorString );
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
        die3( "Failed To Create Cg Context" );
        return;
    }
    cgProfile = cgGLGetLatestProfile( target );
    if (cgProfile == CG_PROFILE_UNKNOWN)
    {
        die3( "Invalid profile type" );
        return;
    }

    cgGLSetOptimalOptions( cgProfile );
    cgProgram = cgCreateProgramFromFile( cgContext, CG_SOURCE, filename, cgProfile, function, 0 );
    if (cgProgram == 0)
    {
        die( "%s", cgGetErrorString(cgGetError()) );
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
