//
// A simple example of OpenGL usage over X11:
// draw a graph of function z=f(x,y)
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
        m_Alpha(0.),
        m_Beta(0.),
        m_MousePos(-1, -1)
    {}

    double f(double x, double y);       // Draw a scene graph
    void gradient(double x, double y, GLfloat grad[3]);
    void render();                      // Draw a scene graph
    void setColor(double z);

    virtual void onExpose(XEvent& event);
    virtual void onKeyPress(XEvent& event);
    virtual void onButtonPress(XEvent& event);
    virtual void onButtonRelease(XEvent& event);
    virtual void onMotionNotify(XEvent& event);
};

double MyWindow::f(double x, double y) {
    double r = sqrt(x*x+y*y);
    return (
        0.5*cos(6.*r) /
        (1. + r*r)
    );
}

void MyWindow::gradient(double x, double y, GLfloat grad[3]) {
    // z = f(x, y)
    // F(x, y, z) = z - f(x, y)
    double dx = 0.0001;
    double dy = 0.0001;
    double dFx = -(f(x+dx, y) - f(x-dx, y)) / (2.*dx);
    double dFy = -(f(x, y+dy) - f(x, y-dy)) / (2.*dy);
    double dFz = 1.;
    double len = sqrt(dFx*dFx + dFy*dFy + dFz*dFz);
    if (len <= 0.)
        len = 0.;
    grad[0] = (GLfloat)(dFx / len);
    grad[1] = (GLfloat)(dFy / len);
    grad[2] = (GLfloat)(dFz / len);
}

void MyWindow::setColor(double z) {
    GLfloat color[3];
    double c = 0.5 + atan(2.*z) / M_PI;

    color[0] = 0.1 + c * 0.9;   // Red
    color[1] = 0.2 + c * 0.4;   // Green
    color[2] = 1.0 - c * 0.8;;  // Blue
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
}

//
// Process the Expose event: draw in the window
//
void MyWindow::onExpose(XEvent& /* event */) {
    makeCurrent();

    // Rotate the scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LIGHT0);        // Turn on the first light source

    // Rotate a model to have a cabinet projection
    glRotatef(30., 1., 0., 0.);
    glRotatef(-30., 0., 1., 0.);

    // Go to mathematical coord. system: Z-axis goes up
    glRotatef(-90., 1., 0., 0.);

    // Rotate a model
    glRotatef(m_Beta, 1., 0., 0.);
    glRotatef(m_Alpha, 0., 0., 1.);

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
        } else {
            // Set initial position.
            m_Alpha = 0.;
            m_Beta = 0.;
            redraw();
        }
    }
}

// Process mouse click
void MyWindow::onButtonPress(XEvent& event) {
    // int x = event.xbutton.x;
    // int y = event.xbutton.y;
    int mouseButton = event.xbutton.button;

    // printf("Mouse click: x=%d, y=%d, button=%d\n", x, y, mouseButton);

    if (mouseButton != Button1) {
        // Set initial position on right and middle mouse buttons
        m_Alpha = 0.;
        m_Beta = 0.;
        redraw();
    } else {
        m_MousePos.x = (-1); m_MousePos.y = (-1); // Mouse position undefined
    }
}

void MyWindow::onButtonRelease(XEvent& /* event */) {
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

    // Coordinate system
    glBegin(GL_LINES);
        // X-axis
        color[0] = 1.0; color[1] = 0.2; color[2] = 0.2;  // Red
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

        normal[0] = 0.;
        normal[1] = 0.;
        normal[2] = 1.;
        glNormal3fv(normal);
        glVertex3f(-10., 0., 0.);
        glVertex3f(10., 0., 0.);

        // Y-axis
        color[0] = 0.3; color[1] = 1.0; color[2] = 0.2;  // Green
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

        //... normal[0] = 0.;
        //... normal[1] = 0.;
        //... normal[2] = 1.;
        //... glNormal3fv(normal);
        glVertex3f(0., -10., 0.);
        glVertex3f(0., 10., 0.);

        // Z-axis
        color[0] = 0.1; color[1] = 0.3; color[2] = 1.0;  // Blue
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

        normal[0] = 1.;
        normal[1] = 0.;
        normal[2] = 0.;
        glNormal3fv(normal);
        glVertex3f(0., 0., -10.);
        glVertex3f(0., 0., 10.);
    glEnd();


    // Draw a graph of function z = f(x, y)
    glBegin(GL_TRIANGLES);
        double xmin = -2.5;
        double xmax = 2.5;
        double ymin = -2.5;
        double ymax = 2.5;
        double dx = 0.05;
        double dy = 0.05;

        //... color[0] = 0.6; color[1] = 0.5; color[2] = 0.4;
        //... glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
        //...
        //... normal[0] = 0.;
        //... normal[1] = 0.;
        //... normal[2] = 1.;
        //... glNormal3fv(normal);

        double y = ymin;
        while (y < ymax) {
            double x = xmin;
            while (x < xmax) {
                double z00 = f(x, y);
                double z10 = f(x+dx, y);
                double z01 = f(x, y+dy);
                double z11 = f(x+dx, y+dy);

                gradient(x, y, normal);
                glNormal3fv(normal);
                setColor(z00);
                glVertex3f(x, y, z00);

                gradient(x+dx, y, normal);
                glNormal3fv(normal);
                setColor(z10);
                glVertex3f(x+dx, y, z10);

                gradient(x+dx, y+dy, normal);
                glNormal3fv(normal);
                setColor(z11);
                glVertex3f(x+dx, y+dy, z11);

                //----------------------------

                gradient(x, y, normal);
                glNormal3fv(normal);
                setColor(z00);
                glVertex3f(x, y, z00);

                gradient(x+dx, y+dy, normal);
                glNormal3fv(normal);
                setColor(z11);
                glVertex3f(x+dx, y+dy, z11);

                gradient(x, y+dy, normal);
                glNormal3fv(normal);
                setColor(z01);
                glVertex3f(x, y+dy, z01);

                x += dx;
            }
            y += dy;
        }
    glEnd();
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
