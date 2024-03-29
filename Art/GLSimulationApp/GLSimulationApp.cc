//
// file art/glsimulationapp/glsimulationapp.cc
// author Maximilien M. Cura
//

#include <Art/Art.hh>
#include <Ynk/Utility.hh>
#include <Ynk/GL/Shader.hh>
#include <Ynk/GL/Program.hh>
#include <Ynk/UX/ColorAdapters.hh>

#include <Art/PushRelabel/PushRelabel.hh>
#include <Art/Model/Model.hh>

#include <unistd.h>
#include <stdlib.h>

using namespace Ynk;

Vec<2, i64> target (Art::iq2, { 35_i64, 35_i64 });

void Art::GLSimulationApp (App::Stub * stub)
{
    window_size[0] = 800_i64;
    window_size[1] = 800_i64;

    Art::GLApp_Init (stub);
    Art::GLSimulationApp_Render ();
    Art::GLApp_Teardown ();
}

void Art::GLSimulationApp_Render ()
{
    Art::Warn ("Rendering...");

    int width, height;
    glfwGetFramebufferSize (window, &width, &height);
    glClearColor (1.0f, 0.993f, 0.993f, 1.0f);
    glViewport (0, 0, width, height);

    Art::Notify ("Constructing model...");

    Art::Notify ("Bristle creation...");
    Art::Bristle bristles[9] = {
        Art::Bristle (Art::d3->create_vec ({ 0, -3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -3, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 3, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -2, -2, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -2, 2, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 2, -2, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 2, 2, 0 }), 65336),
    };
    Art::Notify ("Brush creation...");
    Art::Brush brush (bristles, 9, UX::RGBA (0xFB, 0xFA, 0xF8, 0x04));
    brush.position  = Art::d3->create_vec ({ 10, 10 });
    auto layer_size = Art::iq2->create_vec ({ 300, 300 });

    Art::Notify (Fmt::format ("Creating pixelbuffer [{}x{} -> {}]...", layer_size[0], layer_size[1], layer_size[0] * layer_size[1]));
    UX::RGBA pixelbuffer[layer_size[0].inner_ * layer_size[1].inner_];

    Art::Notify ("Layer creation...");

    println ("\tPaper layer {}", layer_size);
    Art::PaperLayer pl (layer_size, Art::PaperConfiguration ());

    println ("\tWater layer {}", layer_size);
    Art::WaterLayer wl (layer_size, &brush);

    println ("\tTint layer {}", layer_size);
    Art::TintLayer tl (layer_size, &brush);

    Art::Notify ("Constructing Water layer PR network...");
    wl._pr_construct (&pl);
    Art::Notify ("Done.");

    Art::Notify ("Constructing Tint layer PR network...");
    tl._pr_construct (&pl, &wl);
    Art::Notify ("Done.");

    Art::Notify ("Initializing Water layer PR network...");
    Art::Notify ("Readying...");
    // wl._pr_ready ();
    Art::Notify ("Accreting zero layer (filling H-sigma fields)...");
    wl._pr_accrete (&pl);
    Art::Notify ("Done.");

    Art::Notify ("Running simulation proper...");

    struct
    {
        GLfloat x, y;
        GLfloat r, g, b, a;
        GLfloat s, t;
    } vertices[] = {
        { 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f },
        { 1.f, -1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 0.f },
        { -1.f, -1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 0.f },
        { -1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f, 1.f }
    };
    GLuint triangles[] = {
        0,
        1,
        3,
        1,
        2,
        3,
    };

    Art::Notify ("Binding buffers");

    GLuint vao, vbo, ebo;

    glGenVertexArrays (1, &vao);
    glGenBuffers (1, &vbo);
    glGenBuffers (1, &ebo);

    glBindVertexArray (vao);

    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (triangles), triangles, GL_DYNAMIC_DRAW);

    // Layout:
    //  ya_cvs_vrt_position @ 0
    //  ya_cvs_tex_position @ 1
    //  ya_cvs_color @ 2

    // Vertex position as vec2
    glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (GL_FLOAT), (GLvoid *)0);
    glEnableVertexAttribArray (0);

    // Texture position as vec2
    glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (GL_FLOAT), (GLvoid *)(sizeof (GLfloat) * 6));
    glEnableVertexAttribArray (1);

    // Color as vec4 (alpha)
    glVertexAttribPointer (2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof (GL_FLOAT), (GLvoid *)(sizeof (GLfloat) * 2));
    glEnableVertexAttribArray (2);

    glBindVertexArray (0);

    Art::Notify ("Creating texture");

    GLuint txr;

    glGenTextures (1, &txr);
    glBindTexture (GL_TEXTURE_2D, txr);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)layer_size[0].inner_, (GLsizei)layer_size[1].inner_, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, (GLvoid *)pixelbuffer);
    glGenerateMipmap (GL_TEXTURE_2D);

    Art::Warn ("Starting mainloop");

    _u32 counter = 0;
    for (usize i = 0; i < 360_uz; i++) {
    }

    while (!glfwWindowShouldClose (window)) {
        glClear (GL_COLOR_BUFFER_BIT);

        brush.position[0] = layer_size[0].inner_ / 2.0 + 20.0 * cos (counter / 60.0);
        brush.position[1] = layer_size[1].inner_ / 2.0 + 20.0 * cos (counter / 60.0);

        counter++;
        // brush.ink = UX::hsva ((float)counter / 360.0f, 0.78, 0.87, brush.ink.iargb.alpha);

        // Art::Notify ("WL::PR");
        wl._pr_ready ();
        wl._pr_run ();
        // Art::Notify ("TL::PR");
        tl._pr_ready (&pl, &wl);
        tl._pr_run ();
        // Art::Notify ("Accreting...");
        wl._pr_accrete (&pl);
        tl._pr_accrete (&pl, &wl);

        println ("ITERATION {} FINISHED", counter);

        // Art::Notify ("Updating pixelbuffer");
        usize p = 0;
        for (i64 y = 0; y < layer_size[1]; y++) {
            for (i64 x = 0; x < layer_size[0]; x++) {
                if (tl.components[y][x]->tint.quantity)
                    pixelbuffer[p++] = tl.components[y][x]->tint.color.blend_screen (pl.components[y][x]->tint.color);
                else
                    pixelbuffer[p++] = pl.components[y][x]->tint.color;
                // ;
            }
        }
        glTexSubImage2D (GL_TEXTURE_2D,
                         /* level */
                         0,
                         /* x,y-offsets */
                         0,
                         0,
                         /* w,h */
                         layer_size[0],
                         layer_size[1],
                         /* BGRA + 8/8/8/8-REV */
                         GL_BGRA,
                         GL_UNSIGNED_INT_8_8_8_8,
                         /* pixel data */
                         (void *)pixelbuffer);
        glBindTexture (GL_TEXTURE_2D, txr);

        // Art::Notify ("Rendering...");
        glUseProgram (program->gl_program ());
        glBindVertexArray (vao);
        glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers (window);
        glfwPollEvents ();
    }
}
