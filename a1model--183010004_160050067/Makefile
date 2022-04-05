CC=g++

OPENGLLIB= -lGL
GLEWLIB= -lGLEW
GLFWLIB = -lglfw
LIBS=$(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS=-L/usr/local/lib 
CPPFLAGS=-I/usr/local/include -I./

BIN=bin/a1model
SRCS=src/assignment_01.cpp src/gl_framework.cpp src/shader_util.cpp 
INCLUDES=src/gl_framework.hpp src/shader_util.hpp src/assignment_01.hpp

all: $(BIN)

$(BIN): $(SRCS) $(INCLUDES) | bin_folder
	g++ $(CPPFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS) $(LIBS) -g -O3

clean:
	rm -f *~ *.o $(BIN)

bin_folder:
	mkdir -p bin
