
CC = g++
LIBS = -framework GLUT -framework OPENGL
CFLAGS = -Wall
LFLAGS = $(CFLAGS) $(LIBS)

all: birds.cpp
	$(CC) $(LFLAGS) birds.cpp -o birds


clean:
	rm birds
	
