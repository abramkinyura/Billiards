#include <stdlib.h>
#include <string.h>
#include "GLWindow.h"

// static class members
XVisualInfo* GLWindow::gl_visual = 0;
bool GLWindow::gl_swap_flag = false;

// Static methods

static int attributeListDbl[] = {
    GLX_RGBA, GLX_DOUBLEBUFFER,
    //... GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8,
    //... GLX_DEPTH_SIZE, 16,
    GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,
    GLX_DEPTH_SIZE, 1,     // "1" means a maximal nonzero size available
    None
};

static int attributeListSgl[] = {
    GLX_RGBA,
    //... GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8,
    //... GLX_DEPTH_SIZE, 16,
    GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,
    GLX_DEPTH_SIZE, 1,     // "1" means a maximal nonzero size available
    None
};

void GLWindow::selectGLVisual() {
    if (m_Display == 0)
        initX();
    gl_swap_flag = false;
    gl_visual = glXChooseVisual(
        GWindow::m_Display, GWindow::m_Screen, attributeListDbl
    );
    if (gl_visual == 0) {
        gl_visual = glXChooseVisual(
            GWindow::m_Display, GWindow::m_Screen, attributeListSgl
        );
    } else {
        gl_swap_flag = true;
    }
}

void GLWindow::initializeOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glDepthFunc(GL_LEQUAL);

    // Lighting
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    GLfloat pos[4];
    pos[0] = 0.; pos[1] = 0.; pos[2] = 1.; pos[3] = 0.; // Directional light
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    GLfloat light[4];
    light[0] = 0.25; light[1] = 0.25; light[2] = 0.25; light[3] = 1.;
    glLightfv(GL_LIGHT0, GL_AMBIENT, light);

    light[0] = 1.; light[1] = 1.; light[2] = 1.; light[3] = 1.;
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLWindow::terminateOpenGL() {

}

// constructor, destructor
GLWindow::GLWindow():
    GWindow(),
    m_GLXContext(),
    m_GLXContextCreated(false)
{
    memset(&m_GLXContext, 0, sizeof(m_GLXContext));
}

GLWindow::~GLWindow()
{
    if (m_GLXContextCreated) {
        glXDestroyContext(m_Display, m_GLXContext);
        m_GLXContextCreated = false;
    }
}

void GLWindow::createWindow(
    GWindow* parentWindow,              // parent window
    int borderWidth,                    // border width
    unsigned int wndClass,              // or InputOnly, CopyFromParent
    unsigned long attributesValueMask,  // which attributes are defined
    XSetWindowAttributes* attributes    // attributes structure
) {
    if (gl_visual == 0)
        selectGLVisual();

    GWindow::createWindow(
        parentWindow, borderWidth, wndClass, 
        //... (Visual*) gl_visual,
        CopyFromParent,
        attributesValueMask, attributes
    );

    createGLXContext();
    makeCurrent();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // a perspective-view matrix...

    int w = m_IWinRect.width();
    int h = m_IWinRect.height();

    glViewport(0, 0, w, h);

    double depth = m_RWinRect.width();
    if (m_RWinRect.height() > depth)
        depth = m_RWinRect.height();
    glOrtho(
        m_RWinRect.left(), m_RWinRect.right(),  // left, right
        m_RWinRect.bottom(), m_RWinRect.top(),  // bottom, top,
        -10. * depth, 10. * depth               // near, far
    );

    /*
    double aspect = m_RWinRect.width() / m_RWinRect.height();
    gluPerspective(
        30.0,           // Field-of-view angle
        aspect,         // Aspect ratio of view volume
        0.01,           // Distance to near clipping plane
        20.0 * depth    // Distance to far clipping plane
    );
    */
}

void GLWindow::createWindow(
    const I2Rectangle& frameRect,
    const char *title /* = 0 */,
    GWindow* parentWindow /* = 0 */,
    int borderWidth /* = DEFAULT_BORDER_WIDTH */
) {
    if (title == 0) {
        strcpy(m_WindowTitle, "Graphic Window");
    } else {
        strncpy(m_WindowTitle, title, 127);
        m_WindowTitle[127] = 0;
    }

    m_WindowPosition.x = frameRect.left();
    m_WindowPosition.y = frameRect.top();

    m_IWinRect.setLeft(0);
    m_IWinRect.setTop(0);
    m_IWinRect.setWidth(frameRect.width());
    m_IWinRect.setHeight(frameRect.height());

    m_RWinRect.setLeft(0.);
    m_RWinRect.setBottom(0.);
    m_RWinRect.setWidth((double) frameRect.width());
    m_RWinRect.setHeight((double) frameRect.height());

    if (m_IWinRect.width() == 0) {
        m_IWinRect.setWidth(1);
        m_RWinRect.setWidth(1);
    }
    if (m_IWinRect.height() == 0) {
        m_IWinRect.setHeight(1);
        m_RWinRect.setHeight(1);
    }
    m_xcoeff = double(m_IWinRect.width()) / m_RWinRect.width();
    m_ycoeff = double(m_IWinRect.height()) / m_RWinRect.height();
    m_ICurPos = map(m_RCurPos);

    createWindow(parentWindow, borderWidth);
}

void GLWindow::createWindow(
    const I2Rectangle& frameRect,
    const R2Rectangle& coordRect,
    const char *title /* = 0 */,
    GWindow* parentWindow /* = 0 */,
    int borderWidth /* = DEFAULT_BORDER_WIDTH */
) {
    if (title == 0) {
        strcpy(m_WindowTitle, "Graphic Window");
    } else {
        strncpy(m_WindowTitle, title, 127);
        m_WindowTitle[127] = 0;
    }

    m_WindowPosition.x = frameRect.left();
    m_WindowPosition.y = frameRect.top();

    m_IWinRect.setLeft(0);
    m_IWinRect.setTop(0);
    m_IWinRect.setWidth(frameRect.width());
    m_IWinRect.setHeight(frameRect.height());

    m_RWinRect = coordRect;

    if (m_IWinRect.width() == 0) {
        m_IWinRect.setWidth(1);
    }
    if (m_IWinRect.height() == 0) {
        m_IWinRect.setHeight(1);
    }
    //... if (m_RWinRect.width() == 0.) {
    if (fabs(m_RWinRect.width()) <= R2GRAPH_EPSILON) {
        m_RWinRect.setWidth(1.);
    }
    //... if (m_RWinRect.height() == 0.) {
    if (fabs(m_RWinRect.height()) <= R2GRAPH_EPSILON) {
        m_RWinRect.setHeight(1.);
    }
    m_xcoeff = double(m_IWinRect.width()) / m_RWinRect.width();
    m_ycoeff = double(m_IWinRect.height()) / m_RWinRect.height();
    m_ICurPos = map(m_RCurPos);

    createWindow(parentWindow, borderWidth);
}

void GLWindow::createGLXContext() {
    if (gl_visual == 0)
        selectGLVisual();
    m_GLXContext = glXCreateContext(
        m_Display, gl_visual, 0, GL_TRUE
    );
}

void GLWindow::swapBuffers() {
    glFlush();
    if (gl_swap_flag)
        glXSwapBuffers(m_Display, m_Window);
}

void GLWindow::makeCurrent() {
    glXMakeCurrent(m_Display, m_Window, m_GLXContext);
}

void GLWindow::onResize(XEvent& /* event */) {
    glViewport(
        0, 0, m_IWinRect.width(), m_IWinRect.height()
    );

    double depth = m_RWinRect.width();
    if (m_RWinRect.height() > depth)
        depth = m_RWinRect.height();
    glOrtho(
        m_RWinRect.left(), m_RWinRect.right(),  // left, right
        m_RWinRect.bottom(), m_RWinRect.top(),  // bottom, top,
        -10. * depth, 10. * depth               // near, far
    );
}

void GLWindow::destroyWindow() {
    if (m_GLXContextCreated) {
        glXDestroyContext(m_Display, m_GLXContext);
        m_GLXContextCreated = false;
    }
    GWindow::destroyWindow();
}
