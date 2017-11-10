#include <X11/Xlib.h>

#include <GL/glx.h> 
#include <GL/gl.h> 
#include <GL/glu.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/times.h>

static int attributeListSgl[] = {
     GLX_RGBA,
     //... GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8,
     //... GLX_DEPTH_SIZE, 16,
     GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,
     GLX_DEPTH_SIZE, 1,     // "1" means a maximal nonzero size available
     None
};

static int attributeListDbl[] = {
     GLX_RGBA,
     GLX_DOUBLEBUFFER, 
     GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,
     GLX_DEPTH_SIZE, 1,     // "1" means a maximal nonzero size available
     None
};

static Bool WaitForNotify(
    Display * /* d */, XEvent *e, char *arg) {
    return (
        (e->type == MapNotify) &&
        (e->xmap.window == (Window) arg)
    );
}

int main(int /* argc */, char ** /* argv */) {
    Display *dpy;
    XVisualInfo *vi;
    // Colormap cmap;
    XSetWindowAttributes swa;
    Window win;
    GLXContext cx;
    XEvent event;
    int swap_flag = False;
    int i;
    GLfloat r, g, b;
    GLfloat color[4];
    GLUquadricObj* box = gluNewQuadric();

    srand(times(NULL)); /* Initialize random generator */

    dpy = XOpenDisplay(0);

    vi =  glXChooseVisual(
        dpy, DefaultScreen(dpy), attributeListDbl
    );
    if (vi == NULL) {
        vi = glXChooseVisual(
            dpy, DefaultScreen(dpy), attributeListSgl
        );
    } else {
        swap_flag = True;
    }

    cx = glXCreateContext(dpy, vi, 0, GL_TRUE);

    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;
    win = XCreateWindow(
        dpy, RootWindow(dpy, vi->screen),
        40, 40, 300, 300,
        0, vi->depth, InputOutput,
        //... vi->visual,
        CopyFromParent,
        CWBorderPixel|CWEventMask, &swa
    );
    XMapWindow(dpy, win);
    XIfEvent(dpy, &event, WaitForNotify, (char*)win);

    glXMakeCurrent(dpy, win, cx);

    glClearColor(0.3, 0.4, 0.6, 1.f);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glMatrixMode(GL_PROJECTION);
    glOrtho(
        -1., 1.,
        -1., 1.,
        -2., 2.
    );

    for (i = 0; i < 20; i++) {

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(-30., 0., 1., 0.);
        glRotatef(10., 1., 0., 0.);

        r = 0.4 + 0.6 * (GLfloat) rand() / RAND_MAX;
        g = 0.4 + 0.6 * (GLfloat) rand() / RAND_MAX;
        b = 0.4 + 0.6 * (GLfloat) rand() / RAND_MAX;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glNormal3d(0., 0., 1.);

        glBegin(GL_TRIANGLES);
            color[0] = r; color[1] = 0.; color[2] = 0.; color[3] = 1.;
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
            glVertex3f(0., 0., 0.8);
            glVertex3f(1., 0., 0.);
            glVertex3f(0., 1., 0.);

            color[0] = 0.; color[1] = g; color[2] = 0.;
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
            glVertex3f(0., 0., 0.8);
            glVertex3f(0., 1., 0.);
            glVertex3f(-1., 0., 0.);

            color[0] = 0.; color[1] = 0.; color[2] = b;
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
            glVertex3f(0., 0., 0.8);
            glVertex3f(-1., 0., 0.);
            glVertex3f(0., -1., 0.);

            color[0] = r; color[1] = 0.8*b; color[2] = 0.;
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
            glColor3f(r, 0.8*g, 0.);
            glVertex3f(0., 0., 0.8);
            glVertex3f(0., -1., 0.);
            glVertex3f(1., 0., 0.);

        glEnd();

        glTranslatef(
            (GLfloat) rand() / RAND_MAX - 0.5,
            (GLfloat) rand() / RAND_MAX - 0.5,
            (GLfloat) rand() / RAND_MAX - 0.5
        );

        r = 0.2 + 0.8 * (GLfloat) rand() / RAND_MAX;
        g = 0.4 + 0.6 * (GLfloat) rand() / RAND_MAX;
        b = 1.;
        color[0] = r; color[1] = g; color[2] = b;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
        gluSphere(box, 0.5, 32, 16);

        glFlush();
        if (swap_flag)
            glXSwapBuffers(dpy, win);
        sleep(2);
    }
    return 0;
}
