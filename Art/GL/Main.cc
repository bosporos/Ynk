//
// file art/gl/main.cc
// author Maximilien M. Cura
//

#include <Art/Art.hh>
#include <Ynk/Utility.hh>
#include <Ynk/GL/Shader.hh>
#include <Ynk/GL/Program.hh>
#include <Ynk/UX/ColorAdapters.hh>

#include <unistd.h>

using namespace Ynk;

YNK_VEILED static const char * vshader_paths[] = {
    "Art/shaders/core-vertex-shader.glsl",
    "Library/shaders/core-vertex-shader.glsl",
};

YNK_VEILED static const char * fshader_paths[] = {
    "Art/shaders/core-fragment-shader.glsl",
    "Library/shaders/core-fragment-shader.glsl",
};

YNK_VEILED static const GLfloat vertices[][5] = {
    { 0.62f, -0.62f, 1.0f, 0.0f, 0.242f },
    { -0.62f, -0.62f, 0.87f, 0.2f, 0.56f },
    { 0.62f, 0.62f, 0.53f, 0.1f, 0.87f },
    { -0.62f, 0.62f, 0.25, 0.25f, 0.54f },
};

int Art::Init (YNK_UNUSED int argc, YNK_UNUSED char ** argv, YNK_UNUSED Ynk::App::Stub * application_raw)
{
    // auto application = reinterpret_cast<YNK_APP_BY_NAME (Test) *> (application_raw);
    auto application = application_raw;
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

    GLFWwindow * window = glfwCreateWindow (window_size[0], window_size[1], application->get_name (), NULL, NULL);

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
