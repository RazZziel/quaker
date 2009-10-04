/* Thanks to: Simon Flannery  */

#ifndef _CSHADER_H_
#define _CSHADER_H_

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


#ifndef GL_SHADING_LANGUAGE_VERSION
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#endif



class Shader
{
public:
    Shader() {}
    ~Shader() {}

    virtual void Enable() {}
    virtual void Disable() {}
    virtual void Bind(const char* name, const float* value, size_t size) {}

    void GetVersion(int& major, int& minor);
    unsigned int GetId();
};

class ARBShader : public Shader
{
public:
    ARBShader(const char* filename, GLenum target);
    ~ARBShader();
    void Enable();
    void Disable();
    void Bind(const GLuint index, const float* value);
    void GetVersion(int& major, int& minor) {}
    unsigned int GetId() { return program; }
protected:
    GLuint program;
    GLenum m_target;
};

class GLSLShader : public Shader
{
public:
    GLSLShader(const char* source_vertex, const char* source_fragment);
    ~GLSLShader();
    void Enable();
    void Disable();
    void Bind(const char* name, const float* value, size_t size);
    void Bind(const char* name, const Vec& value);
    void GetVersion(int& major, int& minor);
    unsigned int GetId() { return program; }
protected:
    unsigned int program;
private:
    void load_shader(const char* filename);
};


#if USE_SHADERS == CG

class CGShader : public Shader
{
public:
    CGShader(const char* filename, const char* function, CGGLenum target);
    ~CGShader();
    void Enable();
    void Disable();
    void Bind(const char* name, const float* value, size_t size);
    unsigned int GetId() { return 0; }
protected:
    CGcontext   cgContext;
    CGprogram   cgProgram;
    CGprofile   cgProfile;
    CGparameter modelViewMatrix;
};

#endif


#endif /* _CSHADER_H_ */
