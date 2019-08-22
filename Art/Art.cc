//
// file art/Art.cc
// author Maximilien M. Cura
//

#include "linmath.h"
#include <GLAD/glad.h>
#include <OpenGL/glu.h>
#include <glfw/glfw3.h>

#include <Ynk/App.hh>
#include <Ynk/Num/Integers.hh>
#include <Art/PushRelabel.hh>
#include <Ynk/Geometry/Space.hh>
#include <Art/Model.hh>
#include <Art/Color.hh>

using namespace Ynk;

namespace Art::GLFWHooks {
    void OnErrorFn (int, const char *);
    void OnKeyFn (GLFWwindow *, int, int, int, int);
    void OnWindowSizeFn (GLFWwindow *, int, int);
}

namespace Art {
    int Init (int, char **, Ynk::App::Stub *);
    void Render (GLFWwindow *);
}

Space<2, double> * d2 = Space<2, double>::instance (SpaceType::Cartesian);
Space<2, isize> * iz2 = Space<2, isize>::instance (SpaceType::Cartesian);
Vector<2, isize> window_size (iz2, { 800_iz, 800_iz });

///! Main function
//!
//! Normally, C/C++ programs start with a `int main(int argc, char ** argv)` function,
//! but when using Ynk, this is the appropriate way to define the entry point into
//! the program.
//!
//! `argc` and `argv` are actually being passed into this function invisibly by Ynk;
//! they may be used herein, it's just part of the C++ magic I had to use to make
//! this work.
YNK_APP (Test)
{
    Art::Init (argc, argv, application);

    return 0;
}
YNK_LAUNCH_APP (Test);

// static const struct
// {
//     GLfloat x, y;
//     GLfloat r, g, b;
// } vertices[3] = {
//     { -0.6f, -0.4f, 1.0f, 0.f, 0.f },
//     { 0.6f, -0.4f, 0.f, 1.0f, 0.f },
//     { 0.f, 0.6f, 0.f, 0.f, 1.0f }
// };

GLfloat vertices[][5] = {
    { 0.62f, -0.62f, 1.0f, 0.0f, 0.242f },
    { -0.62f, -0.62f, 0.87f, 0.2f, 0.56f },
    { 0.62f, 0.62f, 0.53f, 0.1f, 0.87f },
    { -0.62f, 0.62f, 0.25, 0.25f, 0.54f },
};

static const char * vshader_raw = "#version 410\n"
                                  "layout(location = 0) in vec2 vsynk_position;\n"
                                  "layout(location = 1) in vec3 vsynk_color;\n"
                                  "out vec3 fsynk_color;\n"
                                  "void main()\n"
                                  "{\n"
                                  "  gl_Position = vec4(vsynk_position, 0.0, 1.0);\n"
                                  "  fsynk_color = vsynk_color;\n"
                                  "}\n";
static const char * fshader_raw = "#version 410\n"
                                  "in vec3 fsynk_color;\n"
                                  "out vec4 ynk_FragColor;\n"
                                  "void main()\n"
                                  "{\n"
                                  "  ynk_FragColor  = vec4(fsynk_color, 1.0);\n"
                                  "}\n";

static void dump_shader_log (GLuint);
static void dump_prog_log (GLuint);
static void purge_errors (const char *);
static void dump_error_values ();

int Art::Init (int argc, char ** argv, Ynk::App::Stub * application_raw)
{
    auto application = reinterpret_cast<YNK_APP_BY_NAME (Test) *> (application_raw);
    glfwSetErrorCallback (GLFWHooks::OnErrorFn);

#if OS(APPLE)
    glfwInitHint (GLFW_COCOA_MENUBAR, GLFW_TRUE);
    glfwInitHint (GLFW_COCOA_CHDIR_RESOURCES, GLFW_TRUE);
#endif
    if (!glfwInit ())
        panic ("Could not initialize GLFW!");

    // glfwWindowHint (GLFW_RESIZABLE, GLFW_TRUE);
    // glfwWindowHint (GLFW_VISIBLE, GLFW_TRUE);
    // glfwWindowHint (GLFW_DECORATED, GLFW_FALSE);
    // glfwWindowHint (GLFW_FOCUSED, GLFW_FALSE);
    // glfwWindowHint (GLFW_FOCUS_ON_SHOW, GLFW_FALSE);

    // glfwWindowHint (GLFW_RED_BITS, 8);
    // glfwWindowHint (GLFW_GREEN_BITS, 8);
    // glfwWindowHint (GLFW_BLUE_BITS, 8);
    // glfwWindowHint (GLFW_ALPHA_BITS, 8);

    glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint (GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    glfwWindowHint (GLFW_DOUBLEBUFFER, GLFW_TRUE);

    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow (window_size[0], window_size[1], application->name, NULL, NULL);

    if (!window) {
        glfwTerminate ();
        panic ("Could not create GLFW window!");
    }

    glfwSetKeyCallback (window, GLFWHooks::OnKeyFn);
    glfwSetWindowSizeCallback (window, GLFWHooks::OnWindowSizeFn);

    glfwMakeContextCurrent (window);
    gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval (1);

    glfwShowWindow (window);
    glfwFocusWindow (window);

    GLuint vbuffer, vao, vshader, fshader, prog;
    GLint vpos_loc, vcol_loc;

    vshader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vshader, 1, &vshader_raw, NULL);
    glCompileShader (vshader);

    dump_error_values ();

    GLint vshader_compiled = 0;
    glGetShaderiv (vshader, GL_COMPILE_STATUS, &vshader_compiled);
    if (vshader_compiled == GL_TRUE) {
        println ("Vertex Shader compiled successfully");
    } else {
        println_err ("Vertex Shader did not compile!");

        dump_shader_log (vshader);
    }

    fshader = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fshader, 1, &fshader_raw, NULL);
    glCompileShader (fshader);

    GLint fshader_compiled = 0;
    glGetShaderiv (fshader, GL_COMPILE_STATUS, &fshader_compiled);
    if (fshader_compiled == GL_TRUE) {
        println ("Fragment Shader compiled successfully");
    } else {
        println_err ("Fragment Shader did not compile!");

        dump_shader_log (fshader);
    }

    prog = glCreateProgram ();
    glAttachShader (prog, vshader);
    glAttachShader (prog, fshader);
    glLinkProgram (prog);
    GLint prog_linked = 0;
    glGetProgramiv (prog, GL_LINK_STATUS, &prog_linked);
    if (prog_linked == GL_TRUE) {
        println ("Program linked successfully");
    } else {
        println_err ("Program did not link!");

        dump_prog_log (prog);
    }

    // vpos_loc = glGetUniformLocation (prog, "vPos");
    // vcol_loc = glGetUniformLocation (prog, "vCol");

    glGenVertexArrays (1, &vao);
    glGenBuffers (1, &vbuffer);

    glBindVertexArray (vao);

    glBindBuffer (GL_ARRAY_BUFFER, vbuffer);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (GL_FLOAT), (GLvoid *)0);
    glEnableVertexAttribArray (0);

    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (GL_FLOAT), (GLvoid *)(sizeof (float) * 2));
    glEnableVertexAttribArray (1);

    glBindVertexArray (0);

    int width, height;
    glfwGetFramebufferSize (window, &width, &height);
    glClearColor (1.0f, 0.993f, 0.993f, 1.0f);
    glViewport (0, 0, width, height);

    while (!glfwWindowShouldClose (window)) {
        glClear (GL_COLOR_BUFFER_BIT);
        // glMatrixMode (GL_PROJECTION);
        // glLoadIdentity ();

        glUseProgram (prog);
        glBindVertexArray (vao);
        glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray (0);

        glfwSwapBuffers (window);
        glfwPollEvents ();
    }

    glfwDestroyWindow (window);
    glfwTerminate ();

    return 0;
}

void Art::Render (GLFWwindow * window)
{}

void Art::GLFWHooks::OnErrorFn (int code, const char * text)
{
    panic ("GLFW Error [{}]: {}", code, text);
}

void Art::GLFWHooks::OnKeyFn (GLFWwindow *, int, int, int, int)
{
}

void Art::GLFWHooks::OnWindowSizeFn (GLFWwindow *, int new_width, int new_height)
{
    window_size[0] = new_width;
    window_size[1] = new_height;

    println ("Resizing window to {} {}", new_width, new_height);
}

void dump_shader_log (GLuint shader)
{
    GLint max_length = 0;
    glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &max_length);
    GLchar * buffer = new GLchar[max_length + 1];
    glGetShaderInfoLog (shader, max_length, &max_length, buffer);

    println_err ("SHADER LOG:");
    std::printf ("%s", buffer);
    delete[] buffer;
}

void dump_prog_log (GLuint prog)
{
    GLint max_length = 0;
    glGetProgramiv (prog, GL_INFO_LOG_LENGTH, &max_length);
    GLchar * buffer = new GLchar[max_length + 1];
    glGetProgramInfoLog (prog, max_length, &max_length, buffer);

    println_err ("PROGRAM LOG:");
    std::printf ("%s", buffer);
    delete[] buffer;
}

void purge_errors (const char * ctx)
{
    std::printf ("Purging errors: %s...\n", ctx);
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

void dump_error_values ()
{
#define __ynk_gl_error_dump(_E) std::printf (#_E " %i\n", _E);
    __ynk_gl_error_dump (GL_INVALID_ENUM);
    __ynk_gl_error_dump (GL_INVALID_VALUE);
    __ynk_gl_error_dump (GL_INVALID_OPERATION);
    __ynk_gl_error_dump (GL_INVALID_FRAMEBUFFER_OPERATION);
    __ynk_gl_error_dump (GL_OUT_OF_MEMORY);
    __ynk_gl_error_dump (GL_STACK_OVERFLOW);
    __ynk_gl_error_dump (GL_STACK_UNDERFLOW);
}
