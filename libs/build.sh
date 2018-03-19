#!/bin/sh

# CL2
cd cl2
make lib
cd ..

# DCDraw
cd dcdraw
make -f Makefile.opengl
cd ..

# PhysFS
cd physfs
./configure
make
cd ..

