//
// file ynk/GL/Program.hh
// author Maximilien M. Cura
//

#ifndef __YNK_GL_PROGRAM
#define __YNK_GL_PROGRAM

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <Ynk/GL/Shader.hh>

namespace Ynk::GL {
    struct Program
    {
        GLuint inner;

        Program ();
        ~Program ();
        void attach (Shader & shader);
        bool link ();
        String get_info_log ();

        GLuint gl_program ();
    };
}

#endif /* !@__YNK_GL_PROGRAM */
