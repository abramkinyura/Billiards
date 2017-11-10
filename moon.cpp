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
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GLWindow.h"

static const GLfloat EARTH_RADIUS = 0.3;

static const GLfloat MOON_RADIUS  = 0.1;
static const GLfloat MOON_ORBIT_RADIUS  = 0.8;
static const GLfloat MOON_ROTATION_SPEED = 4.;  // Orbit speed in degrees/sec
static const GLfloat MOON_SPIN_SPEED = 4.;      // Rotation speed
static const GLfloat EARTH_SPIN_SPEED = 12.;

static clock_t clocks_per_sec = 100;            // To be initialized...
static const int SLEEP_USEC = 100000;           // Sleep 0.1 sec

static bool finished = false;

//--------------------------------------------------
// Definition of class "MyWindow"
//
class MyWindow: public GLWindow {  // Our main class derived from GLWindow
    GLUquadricObj*  m_Quadric;  // Quadric object used to draw a sphere
    GLfloat         m_Alpha;    // Angle of rotation around vert.axis in degrees
    GLfloat         m_Beta;     // Angle of rotation around hor.axis in degrees
    I2Point         m_MousePos; // Previous position of mouse pointer

    // Animation
    GLfloat m_EarthSpin;     // Angle of Earth rotation
    GLfloat m_MoonAlpha;     // Moon orbit position
    GLfloat m_MoonSpin;      // Angle of Moon rotation
    clock_t m_AnimationTime; // A previous moment "animate" has been called at

public:
    MyWindow():             // Constructor
        GLWindow(),
        m_Quadric(0),
        m_Alpha(0.),
        m_Beta(10.),
        m_MousePos(-1, -1),
        m_EarthSpin(0.),
        m_MoonAlpha(0.),
        m_MoonSpin(0.),
        m_AnimationTime(0)
    {}

    void animate();

    void drawScene();       // Draw a scene graph
    void render();          // Render a 3D object

    virtual void onExpose(XEvent& event);
    virtual void onKeyPress(XEvent& event);
    virtual void onButtonPress(XEvent& event);
    virtual void onButtonRelease(XEvent& event);
    virtual void onMotionNotify(XEvent& event);
    virtual void destroyWindow();
    virtual bool onWindowClosing();
};

void MyWindow::destroyWindow() {
    GLWindow::destroyWindow();
    finished = true;
}

bool MyWindow::onWindowClosing() { // Called when user presses the close box
    printf("Window closing.\n");
    return true;    // Return true to close the window, false to ignore
}

//
// Process the Expose event: draw in the window
//
void MyWindow::onExpose(XEvent& /* event */) {
    drawScene();
}

void MyWindow::drawScene() {
    makeCurrent();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Rotate the complete model
    glRotatef(m_Beta, 1., 0., 0.);
    glRotatef(m_Alpha, 0., 1., 0.);

    // Light of Sun
    glEnable(GL_LIGHT0);        // Turn on the first light source

    // Position of light source
    GLfloat pos[4];
    pos[0] = 10.; pos[1] = 0.; pos[2] = 10.;
    pos[3] = 0.;                // Directional light
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    // Color of Sun
    GLfloat color[4];
    color[0] = 0.25; color[1] = 0.25; color[2] = 0.25; color[3] = 1.;
    glLightfv(GL_LIGHT0, GL_AMBIENT, color);
    color[0] = 1.; color[1] = 1.; color[2] = 0.8;
    glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
    color[2] = 1.;
    glLightfv(GL_LIGHT0, GL_SPECULAR, color);

    render();           // Draw all 3D objects

    swapBuffers();
}

void MyWindow::animate() {
    clock_t curtime = times(0);
    if (m_AnimationTime == 0) {
        // The first call
        m_AnimationTime = curtime;
        return;
    }

    clock_t dt = curtime - m_AnimationTime;
    if (dt >= SLEEP_USEC*clocks_per_sec/1000000) {
        // printf("Animation, curtime = %d\n", (int) curtime);
        GLfloat Dt = (GLfloat) dt / (GLfloat) clocks_per_sec; // in sec

        m_MoonAlpha += MOON_ROTATION_SPEED * Dt;
        if (m_MoonAlpha > 360.)
            m_MoonAlpha -= 360.;
        m_MoonSpin += MOON_SPIN_SPEED * Dt;
        if (m_MoonSpin > 360.)
            m_MoonSpin -= 360.;
        m_EarthSpin += EARTH_SPIN_SPEED * Dt;
        if (m_EarthSpin > 360.)
            m_EarthSpin -= 360.;

        //... drawScene();
        redraw();

        m_AnimationTime = curtime;
    }
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

    printf("Mouse click: x=%d, y=%d, button=%d\n", x, y, mouseButton);

    m_MousePos.x = (-1); m_MousePos.y = (-1); // Mouse position undefined
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
    GLfloat color[4];

    glClearColor(0.2, 0.3, 0.5, 1.); // Background color: dark blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Earth
    if (m_Quadric == 0) {
        m_Quadric = gluNewQuadric();    // Create a Quadric object
        gluQuadricNormals(m_Quadric, GLU_SMOOTH);
    }
    color[0] = 0.3; color[1] = 0.8; color[2] = 0.6; color[3] = 1.;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    glMaterialf(GL_FRONT, GL_SHININESS, 0.3);

    glRotatef(m_EarthSpin, 0., 1., 0.);
    glRotatef(90., 1., 0., 0.);
    gluSphere(
        m_Quadric, 
        EARTH_RADIUS,
        18,     // Num. slices (similar to lines of longitude)
        10      // Num. stacks (similar to lines of latitude)
    );
    // Draw meridians / parallels
    color[0] = 0.2; color[1] = 0.5; color[2] = 0.4; color[3] = 1.;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    gluQuadricDrawStyle(m_Quadric, GLU_LINE); // Draw lines only
    gluSphere(
        m_Quadric, 
        EARTH_RADIUS + 0.005,
        18,     // Num. slices (similar to lines of longitude)
        10      // Num. stacks (similar to lines of latitude)
    );
    gluQuadricDrawStyle(m_Quadric, GLU_FILL); // Restore the normal draw style
    glRotatef(-90., 1., 0., 0.);
    glRotatef(-m_EarthSpin, 0., 1., 0.);

    // Draw Moon
    glRotatef(m_MoonAlpha, 0., 1., 0.); // Orbit rotation

    // Shift Moon center
    glTranslatef(MOON_ORBIT_RADIUS, 0., 0.);
    color[0] = 0.2; color[1] = 0.6; color[2] = 1.;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    glMaterialf(GL_FRONT, GL_SHININESS, 0.6);

    glRotatef(-90., 1., 0., 0.);
    glRotatef(m_MoonSpin, 0., 1., 0.);  // Moon spin

    gluSphere(
        m_Quadric, 
        MOON_RADIUS,
        16,     // Num. slices (similar to lines of longitude)
        8       // Num. stacks (similar to lines of latitude)
    );
}

/////////////////////////////////////////////////////////////
// Main: initialize X, create an instance of MyWindow class,
//       and start the message loop
int main() {
    XEvent e;

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
        R2Rectangle(                        // Window coordinate rectangle
            R2Point(-1.3 * aspect, -1.3),   //     left-top corner,
            2.6 * aspect, 2.6               //     width, height
        ),
        "OpenGL Test"                   // Window title
    );
    w.setBackground("lightGray");
    w.makeCurrent();
    GLWindow::initializeOpenGL();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // a perspective-view matrix...

    glViewport(0, 0, width, height);
    glOrtho(
        -1.3 * aspect, 1.3 * aspect,    // left, right
        -1.3, 1.3,                      // bottom, top,
        -2., 2.                         // near, far
    );

    clocks_per_sec = (time_t) sysconf(_SC_CLK_TCK);

    // Message loop, animation
    while (!finished) {
        if (GLWindow::getNextEvent(e)) {
            GLWindow::dispatchEvent(e);
        } else {

            // Sleep a bit (we use select for sleeping)
            timeval dt;
            dt.tv_sec = 0;
            dt.tv_usec = 10000; // sleeping time 0.01 sec
            select(1, 0, 0, 0, &dt);

            w.animate();
        }
    }

    GWindow::closeX();
    return 0;
}
