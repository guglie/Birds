
CC = g++
LIBS = -framework GLUT -framework OPENGL
CFLAGS = -Wall
LFLAGS = $(CFLAGS) $(LIBS)

all: birds *.h

%: %.cpp
	$(CC) $(LFLAGS) $< -o $@


clean:
	rm birds
	
