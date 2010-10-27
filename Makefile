
CC = g++
LIBS = -framework GLUT -framework OPENGL
CFLAGS = -Wall
LFLAGS = $(CFLAGS) $(LIBS)



all: birds depend

%: %.cpp
	$(CC) $(LFLAGS) $< -o $@


clean:
	rm birds
	

depend: birds.cpp
	makedepend $^

# DO NOT DELETE THIS LINE -- make depend needs it

birds.o: /usr/include/stdio.h /usr/include/_types.h /usr/include/sys/_types.h
birds.o: /usr/include/sys/cdefs.h /usr/include/machine/_types.h
birds.o: /usr/include/i386/_types.h /usr/include/stdlib.h
birds.o: /usr/include/available.h /usr/include/sys/wait.h
birds.o: /usr/include/sys/signal.h /usr/include/sys/appleapiopts.h
birds.o: /usr/include/machine/signal.h /usr/include/i386/signal.h
birds.o: /usr/include/i386/_structs.h /usr/include/sys/_structs.h
birds.o: /usr/include/machine/_structs.h /usr/include/sys/resource.h
birds.o: /usr/include/machine/endian.h /usr/include/i386/endian.h
birds.o: /usr/include/sys/_endian.h /usr/include/libkern/_OSByteOrder.h
birds.o: /usr/include/libkern/i386/_OSByteOrder.h /usr/include/alloca.h
birds.o: /usr/include/machine/types.h /usr/include/i386/types.h
birds.o: /usr/include/string.h /usr/include/time.h /usr/include/_structs.h
birds.o: /usr/include/math.h /usr/include/architecture/i386/math.h
birds.o: pointLineSegmentDistance.h options.h utils.h bird.h stats.h
birds.o: birdslib.h birdsmouse.h birdskeys.h birdsdisplay.h birdsmenu.h
