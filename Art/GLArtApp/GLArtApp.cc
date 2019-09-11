//
// file art/glartapp/glartapp.cc
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

void Art::GLArtApp (App::Stub * stub)
{
    window_size[0] = 800_i64;
    window_size[1] = 800_i64;

    Art::GLApp_Init (stub);
    Art::GLArtApp_Render ();
    Art::GLApp_Teardown ();
}

void Art::GLArtApp_Render ()
{
    Art::Warn ("Rendering...");

    int width, height;
    glfwGetFramebufferSize (window, &width, &height);
    glClearColor (1.0f, 0.993f, 0.993f, 1.0f);
    glViewport (0, 0, width, height);

    Art::Bristle bristles_fine[5] = {
        Art::Bristle (Art::d3->create_vec ({ 0, -1, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 1, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 1, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, -1, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 0, 0 }), 65336)
    };
    Art::Bristle bristles_square[45] = {
        Art::Bristle (Art::d3->create_vec ({ -7, -7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -7, 7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 7, -7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 7, 7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -7, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, -7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 7, 0, 0 }), 65336),

        Art::Bristle (Art::d3->create_vec ({ -7, -3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -7, 3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 7, -3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 7, 3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -3, -7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -3, 7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 3, -7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 3, 7, 0 }), 65336),

        Art::Bristle (Art::d3->create_vec ({ -3, -3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -3, 3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 3, -3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 3, 3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -3, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, -3, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 3, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -1, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 1, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, -1, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 1, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 0, 0 }), 65336),

        Art::Bristle (Art::d3->create_vec ({ -11, -11, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -11, 11, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 11, -11, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 11, 11, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -11, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 11, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, -11, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 11, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -7, -11, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -7, 11, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 7, -11, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 7, 11, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -11, -7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -11, 7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 11, -7, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 11, 7, 0 }), 65336),
    };
    void * bristles_clone = calloc (45, sizeof (Art::Bristle));
    memcpy (bristles_clone, bristles_square, 45 * sizeof (Art::Bristle));
    Art::Brush brush ((Art::Bristle *)bristles_clone, 45, UX::RGBA (0xFB, 0xFA, 0xF8, 0x01));
    brush.position  = Art::d3->create_vec ({ 0, 200 });
    auto layer_size = Art::iq2->create_vec ({ 400, 400 });

    UX::RGBA pixelbuffer[layer_size[0].inner_ * layer_size[1].inner_],
        pixelbuffer_2[layer_size[0].inner_ * layer_size[1].inner_];

    Art::PaperLayer pl (layer_size, Art::PaperConfiguration ());
    Art::WaterLayer wl (layer_size, &brush);
    Art::TintLayer tl (layer_size, &brush);

    wl._pr_construct (&pl);
    tl._pr_construct (&pl, &wl);
    // wl._pr_ready ();
    wl._pr_accrete (&pl);

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

    GLuint vao, vbo, ebo;

    glGenVertexArrays (1, &vao);
    glGenBuffers (1, &vbo);
    glGenBuffers (1, &ebo);

    glBindVertexArray (vao);

    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (triangles), triangles, GL_DYNAMIC_DRAW);

    glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (GL_FLOAT), (GLvoid *)0);
    glEnableVertexAttribArray (0);

    glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (GL_FLOAT), (GLvoid *)(sizeof (GLfloat) * 6));
    glEnableVertexAttribArray (1);

    glVertexAttribPointer (2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof (GL_FLOAT), (GLvoid *)(sizeof (GLfloat) * 2));
    glEnableVertexAttribArray (2);

    glBindVertexArray (0);

    GLuint txr_wc_layer01, txr_wc_layer02;

    glGenTextures (1, &txr_wc_layer01);
    glBindTexture (GL_TEXTURE_2D, txr_wc_layer01);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)layer_size[0].inner_, (GLsizei)layer_size[1].inner_, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, (GLvoid *)pixelbuffer);
    glGenerateMipmap (GL_TEXTURE_2D);

    glGenTextures (1, &txr_wc_layer02);
    glBindTexture (GL_TEXTURE_2D, txr_wc_layer02);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)layer_size[0].inner_, (GLsizei)layer_size[1].inner_, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, (GLvoid *)pixelbuffer);
    glGenerateMipmap (GL_TEXTURE_2D);

    usize p;
#define BRUSH_RUN            \
    wl._pr_ready ();         \
    wl._pr_run ();           \
    tl._pr_ready (&pl, &wl); \
    tl._pr_run ();           \
    wl._pr_accrete (&pl);    \
    tl._pr_accrete (&pl, &wl);
#define PIXEL_COPY                                                                                                 \
    p = 0;                                                                                                         \
    for (i64 y = 0; y < layer_size[1]; y++) {                                                                      \
        for (i64 x = 0; x < layer_size[0]; x++) {                                                                  \
            if (tl.components[y][x]->tint.quantity)                                                                \
                pixelbuffer[p++] = tl.components[y][x]->tint.color.blend_screen (pl.components[y][x]->tint.color); \
            else                                                                                                   \
                pixelbuffer[p++] = pl.components[y][x]->tint.color;                                                \
        }                                                                                                          \
    }
#define DISTORT(x, y)                                                                         \
    for (usize i = 0; i < brush.num_bristles; i++) {                                          \
        brush.bristles[i].tip_offset[0] += x.inner_ / 2 - (double)arc4random_uniform (x + 1); \
        brush.bristles[i].tip_offset[1] += y.inner_ / 2 - (double)arc4random_uniform (y + 1); \
    }

    for (i32 y = 0; y < 5; y++) {
        memcpy (bristles_clone, bristles_square, 45 * sizeof (Art::Bristle));
        for (i32 j = 0; j < y; j++) {
            DISTORT (y, 0_uz);
        }
        for (i32 x = 0; x < 8; x++) {
            Art::Notify ("Starting new distortion");

            DISTORT (x, (y / 2));

            brush.position[0] = x * 50 + 25;
            brush.position[1] = 300 - (y * 30);
            brush.ink.iargb.green -= 0x18;
            brush.ink.iargb.blue -= 0x10;

            for (i32 i = 0; i < 7; i++) {
                println ("Stage {}/7..", i + 1);
                BRUSH_RUN;
            }

            PIXEL_COPY;

            glClear (GL_COLOR_BUFFER_BIT);
            glUseProgram (program->gl_program ());

            glBindTexture (GL_TEXTURE_2D, txr_wc_layer01);
            glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, layer_size[0], layer_size[1], GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, (void *)pixelbuffer);
            glBindVertexArray (vao);
            glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // glBindTexture (GL_TEXTURE_2D, txr_wc_layer02);
            // glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, layer_size[0], layer_size[1], GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, (void *)pixelbuffer_2);
            // glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glfwSwapBuffers (window);
            glfwPollEvents ();
        }
    }

    Art::Notify ("Done");

    while (!glfwWindowShouldClose (window)) {
        glfwPollEvents ();
    }
}
