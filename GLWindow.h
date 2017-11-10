//
// File "GLWindow.h"
//
// The definition of the class GLWindow,
// that provides a C++ interface to OpenGL and X11 functions
//
// Based on the class GWindow that gives a C++ interface
// to X11 graphics
//
// Written by V. Borisenko
//
#ifndef _GL_WINDOW_H
#define _GL_WINDOW_H

#include "GWindow/gwindow.h"

#include <GL/glx.h> 
#include <GL/gl.h> 
#include <GL/glu.h>

class GLWindow: public GWindow {
    // Data members
public:
    static XVisualInfo* gl_visual;
    static bool         gl_swap_flag;

    GLXContext          m_GLXContext;
    bool                m_GLXContextCreated;

    // Methods
private:
    static void selectGLVisual();

public:
    GLWindow();
    virtual ~GLWindow();

    static void initializeOpenGL();
    static void terminateOpenGL();

    void createWindow(
        GWindow* parentWindow = 0,              // parent window
        int borderWidth = DEFAULT_BORDER_WIDTH, // border width
        unsigned int wndClass = InputOutput,    // or InputOnly, CopyFromParent
        unsigned long attributesValueMask = 0,  // which attributes are defined
        XSetWindowAttributes* attributes = 0    // attributes structure
    );
    void createWindow(
        const I2Rectangle& frameRect, 
        const char *title = 0,
        GWindow* parentWindow = 0,
        int borderWidth = DEFAULT_BORDER_WIDTH
    );
    void createWindow(
        const I2Rectangle& frameRect, 
        const R2Rectangle& coordRect,
        const char *title = 0,
        GWindow* parentWindow = 0,
        int borderWidth = DEFAULT_BORDER_WIDTH
    );

    void createGLXContext(); // Called from createWindow

    virtual void destroyWindow();

    void makeCurrent();
    void swapBuffers();

    // X-Event processing
    virtual void onResize(XEvent& event);
};

#endif /* _GL_WINDOW_H */
