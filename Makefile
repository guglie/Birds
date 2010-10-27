
CC = g++
LIBS = -framework GLUT -framework OPENGL
CFLAGS = -Wall
LFLAGS = $(CFLAGS) $(LIBS)

BIRDLIBS = pointLineSegmentDistance.h options.h utils.h bird.h stats.h \
birdslib.h birdsmouse.h birdskeys.h birdsdisplay.h birdsmenu.h

all: birds $(BIRDLIBS) #depend

%: %.cpp
	$(CC) $(LFLAGS) $< -o $@


clean:
	rm birds
	

#depend: birds.cpp
#	makedepend $^
#
# DO NOT DELETE THIS LINE -- make depend needs it

