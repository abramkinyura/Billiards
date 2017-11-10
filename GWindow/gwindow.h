//
// File "gwindow.h"
// Simple graphic window, based on Xlib primitives
//

#ifndef _GWINDOW_H
#define _GWINDOW_H

// Classes for simple 2-dimensional objects
#include "R2Graph/R2Graph.h"

// include the X library headers
extern "C" {

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

}

//===============================

class ListHeader {
public:
    ListHeader*     next;
    ListHeader*     prev;

public:
    ListHeader():
        next(0),
        prev(0)
    {
    }

    ListHeader(ListHeader* n, ListHeader* p):
        next(n),
        prev(p)
    {
    }

    ListHeader(const ListHeader& h):
        next(h.next),
        prev(h.prev)
    {
    }

    ~ListHeader() {}

    ListHeader& operator=(const ListHeader& h) {
        next = h.next;
        prev = h.prev;
        return *this;
    }

    void link(ListHeader& h) {
        next = &h;
        h.prev = this;
    }
};

const int DEFAULT_BORDER_WIDTH = 2;

class GWindow: public ListHeader {
public:
    // Xlib objects:
    // Display and screen are the same for all windows
    static Display*     m_Display;
    static int          m_Screen;
    static Atom         m_WMProtocolsAtom;
    static Atom         m_WMDeleteWindowAtom;

    Window   m_Window;
    GC       m_GC;

    // Coordinates in window
    I2Point     m_WindowPosition;   // Window position in screen coord
    I2Rectangle m_IWinRect; // Window rectangle in (local) pixel coordinates
    R2Rectangle m_RWinRect; // Window rectangle in inner (real) coordinates

    I2Point m_ICurPos; // Current position, integer
    R2Point m_RCurPos; //                   real

    double m_xcoeff;  // Optimization: Coeff. for real->integer conversion
    double m_ycoeff;

    char   m_WindowTitle[128];

private:

    // Window state
    bool        m_WindowCreated;

    // Static members:
    // List of all windows
    static int          m_NumWindows;
    static int          m_NumCreatedWindows;
    static ListHeader   m_WindowList;

    // Background, foreground
    unsigned long       m_bgPixel;
    unsigned long       m_fgPixel;
    const char*         m_bgColorName;
    const char*         m_fgColorName;

    // Border width
    int m_BorderWidth;

    // Clip rectangle
    XRectangle          m_ClipRectangle;
    bool                m_BeginExposeSeries;

public:

    GWindow();
    GWindow(const I2Rectangle& frameRect, const char *title = 0);
    GWindow(
        const I2Rectangle& frameRect, 
        const R2Rectangle& coordRect,
        const char *title = 0
    );
    virtual ~GWindow();

    // The "createWindow" method uses the m_IWinRect member to
    // define the position and size of the window, and the
    // m_WindowTitle member to set the window title.
    // All parameters have their default values, so the method can
    // be called without any parameters: createWindow()
    void createWindow(
        GWindow* parentWindow = 0,              // parent window
        int borderWidth = DEFAULT_BORDER_WIDTH, // border width
        unsigned int wndClass = InputOutput,    // or InputOnly, CopyFromParent
        Visual* visual = CopyFromParent,        //
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

    static bool initX();
    static void closeX();
    static int  screenMaxX();
    static int  screenMaxY();

private:
    static GWindow* findWindow(Window w);

public:
    void drawFrame();
    void setCoordinates(double xmin, double ymin, double xmax, double ymax);
    void setCoordinates(const R2Rectangle& coordRect);
    void setCoordinates(const R2Point& leftBottom, const R2Point& rightTop);

    double getXMin() const { return m_RWinRect.getXMin(); }
    double getXMax() const { return m_RWinRect.getXMax(); }
    double getYMin() const { return m_RWinRect.getYMin(); }
    double getYMax() const { return m_RWinRect.getYMax(); }

    void drawAxes(
        const char *axesColorName = 0,
        bool drawGrid = false,
        const char *gridColorName = 0
    );

    I2Rectangle getWindowRect() const { return m_IWinRect; }
    R2Rectangle getCoordRect() const  { return m_RWinRect; }

    void moveTo(const I2Point& p);
    void moveTo(const R2Point& p);
    void moveTo(int x, int y);
    void moveTo(double x, double y);

    void moveRel(const I2Vector& p);
    void moveRel(const R2Vector& p);
    void moveRel(int x, int y);
    void moveRel(double x, double y);

    void drawLineTo(const I2Point& p);
    void drawLineTo(const R2Point& p);
    void drawLineTo(int x, int y);
    void drawLineTo(double x, double y);

    void drawLineRel(const I2Vector& p);
    void drawLineRel(const R2Vector& p);
    void drawLineRel(int x, int y);
    void drawLineRel(double x, double y);

    void drawLine(const I2Point& p1, const I2Point& p2);
    void drawLine(const I2Point& p,  const I2Vector& v);
    void drawLine(int x1, int y1, int x2, int y2);

    void drawLine(const R2Point& p1, const R2Point& p2);
    void drawLine(const R2Point& p,  const R2Vector& v);
    void drawLine(double x1, double y1, double x2, double y2);

    void drawString(int x, int y, const char *str, int len = (-1));
    void drawString(const I2Point& p, const char *str, int len = (-1));
    void drawString(const R2Point& p, const char *str, int len = (-1));
    void drawString(double x, double y, const char *str, int len = (-1));

    // The following methods should be called before "createWindow"
    void setBgColorName(const char* colorName);
    void setFgColorName(const char* colorName);

    unsigned long allocateColor(const char *colorName);

    void setBackground(unsigned long bg);
    void setBackground(const char *colorName);
    void setForeground(unsigned long fg);
    void setForeground(const char *colorName);

    unsigned long getBackground() const { return m_bgPixel; }
    unsigned long getForeground() const { return m_fgPixel; }

    void fillRectangle(const I2Rectangle&);
    void fillRectangle(const R2Rectangle&);

    void fillPolygon(const R2Point* points, int numPoints);
    void fillPolygon(const I2Point* points, int numPoints);

    void redraw();
    void redrawRectangle(const R2Rectangle&);
    void redrawRectangle(const I2Rectangle&);

    void setWindowTitle(const char* title);

    I2Point map(const R2Point& p) const;
    I2Point map(double x, double y) const;
    int mapX(double x) const;
    int mapY(double y) const;

    R2Point invMap(const I2Point& p) const;

    void recalculateMap();

    // Font methods
    Font loadFont(const char* fontName);
    void unloadFont(Font fontID);
    XFontStruct* queryFont(Font fontID);
    void setFont(Font fontID);

    // Callbacks:
    virtual void onExpose(XEvent& event);
    virtual void onResize(XEvent& event); // event.xconfigure.width, height
    virtual void onKeyPress(XEvent& event);
    virtual void onButtonPress(XEvent& event);
    virtual void onButtonRelease(XEvent& event);
    virtual void onMotionNotify(XEvent& event);
    virtual void onCreateNotify(XEvent& event);
    virtual void onDestroyNotify(XEvent& event);
    virtual void onFocusIn(XEvent& event);
    virtual void onFocusOut(XEvent& event);

    // Message from Window Manager, such as "Close Window"
    virtual void onClientMessage(XEvent& event);

    // This method is called from the base implementation of
    // method "onClientMessage". It allows a user application
    // to save all unsaved data and to do other operations before
    // closing a window, when a user pressed the closing box in the upper
    // right corner of a window. The application supplied method should return
    // "true" to close the window or "false" to leave the window open.
    virtual bool onWindowClosing();

    // Message loop
    static bool getNextEvent(XEvent& e);
    static void dispatchEvent(XEvent& e);
    static void messageLoop(GWindow* = 0);

    // For dialog windows
    void doModal();

    // Some methods implementing X-primitives
    virtual void destroyWindow();
    void mapRaised();
    void raise();

private:
    int clip(const R2Point& p1, const R2Point& p2,
                   R2Point& c1,       R2Point& c2);
};

#endif
//
// End of file "gwindow.h"
