//
// A simple example of OpenGL usage over X11:
// draw a tetraedron with a spere on top of it.
//
// The 3D model can be rotated by the mouse movement
// with the left mouse button pressed.
//
// The program is based on the class GLWindow, that presents a
// convenient C++ interface to OpenGL and X11 functions.
//
//                                      Written by V.Borisenko
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GLWindow.h"

//--------------------------------------------------
// Definition of class "MyWindow"
//
class MyWindow: public GLWindow {  // Our main class derived from GLWindow
    GLUquadricObj*  m_Quadric;  // Quadric object used to draw a sphere
    GLfloat         m_Alpha;    // Angle of rotation around vert.axis in degrees
    GLfloat         m_Beta;     // Angle of rotation around hor.axis in degrees
    I2Point         m_MousePos; // Previous position of mouse pointer
public:
    MyWindow():                 // Constructor
        GLWindow(),
        m_Quadric(0),
        m_Alpha(-10.),
        m_Beta(-7.),
        m_MousePos(-1, -1)
    {}

    void render();              // Draw a scene graph

    double f(double x, double y);

    virtual void onExpose(XEvent& event);
    virtual void onKeyPress(XEvent& event);
    virtual void onButtonPress(XEvent& event);
    virtual void onButtonRelease(XEvent& event);
    virtual void onMotionNotify(XEvent& event);
};

//
// Process the Expose event: draw in the window
//
void MyWindow::onExpose(XEvent& event) {
    makeCurrent();

    // Rotate the scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LIGHT0);        // Turn on the first light source

    // Rotate a model
    glRotatef(m_Beta, 1., 0., 0.);
    glRotatef(m_Alpha, 0., 1., 0.);

    render();           // Draw a scene graph

    swapBuffers();
}

//
// Process the KeyPress event: 
// if "q" is pressed, then close the window
//
void MyWindow::onKeyPress(XEvent& event) {
    KeySym key;
    char keyName[256];
    int nameLen = XLookupString(&(event.xkey), keyName, 255, &key, 0);
    printf("KeyPress: keycode=0x%x, state=0x%x, KeySym=0x%x\n",
        event.xkey.keycode, event.xkey.state, (int) key);
    if (nameLen > 0) {
        keyName[nameLen] = 0;
        printf("\"%s\" button pressed.\n", keyName);
        if (keyName[0] == 'q') { // quit => close window
            destroyWindow();
        }
    }
}

// Process mouse click
void MyWindow::onButtonPress(XEvent& event) {
    int x = event.xbutton.x;
    int y = event.xbutton.y;
    int mouseButton = event.xbutton.button;

    // printf("Mouse click: x=%d, y=%d, button=%d\n", x, y, mouseButton);

    m_MousePos.x = (-1); m_MousePos.y = (-1); // Mouse position undefined
}

void MyWindow::onButtonRelease(XEvent& event) {
    m_MousePos.x = (-1); m_MousePos.y = (-1); // Mouse position undefined
}

//
// Process the mouse movement
//
void MyWindow::onMotionNotify(XEvent& event) {
    int x = event.xbutton.x;
    int y = event.xbutton.y;
    if ((event.xbutton.state & Button1Mask) != 0) {
        if (m_MousePos.x >= 0) {
            int dx = x - m_MousePos.x;
            int dy = y - m_MousePos.y;
            if (m_Beta > 100. || m_Beta < (-100.))
                dx = -dx;

            m_Alpha += (GLfloat) dx * 0.1;
            if (fabs(m_Alpha) > 360.)
                m_Alpha -= ((int) m_Alpha / 360) * 360.;

            m_Beta += (GLfloat) dy * 0.1;
            if (fabs(m_Beta) > 360.)
                m_Beta -= ((int) m_Beta / 360) * 360.;

            redraw();
        }
        m_MousePos.x = x; m_MousePos.y = y;
    } else {
        m_MousePos.x = (-1); m_MousePos.y = (-1);   // Undefined
    }
}

//
// Draw a 3D model
//
void MyWindow::render() {
    GLfloat color[4], normal[3];

    glClearColor(0.2, 0.3, 0.5, 1.); // Background color: dark blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    color[0] = 1.0; color[1] = 0.2; color[2] = 0.2;  // Red
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

    // Coordinate system
    glBegin(GL_LINES);
        // X-axis
        normal[0] = 0.;
        normal[1] = 0.;
        normal[2] = 1.;
        glNormal3fv(normal);
        glVertex3f(-10., 0., 0.);
        glVertex3f(10., 0., 0.);

        // Y-axis
        //... normal[0] = 0.;
        //... normal[1] = 0.;
        //... normal[2] = 1.;
        //... glNormal3fv(normal);
        glVertex3f(0., -10., 0.);
        glVertex3f(0., 10., 0.);

        // Z-axis
        normal[0] = 1.;
        normal[1] = 0.;
        normal[2] = 0.;
        glNormal3fv(normal);
        glVertex3f(0., 0., -10.);
        glVertex3f(0., 0., 10.);
    glEnd();

    // Construct a tetraedron
    GLfloat side = sqrt(3.);
    GLfloat height = sqrt(2.);

    // Coordinates of tetraedron vertices:
    GLfloat x0 = 0.,      y0 = (-height/4.), z0 = 1.;
    GLfloat x1 = side/2., y1 = y0,           z1 = (-0.5);
    GLfloat x2 = (-x1),   y2 = y0,           z2 = z1;
    GLfloat x3 = 0.,      y3 = height*3./4., z3 = 0.;

    // Draw a tetraedron
    glBegin(GL_TRIANGLES);
        // Bottom face --------------------------------------
        color[0] = 0.2; color[1] = 1.; color[2] = 0.2;  // Green
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

        // Set a normal to the bottom face
        normal[0] = 0.;
        normal[1] = -1.;
        normal[2] = 0.;
        glNormal3fv(normal);

        // Define vertices of the face
        glVertex3f(x0, y0, z0);
        glVertex3f(x2, y2, z2);
        glVertex3f(x1, y1, z1);

        // Right side face --------------------------------------
        color[0] = 1.; color[1] = 0.2; color[2] = 0.2;  // Red
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

        // Set a normal to the right side face
        normal[0] = (x0 + x1 + x3) / 3.;
        normal[1] = (y0 + y1 + y3) / 3.;
        normal[2] = (z0 + z1 + z3) / 3.;
        glNormal3fv(normal);

        // Define vertices of the face
        glVertex3f(x0, y0, z0);
        glVertex3f(x1, y1, z1);
        glVertex3f(x3, y3, z3);

        // Back face --------------------------------------
        color[0] = 0.9; color[1] = 0.8; color[2] = 0.1; // Yellow
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

        // Define the normal to the face
        normal[0] = (x1 + x2 + x3) / 3.;
        normal[1] = (y1 + y2 + y3) / 3.;
        normal[2] = (z1 + z2 + z3) / 3.;
        glNormal3fv(normal);

        // Define vertices of the face
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
        glVertex3f(x3, y3, z3);

        // Left side face --------------------------------------
        color[0] = 0.2; color[1] = 0.2; color[2] = 1.;  // Blue
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

        // Define the normal to the face
        normal[0] = (x0 + x2 + x3) / 3.;
        normal[1] = (y0 + y2 + y3) / 3.;
        normal[2] = (z0 + z2 + z3) / 3.;
        glNormal3fv(normal);

        // Define vertices of the face
        glVertex3f(x2, y2, z2);
        glVertex3f(x0, y0, z0);
        glVertex3f(x3, y3, z3);
    glEnd();

    // Draw a sphere on top of tetraedron
    if (m_Quadric == 0) {
        m_Quadric = gluNewQuadric();    // Create a Quadric object
        gluQuadricNormals(m_Quadric, GLU_SMOOTH);
    }
    glTranslatef(x3, y3, z3);
    color[0] = 0.2; color[1] = 0.6; color[2] = 0.9;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

    gluSphere(
        m_Quadric, 
        0.3, 
        32,     // Num. slices (similar to lines of longitude)
        16      // Num. stacks (similar to lines of latitude)
    );
}

/////////////////////////////////////////////////////////////
// Main: initialize X, create an instance of MyWindow class,
//       and start the message loop
int main() {
    // Initialize X stuff
    if (!GWindow::initX()) {
        printf("Could not connect to X-server.\n");
        exit(1);
    }

    int width = GWindow::screenMaxX()/2;
    int height = GWindow::screenMaxY()/2;
    double aspect = (double) width / (double) height;

    MyWindow w;
    w.createWindow(
        I2Rectangle(                    // Window frame rectangle:
            I2Point(10, 10),            //     left-top corner,
            width, height               //     width, height
        ),
        R2Rectangle(                    // Window coordinate rectangle
            R2Point(-2. * aspect, -2.), //     left-top corner,
            4. * aspect, 4.             //     width, height
        ),
        "OpenGL Test"                   // Window title
    );
    w.setBackground("lightGray");
    w.makeCurrent();
    GLWindow::initializeOpenGL();

    GWindow::messageLoop();

    GWindow::closeX();
    return 0;
}
