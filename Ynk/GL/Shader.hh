//
// file ynk/GL/Shader.hh
// author Maximilien M. Cura
//

#ifndef __YNK_GL_SHADER
#define __YNK_GL_SHADER

#include <Ynk/String/String.hh>

#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace Ynk::GL {
    enum class ShaderType : GLenum {
        VertexShader   = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER,
    };

    String name_shader_type (ShaderType);
    String name_shader_type (GLenum);

    struct Shader
    {
        ShaderType type;
        char * raw;

        GLuint inner;

        Shader (ShaderType type);
        Shader (Shader && husk);
        ~Shader ();

        void load_from (const char * os_path);
        void load (const char * shader_source);

        bool compile ();
        String get_info_log ();

        GLuint gl_shader ();
    };
}

#endif /* !@__YNK_GL_SHADER */
