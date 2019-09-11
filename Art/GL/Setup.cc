//
// file art/gl/setup.cc
// author Maximilien M. Cura
//

#include <Art/Art.hh>
#include <Ynk/Utility.hh>
#include <Ynk/GL/Shader.hh>
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

GLFWwindow * Art::window;
GL::Program * Art::program;

int Art::GLApp_Init (YNK_UNUSED Ynk::App::Stub * application_raw)
{
    Art::Warn ("Initializing...");
    // auto application = reinterpret_cast<YNK_APP_BY_NAME (Test) *> (application_raw);
    auto application = application_raw;
    glfwSetErrorCallback (GLFWHooks::OnErrorFn);

#if OS(APPLE)
    glfwInitHint (GLFW_COCOA_MENUBAR, GLFW_TRUE);
    glfwInitHint (GLFW_COCOA_CHDIR_RESOURCES, GLFW_TRUE);
#endif
    if (!glfwInit ())
        panic ("Could not initialize GLFW!");

    glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint (GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint (GLFW_DECORATED, GLFW_TRUE);
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

    // window = glfwCreateWindow (window_size[0], window_size[1], application->get_name (), NULL, NULL);
    window = glfwCreateWindow (window_size[0], window_size[1], "(DIS)ORDER?", NULL, NULL);

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

    usize load_from_library = (0 == access ("Info.plist", F_OK | R_OK) ? 1 : 0);
    if (load_from_library)
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

    program = new GL::Program ();
    program->attach (vshader);
    program->attach (fshader);
    if (!program->link ()) {
        Art::Error (Fmt::format ("Could not link GL program!\nDumping infolog:\n{}", program->get_info_log ()));
    } else {
        Art::Notify ("GL program linked successfully!");
    }

    Art::Warn ("Dumping error codes:");
    Art::_gl_dump_error_codes ();

    return 0;
}

void Art::GLApp_Teardown ()
{
    Art::Warn ("Tearing down...");
    glfwDestroyWindow (window);
    glfwTerminate ();
}
