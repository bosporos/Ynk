//
// file art/Art.cc
// author Maximilien M. Cura

#include <Ynk/App.hh>
#include <Ynk/Num/Integers.hh>
#include <Art/PushRelabel.hh>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <Ynk/Geometry/Space.hh>
#include <Art/Model.hh>

using namespace Ynk;

namespace Art {
    void init (int, char **);
    void reshape (int, int);
    void display ();
}

usize wwidth = 800, wheight = 800;

///! Main function
//!
//! Normally, C/C++ programs start with a `int main(int argc, char ** argv)` function,
//! but when using Ynk, this is the appropriate way to define the entry point into
//! the program.
//!
//! `argc` and `argv` are actually being passed into this function invisibly by Ynk;
//! they may be used herein, it's just part of the C++ magic I had to use to make
//! this work.
YNK_APP (Art)
{
    Space<2, double> * two_space = Space<2, double>::instance (SpaceType::Cartesian);
    two_space->create_vector ({ 0.0, 2.0 }) + two_space->create_vector ({ 1.0, 4.0 });
    println ("Vector: {}", two_space->create_vector ({ 0.0, 1.0 }));
    // Art::init (argc, argv);

    return 0;
}

const usize TINT_MAX;

usize ** hydrosaturation;
usize ** tint_saturation;

void Art::init (int argc, char ** argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize (wwidth, wheight);
    // Leave it to the window system to determine
    glutInitWindowPosition (-1, -1);
    glutCreateWindow ("Ynk::Art");

    glutReshapeFunc (Art::reshape);
    glutDisplayFunc (Art::display);
    // glutIdleFunc (Art::display);

    println ("Initializing");
    glClearColor (0.2039, 0.1804, 0.2157, 1.0);
    glShadeModel (GL_SMOOTH);

    glutMainLoop ();
}

void Art::display ()
{
    println ("Displaying");
    glClear (GL_COLOR_BUFFER_BIT);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D (0.0, (float)wwidth, (float)wheight, 0.0);

    glBegin (GL_QUADS);
    glColor4f (0.8588, 0.3294, 0.3804, 1.0);
    glVertex2i (wwidth / 4, wheight / 8);
    glVertex2i (wwidth / 4, wheight);
    glVertex2i (wwidth.inner_ * 0.4125, wheight);
    glVertex2i (wwidth.inner_ * 0.4125, wheight / 8);
    glVertex2i (wwidth / 2, 0);
    glVertex2i (wwidth / 2, wheight - wheight / 8);
    glVertex2i (wwidth.inner_ * 0.6875, wheight - wheight / 8);
    glVertex2i (wwidth.inner_ * 0.6875, 0);
    glEnd ();

    glutSwapBuffers ();
}

void Art::reshape (int w, int h)
{
    wwidth  = w;
    wheight = h;
    printf ("Reshape %i %i\n", w, h);
    glViewport (0, 0, w, h);
}