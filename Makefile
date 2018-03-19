
include Makefile.common

CPPFLAGS = -DDEBUG -O3 -g -Wall -I. -Ilibs -Ilibs/fmod/api/inc -DDCDRAW_USE_OPENGL `sdl-config --cflags` 
LFLAGS = -lpng -lz `sdl-config --libs` -lGL -lGLU #-Llibs/fmod/api/lib -lfmodex
STATIC_LIBS = libs/physfs/.libs/libphysfs.a libs/cl2/bin/cl2.a libs/dcdraw/dcdraw_opengl.a

all: $(OBJS)
	g++ $(LFLAGS) $(OBJS) $(STATIC_LIBS) -o $(EXE)

clean:
	-rm $(OBJS)
	-rm $(EXE)

%.o : %.cpp
	g++ -c $(CPPFLAGS) $< -o $@

