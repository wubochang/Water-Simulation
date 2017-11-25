# Makefile 
# CSCI 480
# Assignment 1

# Make sure you have the following libraries before
# compiling this program:
# 1. libjpeg (e.g. sudo apt-get install libjpeg62-dev)
# 2. glut (e.g. sudo apt-get install libglut3-dev)
# 3. openGL (e.g. sudo apt-get install libglu1-mesa-dev libgl1-mesa-dev)

# we assume the pic directory is located one level above,
# change PIC_PATH if this is not the case
#PIC_PATH = $(abspath $(CURDIR)/../pic)

#INCLUDE = -I$(PIC_PATH)
#LIBRARIES = -L$(PIC_PATH) -lGL -lglfw -lGLEW  -lglm 

INCLUDE = -I$(CURDIR)
LIBRARIES = -L$(CURDIR) -lGL -lglfw -lGLEW  -lglm 

COMPILER = g++
COMPILERFLAGS = -O3 $(INCLUDE)
COMPILERFLAGS = -O3

PROGRAM = main
SOURCE = main.cpp shader.cpp texture.cpp controls.cpp objloader.cpp 
OBJECT = main.o shader.o texture.o controls.o objloader.o

.cpp.o: 
	$(COMPILER) -c $(COMPILERFLAGS) $<

all: $(PROGRAM)

$(PROGRAM): $(OBJECT)
	$(COMPILER) $(COMPILERFLAGS) -o $(PROGRAM) $(OBJECT) $(LIBRARIES)

clean:
	-rm -rf *.o *~ .*~ $(PROGRAM)
