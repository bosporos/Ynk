//
// file art/Art.cc
// author Maximilien M. Cura
//

#include <Art/Art.hh>

#include <Art/PushRelabel.hh>

#include <Art/Model/Model.hh>

#include <Ynk/UX/ColorAdapters.hh>

#include <Ynk/GL/Shader.hh>
#include <Ynk/GL/Program.hh>

#include <unistd.h>

using namespace Ynk;

Space<3, double> * Art::d3 = Space<3, double>::instance (SpaceType::Cartesian);
Space<2, i64> * Art::iq2   = Space<2, i64>::instance (SpaceType::Cartesian);
Vec<2, i64> Art::window_size (Art::iq2, { 800_i64, 800_i64 });

void Simulation ();

const char * vshader_paths[] = {
    "Art/shaders/core-vertex-shader.glsl",
    "Library/shaders/core-vertex-shader.glsl",
};

const char * fshader_paths[] = {
    "Art/shaders/core-fragment-shader.glsl",
    "Library/shaders/core-fragment-shader.glsl",
};

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
    // Simulation();

    return 0;
}

YNK_LAUNCH_APP (Test);

GLfloat vertices[][5] = {
    { 0.62f, -0.62f, 1.0f, 0.0f, 0.242f },
    { -0.62f, -0.62f, 0.87f, 0.2f, 0.56f },
    { 0.62f, 0.62f, 0.53f, 0.1f, 0.87f },
    { -0.62f, 0.62f, 0.25, 0.25f, 0.54f },
};

YNK_USED static void dump_prog_log (GLuint);
YNK_USED static void purge_errors (const char *);
YNK_USED static void dump_error_values ();

int Art::Init (YNK_UNUSED int argc, YNK_UNUSED char ** argv, YNK_UNUSED Ynk::App::Stub * application_raw)
{
    auto application = reinterpret_cast<YNK_APP_BY_NAME (Test) *> (application_raw);
    glfwSetErrorCallback (GLFWHooks::OnErrorFn);

#if OS(APPLE)
    glfwInitHint (GLFW_COCOA_MENUBAR, GLFW_TRUE);
    glfwInitHint (GLFW_COCOA_CHDIR_RESOURCES, GLFW_TRUE);
#endif
    if (!glfwInit ())
        panic ("Could not initialize GLFW!");

    glfwWindowHint (GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint (GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint (GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint (GLFW_FOCUSED, GLFW_FALSE);
    glfwWindowHint (GLFW_FOCUS_ON_SHOW, GLFW_FALSE);

    glfwWindowHint (GLFW_RED_BITS, 8);
    glfwWindowHint (GLFW_GREEN_BITS, 8);
    glfwWindowHint (GLFW_BLUE_BITS, 8);
    glfwWindowHint (GLFW_ALPHA_BITS, 8);

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

    GLuint vbuffer, vao;

    usize load_from_library = (0 == access ("Info.plist", F_OK | R_OK) ? 1 : 0);
    Art::Warn ("Loading from application bundle (macOS only)...");

    GL::Shader vshader (GL::ShaderType::VertexShader);
    vshader.load_from (vshader_paths[load_from_library]);
    if (!vshader.compile ()) {
        Art::Error (Fmt::format ("Core vertex shader failed to compile!\nDumping infolog:\n{}", vshader.get_info_log ()));
    } else {
        Art::Notify ("Core vertex shader compiled & loaded!");
    }

    GL::Shader fshader (GL::ShaderType::FragmentShader);
    fshader.load_from (fshader_paths[load_from_library]);
    if (!fshader.compile ()) {
        Art::Error (Fmt::format ("Core fragment shader failed to compile!\nDumping infolog:\n{}", fshader.get_info_log ()));
    } else {
        Art::Notify ("Core fragment shader compiled & loaded!");
    }

    GL::Program program;
    program.attach (vshader);
    program.attach (fshader);
    if (!program.link ()) {
        Art::Error (Fmt::format ("Could not link GL program!\nDumping infolog:\n{}", program.get_info_log ()));
    } else {
        Art::Notify ("GL program linked successfully!");
    }

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

        glUseProgram (program.gl_program ());
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

void Art::Render (YNK_UNUSED GLFWwindow * window)
{}

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

void Simulation ()
{
    Art::Bristle bristles[5] = {
        Art::Bristle (Art::d3->create_vec ({ 0, -1, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -1, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 1, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 1, 0 }), 65336)
    };
    Art::Brush brush (
        bristles,
        5,
        UX::RGBA (0x9f, 0xa0, 0xff, 0xff));
    brush.position += Art::d3->create_vec ({ 0, 0, 0 });

    auto layer_size = Art::iq2->create_vec ({ 25, 25 });

    Art::PaperLayer pl (layer_size, Art::PaperConfiguration ());
    Art::WaterLayer wl (layer_size, &brush);

    Art::Notify ("Constructing PR network...");
    wl._pr_construct (&pl);
    Art::Notify ("Constructing PR network... done");

    Art::Notify ("Initializing PR network...");
    wl._pr_ready ();
    wl._pr_accrete (&pl);
    Art::Notify ("Initializing PR network... done");

    getchar ();

    for (int i = 0; i < 1000; i++) {
        if (!arc4random_uniform (4)) {
            brush.position += Art::d3->create_vec ({ 2.0 - arc4random_uniform (4),
                                                     2.0 - arc4random_uniform (4),
                                                     0 });
        }
        print ("\x1b[1;1H");
        println ("BRUSH {} {} {}", brush.position[0], brush.position[1], brush.position[2]);
        wl._pr_ready ();
        wl._pr_run ();

        wl._pr_accrete (&pl);
        for (i64 y = 0; y < layer_size[1]; y++) {
            for (i64 x = 0; x < layer_size[0]; x++) {
                print ("{}#{}",
                       UX::ANSI (UX::hsva (354, (float)wl.components[y][x]->hydrosaturation / 100.0, 0.86, 1)),
                       UX::ANSIRst ());
            }
            println ("");
        }
    }
}
