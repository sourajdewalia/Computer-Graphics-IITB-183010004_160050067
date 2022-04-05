CC=g++

OPENGLLIB= -lGL
GLEWLIB= -lGLEW
GLFWLIB = -lglfw
LIBS=$(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS=-L/usr/local/lib 
CPPFLAGS=-I/usr/local/include -I./

BIN=bin/spline
SRCS=src/trajectory.cpp src/trajectory_framework.cpp src/shader_util.cpp 
INCLUDES=src/trajectory_framework.hpp src/shader_util.hpp src/trajectory.hpp

BIN2=bin/animate
SRCS2=src/animation.cpp src/gl_framework.cpp src/shader_util.cpp src/texture.cpp src/obj_model.cpp src/hierarchy_node.cpp src/util.cpp
INCLUDES2=src/gl_framework.hpp src/shader_util.hpp src/texture.hpp src/obj_model.hpp src/animation.hpp src/util.hpp

all: $(BIN) $(BIN2)

$(BIN): $(SRCS) $(INCLUDES) | bin_folder
	g++ $(CPPFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS) $(LIBS) --std=c++14 -g -O3

$(BIN2): $(SRCS2) $(INCLUDES2) | bin_folder
	g++ $(CPPFLAGS) $(SRCS2) -o $(BIN2) $(LDFLAGS) $(LIBS) --std=c++14 -g -O3 -l assimp

clean:
	rm -f *~ *.o $(BIN)
	rm -f *~ *.o $(BIN2)

bin_folder:
	mkdir -p bin
