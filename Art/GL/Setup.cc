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

// Because the vertex and fragment shaders have to be compiled at runtime (because the binaries will be GPU-specific),
// we have to package them with the executable; right now, the only packaging we support is Artisan's macOS packager,
// which puts the shaders in *.app/Contents/Library/shaders/*.glsl
//
// Unfortunately, the basic CMake/Ninja version doesn't run with those, so *instead*, we include a quick check to figure
// out if we're running from inside an application bundle or not.
YNK_VEILED static const char * vshader_paths[] = {
    "Art/shaders/core-vertex-shader.glsl",
    "Library/shaders/core-vertex-shader.glsl",
};

YNK_VEILED static const char * fshader_paths[] = {
    "Art/shaders/core-fragment-shader.glsl",
    "Library/shaders/core-fragment-shader.glsl",
};

// We have to supply symbol instantiations for the declarations that were given in Art.hh
// This file seems like the most logical place to do so, so we do it here.
GLFWwindow * Art::window;
GL::Program * Art::program;

// Basically creates a window using GLFW; it's a process that lends itself well to lots of
// boilerplate, and I haven't had a chance to go through and make sure everything is packed away
// in a proper object structure
int Art::GLApp_Init (Ynk::App::Stub * application_raw)
{
    Art::Warn ("Initializing...");
    auto application = reinterpret_cast<YNK_APP_BY_NAME (Test) *> (application_raw);
    // Callback the GLFW notifies when it encounters errors
    glfwSetErrorCallback (GLFWHooks::OnErrorFn);

#if OS(APPLE)
    // Special handling of macOS' Cocoa framework;
    glfwInitHint (GLFW_COCOA_MENUBAR, GLFW_TRUE);
    // TODO: Check behaviour of GLFW_COCOA_CHDIR_RESOURCES
    glfwInitHint (GLFW_COCOA_CHDIR_RESOURCES, GLFW_TRUE);
#endif
    if (!glfwInit ())
        panic ("Could not initialize GLFW!");

    // Non-resizable (breaks some of the size-dependent calculations of the art system)
    glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);
    // Make it visible from the get-go (still has to updated with glfwSwapBuffers)
    glfwWindowHint (GLFW_VISIBLE, GLFW_TRUE);
    // Decorations = Close/Minimize buttons and the "Title" bar
    glfwWindowHint (GLFW_DECORATED, GLFW_TRUE);
    // Window should not be in focus until the user clicks on it
    glfwWindowHint (GLFW_FOCUSED, GLFW_FALSE);
    glfwWindowHint (GLFW_FOCUS_ON_SHOW, GLFW_FALSE);

    // Set up the sizing of the color channels (8x8x8x8 R/G/B/A)
    glfwWindowHint (GLFW_RED_BITS, 8);
    glfwWindowHint (GLFW_GREEN_BITS, 8);
    glfwWindowHint (GLFW_BLUE_BITS, 8);
    glfwWindowHint (GLFW_ALPHA_BITS, 8);

    // Use OpenGL API as opposed to Vulkan/Molten
    glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_API);
    // Piggyback on existing context creation APIs
    glfwWindowHint (GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    // Use double-buffering (glfwSwapBuffers; means that the [very slow] animation can run without getting temporary process artefacts)
    glfwWindowHint (GLFW_DOUBLEBUFFER, GLFW_TRUE);

    // Use OpenGL 4.1
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
#if OS(APPLE)
    // Weird bit of cruft for Apple's OpenGL drivers...
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window...
    // Technically, this is Art::window of GLFWwindow *
    window = glfwCreateWindow (window_size[0], window_size[1], application->get_name (), NULL, NULL);

    if (!window) {
        glfwTerminate ();
        panic ("Could not create GLFW window!");
    }

    // Keypress callback (standard boilerplate)
    glfwSetKeyCallback (window, GLFWHooks::OnKeyFn);
    // (Re)size callback (standard boilerplate;) takes care of some window-size monitoring stuff
    glfwSetWindowSizeCallback (window, GLFWHooks::OnWindowSizeFn);

    // Use `window`'s OpenGL context
    glfwMakeContextCurrent (window);
    // Use GLAD to load up the GL framework/whatever it is we're using
    gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress);

    // glfwSwapInterval (1);

    // Show & focus the window
    // Put in just for reusability reasons
    glfwShowWindow (window);
    glfwFocusWindow (window);

    // Check if we're in the macOS application bundle or not; use this to figure out where to load the shaders from.
    // If we're in the macOS bundle, load the shaders from "./Library/shaders"
    // Otherwise, we don't
    usize load_from_library = (0 == access ("Info.plist", F_OK | R_OK) ? 1 : 0);
    if (load_from_library)
        Art::Warn ("Loading from application bundle (macOS only)...");

    // Load the vertex shader
    GL::Shader vshader (GL::ShaderType::VertexShader);
    vshader.load_from (vshader_paths[load_from_library]);
    if (!vshader.compile ()) {
        Art::Error (Fmt::format ("Core vertex shader failed to compile!\nDumping infolog:\n{}", vshader.get_info_log ()));
    } else {
        Art::Notify ("Core vertex shader compiled & loaded!");
    }

    // Load the fragment shader
    GL::Shader fshader (GL::ShaderType::FragmentShader);
    fshader.load_from (fshader_paths[load_from_library]);
    if (!fshader.compile ()) {
        Art::Error (Fmt::format ("Core fragment shader failed to compile!\nDumping infolog:\n{}", fshader.get_info_log ()));
    } else {
        Art::Notify ("Core fragment shader compiled & loaded!");
    }

    // Create a program from the shaders, and get it on the pipelined

    // `program` is Art::program of Ynk::GL::Program
    program = new GL::Program ();
    // Attach the shaders
    program->attach (vshader);
    program->attach (fshader);
    // Link the program
    if (!program->link ()) {
        Art::Error (Fmt::format ("Could not link GL program!\nDumping infolog:\n{}", program->get_info_log ()));
    } else {
        Art::Notify ("GL program linked successfully!");
    }

    Art::Warn ("Dumping error codes:");
    // Dump the GL error codes
    Art::_gl_dump_error_codes ();

    return 0;
}

// Teardown boilerplate (a *lot* simpler...)
// Some memory leaks, so go back and fix those before we merge into babylon/upstream
void Art::GLApp_Teardown ()
{
    Art::Warn ("Tearing down...");

    delete Art::program;

    glfwDestroyWindow (window);
    glfwTerminate ();
}
