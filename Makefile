#
# Interface to OpenGL over X11 Graphics:
# class GLWindow
#
# CFLAGS = -g -O0 -L/usr/X11R6/lib -Xlinker "-rpath-link /usr/X11R6/lib"
CFLAGS = -g -O0 -L/usr/X11R6/lib
### CFLAGS = -g -O0 -L/usr/X11R6/lib -Wl,-rpath-link -Wl,/usr/X11R6/lib
# CC = LD_LIBRARY_PATH=/usr/X11R6/lib /usr/bin/g++ $(CFLAGS)
CC = g++ $(CFLAGS)

all: tetraedr moon func glfirst biliard

# Draw a Tetraedron
tetraedr: tetraedr.o GLWindow.o GWindow/gwindow.o
	$(CC) -o tetraedr tetraedr.o GLWindow.o GWindow/gwindow.o \
		-lm -lX11 -lGL -lGLU

# Moon movement
moon: moon.o GLWindow.o GWindow/gwindow.o
	$(CC) -o moon moon.o GLWindow.o GWindow/gwindow.o \
                -lm -lX11 -lGL -lGLU -lpthread

# Draw a Graph of Function z=f(x,y)
func: func.o GLWindow.o GWindow/gwindow.o
	$(CC) -o func func.o GLWindow.o GWindow/gwindow.o \
		-lm -lX11 -lGL -lGLU

biliard: biliard.o GLWindow.o GWindow/gwindow.o
	$(CC) -o biliard biliard.o GLWindow.o GWindow/gwindow.o \
		-lm -lX11 -lGL -lGLU

# Timer test
timtst: timtst.cpp
	$(CC) -o timtst timtst.cpp

tetraedr.o: tetraedr.cpp GLWindow.h
	$(CC) -c tetraedr.cpp

moon.o: moon.cpp GLWindow.h
	$(CC) -c moon.cpp

func.o: func.cpp GLWindow.h
	$(CC) -c func.cpp

b.o: biliard.cpp GLWindow.h
	$(CC) -c biliard.cpp

GLWindow.o: GLWindow.cpp GLWindow.h GWindow/gwindow.h
	$(CC) -c GLWindow.cpp

GWindow/gwindow.o:
	cd GWindow; make gwindow.o; cd ..

# Very simple test
glfirst: glFirst.cpp
	$(CC) -o glfirst glFirst.cpp -lm -lX11 -lGL -lGLU

clean:
	rm -rf *.o tetraedr moon timtst glfirst func biliard*\~
	cd GWindow; make clean; cd ..
