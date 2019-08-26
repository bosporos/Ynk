//
// file art/GL/Errors.cc
// author Maximilien M. Cura
//

#include <Art/Art.hh>
#include <Ynk/Fmt/Fmt.hh>

#include <cstdio>

using namespace Ynk;

void Art::_gl_dump_error_codes ()
{
#define __ynk_gl_error_dump(_E) std::printf (#_E " %i\n", _E);
    __ynk_gl_error_dump (GL_INVALID_ENUM);
    __ynk_gl_error_dump (GL_INVALID_VALUE);
    __ynk_gl_error_dump (GL_INVALID_OPERATION);
    __ynk_gl_error_dump (GL_INVALID_FRAMEBUFFER_OPERATION);
    __ynk_gl_error_dump (GL_OUT_OF_MEMORY);
    __ynk_gl_error_dump (GL_STACK_OVERFLOW);
    __ynk_gl_error_dump (GL_STACK_UNDERFLOW);
#undef __ynk_gl_error_dump
}

void Art::_gl_purge_error_stack (String ctx)
{
    Art::Notify (Fmt::format ("Purging errors: {}...", ctx));
    GLenum gle = GL_NO_ERROR;
    while ((gle = glGetError ()) != GL_NO_ERROR) {
        switch (gle) {
            case GL_INVALID_ENUM:
                println_err ("GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.");
                break;
            case GL_INVALID_VALUE:
                println_err ("GL_INVALID_VALUE: A numeric argument is out of range.");
                break;
            case GL_INVALID_OPERATION:
                println_err ("GL_INVALID_OPERATION: The specified operation is not allowed in the current state.");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                println_err ("GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.");
                break;
            case GL_OUT_OF_MEMORY:
                panic ("GL_OUT_OF_MEMORY: GL ERROR: OOM");
            case GL_STACK_UNDERFLOW:
                println_err ("GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow.");
                break;
            case GL_STACK_OVERFLOW:
                println_err ("GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow. ");
                break;
        }
    }
    println ("Errors purged!");
}
