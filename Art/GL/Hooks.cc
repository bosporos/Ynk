//
// file art/GL/Hooks.cc
// author Maximilien M. Cura
//

#include <Art/Art.hh>
#include <Ynk/Fmt/Fmt.hh>

void Art::GLFWHooks::OnErrorFn (int code, const char * text)
{
    Art::Error (Ynk::Fmt::format ("GLFW[{}]: {}", code, text));
    panic ("GLFW Error!");
}

void Art::GLFWHooks::OnKeyFn (GLFWwindow *, int, int, int, int)
{
}

void Art::GLFWHooks::OnWindowSizeFn (GLFWwindow *, int new_width, int new_height)
{
    window_size[0] = new_width;
    window_size[1] = new_height;

    Ynk::println ("Resizing window to {} {}", new_width, new_height);
}
