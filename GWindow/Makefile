CFLAGS= -g -O0 -Wall -I/usr/X11R6/include -L/usr/X11R6/lib -I.. -I.
CC= g++ $(CFLAGS)

all: func gclock

func: func.o gwindow.o R2Graph/R2Graph.o
	$(CC) -o func func.o gwindow.o R2Graph/R2Graph.o -lX11

gclock: clock.o gwindow.o R2Graph/R2Graph.o
	$(CC) -o gclock clock.o gwindow.o R2Graph/R2Graph.o -lX11

gwindow.o: gwindow.cpp gwindow.h
	$(CC) -c gwindow.cpp

func.o: func.cpp gwindow.h
	$(CC) -c func.cpp

clock.o: clock.cpp gwindow.h
	$(CC) -c clock.cpp

R2Graph/R2Graph.o:
	cd R2Graph; make R2Graph.o; cd ..

gwindow.h: ../R2Graph/R2Graph.h

grtst: grtst.cpp gwindow.o
	$(CC) -o grtst grtst.cpp gwindow.o -lX11

clean:
	rm -f *.o func gclock grtst *\~
	cd R2Graph; make clean; cd ..
