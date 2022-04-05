CC=g++

OPENGLLIB= -lGL
GLEWLIB= -lGLEW
GLFWLIB = -lglfw
LIBS=$(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS=-L/usr/local/lib
CPPFLAGS=-I/usr/local/include -I./

BIN=bin/a3-model-0
SRCS=src/earth.cpp src/gl_framework.cpp src/shader_util.cpp src/texture.cpp src/obj_model.cpp
INCLUDES=src/gl_framework.hpp src/shader_util.hpp src/earth.hpp src/texture.hpp src/obj_model.hpp

BIN2=bin/scene
SRCS2=src/scene.cpp src/gl_framework.cpp src/shader_util.cpp src/texture.cpp src/obj_model.cpp
INCLUDES2=src/gl_framework.hpp src/shader_util.hpp src/scene.hpp src/texture.hpp src/obj_model.hpp

BIN3=bin/a3-model-2
SRCS3=src/satellite.cpp src/gl_satframework.cpp src/shader_util.cpp src/texture.cpp src/obj_model.cpp src/hierarchy_node.cpp
INCLUDES3=src/gl_framework.hpp src/shader_util.hpp src/satellite.hpp src/texture.hpp src/obj_model.hpp src/hierarchy_node.hpp

BIN4=bin/a3-model-1
SRCS4=src/rocket.cpp src/gl_framework.cpp src/shader_util.cpp src/texture.cpp src/obj_model.cpp src/hierarchy_node.cpp
INCLUDES4=src/gl_framework.hpp src/shader_util.hpp src/satellite.hpp src/texture.hpp src/obj_model.hpp src/rocket.hpp

BIN5=bin/a3-scene

all: $(BIN) $(BIN2) $(BIN3) $(BIN4)

$(BIN): $(SRCS) $(INCLUDES) | bin_folder
	g++ $(CPPFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS) $(LIBS) --std=c++14 -g -O3 -l assimp

$(BIN2): $(SRCS2) $(INCLUDES2) | bin_folder
	g++ $(CPPFLAGS) $(SRCS2) -o $(BIN2) $(LDFLAGS) $(LIBS) --std=c++14 -g -O3 -l assimp
$(BIN3): $(SRCS3) $(INCLUDES3) | bin_folder
	g++ $(CPPFLAGS) $(SRCS3) -o $(BIN3) $(LDFLAGS) $(LIBS) --std=c++14 -g -O3 -l assimp
$(BIN4): $(SRCS4) $(INCLUDES4) | bin_folder
	g++ $(CPPFLAGS) $(SRCS4) -o $(BIN4) $(LDFLAGS) $(LIBS) --std=c++14 -g -O3 -l assimp

$(BIN5): $(SRCS4) $(INCLUDES4) | bin_folder
	g++ $(CPPFLAGS) $(SRCS4) -o $(BIN5) $(LDFLAGS) $(LIBS) --std=c++14 -g -O3 -l assimp

clean:
	rm -f *~ *.o $(BIN)
	rm -f *~ *.o $(BIN2)
	rm -f *~ *.o $(BIN3)
	rm -f *~ *.o $(BIN4)
	rm -f *~ *.o $(BIN5)

bin_folder:
	mkdir -p bin
