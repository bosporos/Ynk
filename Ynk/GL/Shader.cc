//
// file ynk/gl/Shader.cc
// author Maximilien M. Cura
//

#include <Ynk/GL/Shader.hh>
#include <Ynk/Fmt/Fmt.hh>
#include <Ynk/Panic/Panic.hh>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

using namespace Ynk;

GL::Shader::Shader (GL::ShaderType type)
    : type { type }
    , raw { nullptr }
    , inner { 0 }
{
    this->inner = glCreateShader (static_cast<GLenum> (type));
}

GL::Shader::~Shader ()
{
    if (this->raw != nullptr)
        delete[] this->raw;
    glDeleteShader (this->inner);
}

// Adapted from https://wiki.sei.cmu.edu/confluence/display/c/FIO19-C.+Do+not+use+fseek%28%29+and+ftell%28%29+to+compute+the+size+of+a+regular+file
void GL::Shader::load_from (const char * os_path)
{
    // Doing it Unix-style
    int fd = open (os_path, O_RDONLY);
    if (fd == -1)
        panic ("GL::Shader::load_from(\"{}\"): could not open file!", os_path);
    struct stat fstat_buf;
    if ((fstat (fd, &fstat_buf) != 0))
        panic ("GL::Shader::load_from(\"{}\"): could not fstat file!", os_path);
    if (!S_ISREG (fstat_buf.st_mode))
        panic ("GL::Shader::load_from(\"{}\"): could not open file: not regular (i.e. text) file!", os_path);

    off_t length = fstat_buf.st_size;

    this->raw = new char[length + 1];
    read (fd, static_cast<void *> (this->raw), length);
    this->raw[length] = '\0';
}

void GL::Shader::load (const char * shader_source)
{
    _usize ss_len = strlen (shader_source) + 1;
    this->raw     = new char[ss_len];
    strlcpy (this->raw, shader_source, ss_len);
}

bool GL::Shader::compile ()
{
    // We're just using a single null-terminated string, so pass in NULL for the string_lengths param
    glShaderSource (this->inner, 1, &this->raw, NULL);
    glCompileShader (this->inner);

    GLint shader_compiled = 0;
    glGetShaderiv (this->inner, GL_COMPILE_STATUS, &shader_compiled);
    return shader_compiled == GL_TRUE ? true : false;
}

String GL::Shader::get_info_log ()
{
    GLint infolog_len = 0;
    glGetShaderiv (this->inner, GL_INFO_LOG_LENGTH, &infolog_len);
    // GL_INFO_LOG_LENGTH includes the \0 terminator
    if (infolog_len == 0) {
        GLchar * buffer = new GLchar[infolog_len];
        glGetShaderInfoLog (this->inner, infolog_len, &infolog_len, buffer);
        buffer[infolog_len - 1] = '\0';
        return String (buffer);
    } else {
        println_err ("Shader has no infolog!");
        return String ();
    }
}

GLuint GL::Shader::gl_shader ()
{
    return this->inner;
}
