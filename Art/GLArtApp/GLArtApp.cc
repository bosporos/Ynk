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

// This is basically the entry point for the GL Art App
void Art::GLArtApp (App::Stub * stub)
{
    // Manually set up the window size, as it's non-resizeable
    window_size[0] = 800_i64;
    window_size[1] = 800_i64;

    // Run the boilerplate code that sets up the window
    Art::GLApp_Init (stub);
    // Run the actual rendering code
    Art::GLArtApp_Render ();
    // Run the teardown boilerplate
    Art::GLApp_Teardown ();
}

void Art::GLArtApp_Render ()
{
    Art::Warn ("Rendering...");

    // Set up GL's viewport, and the clear color (if Processing used setBackground(color) and invokeBackground() instead of background(color))
    int width, height;
    glfwGetFramebufferSize (window, &width, &height);
    glClearColor (1.0f, 0.993f, 0.993f, 1.0f);
    glViewport (0, 0, width, height);

    // Create the square-ish brush used by the generative art piece.
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
    // There is one small complication with the generative art piece.
    // The piece loses its visual consistency and elegance when the brush
    // simply becomes too distorted.
    // Our solution to this was to reset the brush at the beginning of each row,
    // a little bit more distorted each row
    // However, this necessitates that the original state of the brush is stored
    // So, what we do is we create a *copy* of `bristles`, `bristles_clone`, which
    // is actually modified as the algorithm moves along the rows, and is reset
    // to the contents of bristles_square at the beginning of each row

    // calloc(n,s) has a bit of a strange format:
    // It allocates n objects of size s in a contiguous region of memory and sets
    // them all to 0
    void * bristles_clone = calloc (45, sizeof (Art::Bristle));
    // Copy operation
    memcpy (bristles_clone, bristles_square, 45 * sizeof (Art::Bristle));
    // Creating the brush--use a very high-transparency almost-white tint
    Art::Brush brush ((Art::Bristle *)bristles_clone, 45, UX::RGBA (0xFB, 0xFA, 0xF8, 0x01));
    // Set the brush's position to the initial position
    brush.position = Art::d3->create_vec ({ 0, 200 });
    // Use a 400x400 watercolor simulation-- half the resolution of the window,
    // but without some of the strange blurring you get with non-powers-of-2
    auto layer_size = Art::iq2->create_vec ({ 400, 400 });

    // Every iteration, the contents of the TintLayer are combined with the base
    // coloration present in the PaperLayer and composited, then copied into
    // this pixel buffer that is is then copied into an OpenGL texture and drawn
    // onto the screen
    UX::RGBA pixelbuffer[layer_size[0].inner_ * layer_size[1].inner_];
    // pixelbuffer_2[layer_size[0].inner_ * layer_size[1].inner_];

    // Create the component layers
    Art::PaperLayer pl (layer_size, Art::PaperConfiguration ());
    Art::WaterLayer wl (layer_size, &brush);
    Art::TintLayer tl (layer_size, &brush);

    // Set up the water and tint layers
    wl._pr_construct (&pl);
    tl._pr_construct (&pl, &wl);
    // Necessary so that tint layer has a substrate to bind on, I think?
    wl._pr_accrete (&pl);

    // Boilerplate for putting the OpenGL texture generated from the image onto
    // the screen
    // Basically, we put a rectangle onto the screen and tell OpenGL to paint the
    // image onto it

    /// A corner of the rectangle.
    struct
    {
        // X and Y on the screen (clipping coordinates -1..1)
        GLfloat x, y;
        // Base color-- GLclampf so 0..1 (Just set it to 1,1,1,1)
        GLfloat r, g, b, a;
        // Texture coordinates, i.e. where does x,y map to on the texture
        // Note: texture coordinates are 0..1 not -1..1
        GLfloat s, t;
    } vertices[] = {
        { 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f },
        { 1.f, -1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 0.f },
        { -1.f, -1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 0.f },
        { -1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f, 1.f }
    };

    // More boilerplate...
    // OpenGL doesn't really know how to draw quadrilaterals, but it *does* know
    // how to draw triangles, so that's what we're doing here:
    // We turn the 4 points of the rectangle into a pair of triangles that have
    // 2 points in common, and then draw *that* onto the screen
    GLuint triangles[] = {
        0,
        1,
        3,
        1,
        2,
        3,
    };

    // vao = Vertex array object
    // vbo = Vertex buffer object
    // ebo = Element buffer object
    GLuint vao, vbo, ebo;

    // Generate the OpenGL objects
    glGenVertexArrays (1, &vao);
    glGenBuffers (1, &vbo);
    glGenBuffers (1, &ebo);

    // OpenGL is a state machine; you have to tell it which of its many objects/buffers is being operated on
    glBindVertexArray (vao);

    // Basically, we fill vbo and ebo from vertices[] and triangles[], respectively
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    // Point vbo in the general direction of the vertex field
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_DYNAMIC_DRAW);

    // The first parameter to glVertexAttribPointer and glEnableVertexAttribArray is the same as the value of
    // the `layout` property of the input variables in the vertex shader; basically,
    // this tells OpenGL that when the vertex shader asks, it needs to send in data from specific places in vertices[]

    // Set up the x,y
    glVertexAttribPointer (
        /* layout = 0 is ya_cvs_vrt_position */
        0,
        /* x,y = 2 elements */
        2,
        /* x and y will be GLfloats */
        GL_FLOAT,
        GL_FALSE,
        /* each vertex has a total of 8 floating point elements associated with it--
           basically, this tells OpenGL where to grab the next vertex from */
        8 * sizeof (GLfloat),
        /* x and y are the first two elements, e.g. are located at offset 0 within each record */
        (GLvoid *)0);
    glEnableVertexAttribArray (0);

    // Set up s,t (the texture coordinates)
    // Again, layout = 1 is ya_cvs_tex_position
    // s,t is two elements of GLfloat, and total record length is (still) 8*sizeof(GLfloat)
    // What's a bit different here is the last element; since s,t are the last two
    // elements in the vertex record, we have to specify their location (i.e. offset) within the record.
    glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (GLfloat), (GLvoid *)(sizeof (GLfloat) * 6));
    glEnableVertexAttribArray (1);

    // Set up the color, an RGBA of GLfloat
    // layout = 2 is ya_cvs_color
    // r,g,b,a are 4 of GLfloat, total record length is (still) 8*sizeof(GLfloat)
    // Offset within the record is 2
    glVertexAttribPointer (2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof (GLfloat), (GLvoid *)(sizeof (GLfloat) * 2));
    glEnableVertexAttribArray (2);

    // The element buffer object tells OpenGL how to makes triangles out of the vertex buffer
    // We just bind it to triangles[] without any special fanfare
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (triangles), triangles, GL_DYNAMIC_DRAW);

    glBindVertexArray (0);

    // Generate the textures
    GLuint txr_wc_layer01;   //, txr_wc_layer02;

    glGenTextures (1, &txr_wc_layer01);
    glBindTexture (GL_TEXTURE_2D, txr_wc_layer01);
    // Texture wrapping; boilerplate; ignorable
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Point the texture at the image data in pixelbuffer.
    // Mostly deals with telling OpenGL how it can get the data out of the buffer,
    // and how it should be stored internally, etc.
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)layer_size[0].inner_, (GLsizei)layer_size[1].inner_, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, (GLvoid *)pixelbuffer);
    // Generates varied-resolution versions of the texture? I think...
    glGenerateMipmap (GL_TEXTURE_2D);

    // glGenTextures (1, &txr_wc_layer02);
    // glBindTexture (GL_TEXTURE_2D, txr_wc_layer02);
    // glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //
    // glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)layer_size[0].inner_, (GLsizei)layer_size[1].inner_, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, (GLvoid *)pixelbuffer);
    // glGenerateMipmap (GL_TEXTURE_2D);

    usize p;

    // Some macros for simplifying the running processs
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

    // Mainloop

    for (i32 y = 0; y < 5; y++) {   // Five rows
        // Reset the brush to base
        memcpy (bristles_clone, bristles_square, 45 * sizeof (Art::Bristle));
        // Ruffle it up a bit in proportion to the row number
        for (i32 j = 0; j < y; j++) {
            DISTORT (y, 0_uz);
        }

        for (i32 x = 0; x < 8; x++) {   // 8 columns
            Art::Notify ("Starting new distortion");

            // Distort a little bit
            DISTORT (x, (y / 2));

            // Move it along
            brush.position[0] = x * 50 + 25;
            brush.position[1] = 300 - (y * 30);
            // Color movement
            brush.ink.iargb.green -= 0x18;
            brush.ink.iargb.blue -= 0x10;

            // Run 7-iterations of the watercoloring algorithm for each "grid cell"
            for (i32 i = 0; i < 7; i++) {
                println ("Stage {}/7..", i + 1);
                BRUSH_RUN;
            }

            // Copy the tint/paper layer composite into the pixelbuffer
            PIXEL_COPY;

            // Clear the window
            glClear (GL_COLOR_BUFFER_BIT);
            // Tell it to use the shaders we set up earlier
            glUseProgram (program->gl_program ());

            // Tell it where to find the texture with which to draw the rectangle onto the screen
            glBindTexture (GL_TEXTURE_2D, txr_wc_layer01);
            // We need to re-tell OpenGL where to pull the image data from... I think...
            // We might be able to get along without this, but this a trivial call, anyway
            glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, layer_size[0], layer_size[1], GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, (void *)pixelbuffer);
            glBindVertexArray (vao);
            // Draw the texture onto the screen
            glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // glBindTexture (GL_TEXTURE_2D, txr_wc_layer02);
            // glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, layer_size[0], layer_size[1], GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, (void *)pixelbuffer_2);
            // glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Display window and check for user interaction
            glfwSwapBuffers (window);
            glfwPollEvents ();
        }
    }

    Art::Notify ("Done");
}
