MindBender v0.0 - Copyright (C) 2006 by Gunnar Selke

This is free software, and you are welcome to redistribute it under certain conditions.
MindBender comes with ABSOLUTELY NO WARRANTY. 
See GPL.txt (included in this distribution) for details.

This program is not functional as such, you will need to download and install the demo package (TODO)
which contains some scripts and graphics first (see http://mindbender.sf.net).

Have fun!

How to build (Linux):
---------------------

Libraries you need: 
 1. SDL
 2. libpng, zlib
 3. OpenGL
In libs directory (see libs/libs.txt):
 4. PhysFS
 5. FModEx sound system (www.fmod.org)

Run the following commands to build MindBender:

cd libs
# Build scripting language (creates cl2/bin/cl2.a)
cd cl2
make lib
cd ..
# Build OpenGL wrappers (creates dcdraw/dcdraw_opengl.a)
cd dcdraw
make -f Makefile.opengl
cd ..
# Build Physfs (creates physfs/.libs/libphysfs.a)
cd physfs
./configure
make
cd ..
cd ..
# Build MindBender
make

How to build (Windows): (Tested with DevC++ 4.9.9.2)
-----------------------

1. Grab and install DevCpp at www.bloodshed.net/devcpp.html (tested with beta version 4.9.9.2)
2. In DevCpp, install the following packages using the package update manager:
     SDL, libpng, zlib, windows-api
3. Install FMODex (www.fmod.org) into the libs directory and rename it (from e.g. libs/fmodex1.2.3.4) to libs/fmod.
3. If you got the source via CVS, you need to download the source of the PhysFS library 
   (version 1.1.0) and copy its physfs-1.1.0 directory it to the mindbender directory. Rename physfs-1.1.0 to physfs. 
   (If you downloaded a source distribution, the physfs-1.1.0 directory should already exist.)
3. Open the Project make/mindbender.dev
4. Compile and run mindbender.exe
5. It will complain about .dlls, they can be found in your DevCpp/bin directory. Copy them into
   your mindbender directory.

Libraries used:
---------------

 - SDL (http://www.libsdl.org), LGPL
 - libPNG, zlib
 - OpenGL
 - TinyXML (newest version at http://tinyxml.sourceforge.net), ZLib licence
 - PhysicsFS (http://www.icculus.org/physfs), ZLib licence
 - FMODex (www.fmod.org)

