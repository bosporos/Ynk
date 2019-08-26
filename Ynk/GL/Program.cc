//
// file ynk/GL/Program.cc
// author Maximilien M. Cura
//

#include <Ynk/GL/Program.hh>
#include <Ynk/Fmt/Fmt.hh>

using namespace Ynk;

GL::Program::Program ()
    : inner { 0 }
{
    this->inner = glCreateProgram ();
}

GL::Program::~Program ()
{
    if (glIsProgram (this->inner) == GL_TRUE) {
        // Detach shaders first
        GLsizei num_shaders;
        GLuint shaders[2];

        glGetAttachedShaders (this->inner, 2, &num_shaders, shaders);
        for (GLsizei i = 0; i < num_shaders; i++) {
            glDetachShader (this->inner, shaders[i]);
        }
        glDeleteProgram (this->inner);
    }
}

void GL::Program::attach (Shader & shader)
{
    if (glIsShader (shader.gl_shader ())) {
        glAttachShader (this->inner, shader.gl_shader ());
    } else {
        println_err ("Error: could not attach shader to program: shader not actually a shader!");
    }
}

bool GL::Program::link ()
{
    glLinkProgram (this->inner);
    GLint prog_linked = 0;
    glGetProgramiv (this->inner, GL_LINK_STATUS, &prog_linked);
    return prog_linked == GL_TRUE;
}

String GL::Program::get_info_log ()
{
    GLint infolog_len = 0;
    glGetProgramiv (this->inner, GL_INFO_LOG_LENGTH, &infolog_len);
    // GL_INFO_LOG_LENGTH includes the \0 terminatro
    if (infolog_len != 0) {
        GLchar * buffer = new GLchar[infolog_len];
        glGetProgramInfoLog (this->inner, infolog_len, &infolog_len, buffer);
        buffer[infolog_len - 1] = '\0';
        return String (buffer);
    } else {
        println_err ("Program has no infolog!");
        return String ();
    }
}

GLuint GL::Program::gl_program ()
{
    return this->inner;
}
