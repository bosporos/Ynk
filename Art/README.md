# The Art

### Base directory

`Art.cc` is the program entry point.

`Art.hh` contains the declarations for the GLFW stuff and everything that's not part of the model.

`Notes.cc` contains some utilities for printing messages to the terminal.

### Subdirectories

`Art/GL` contains the support for the OpenGL App(s).

 - `Art/GL/Errors.cc` contains a couple functions for debugging OpenGL problems.
 - `Art/GL/Hooks.cc` contains callbacks set up with the GLFW windowing system.
 - `Art/GL/Setup.cc` sets up and tears down a windowed OpenGL environment. Documented.

`Art/TUIApp` contains the sources for the debugging TUI.

 - `Art/TUIApp/TUIApp.cc` contains all the sources of the TUI debugging interface.

`Art/GLSimulationApp` contains the sources for the windowed paint simulating app.

 - `Art/GLSimulationApp/GLSimulationApp.cc` runs the real-time watercolor simulation

`Art/GLArtApp` contains the sources for the final generative art piece.

 - `Art/GLArtApp/GlArtApp.cc` runs the semi-dry-brush final art piece ("Distorted progression"). Documented.

`Art/Model` contains the sources for the watercolor model.

 - `Art/Model/Model.hh` contains all the declarations and definitions for the watercolor modeling system. (Moderately) documented.
 - `Art/Model/Brush.cc` contains the code for the `Brush`
 - `Art/Model/PaperLayer.cc` contains the sources for `PaperLayerComponent` and the `PaperLayer`. Trivial.
 - `Art/Model/WaterLayer.cc` contains the sources for `WaterLayerComponent` and the `WaterLayer`. (Moderately) documented.
 - `Art/Model/TintLayer.cc` contains the sources for `TintLayerComponent`, `TintLayer`, and `Tint`. (Moderately) documented.

`Art/PushRelabel` contains the sources of the push-relabel algorithm I implemented.

 - `Art/PushRelabel/PushRelabel.hh` defines the interface and data structures used. Documented.
 - `Art/PushRelabel/PushRelabel.cc` contains the implementations thereof. Documented.

`Art/shaders` contains the GLSL shaders used in the OpenGL rendering pipeline.

Ynk Core Rendering OpenGL GPU program (for the watercolor image):
 - `Art/shaders/core-vertex-shader.glsl` defines the vertex shader
 - `Art/shaders/core-fragment-shader.glsl` defines the fragment shader

They're both just (texture) passthrough shaders, so there's not much to look at there.
