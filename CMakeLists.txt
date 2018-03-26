cmake_minimum_required(VERSION 3.9)
project(mindbender)

set(CMAKE_CXX_STANDARD 11)

find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(PhysFS REQUIRED)

include_directories(${PNG_INCLUDE_DIR})
link_libraries(${PNG_LIBRARY})

include_directories(${OPENGL_INCLUDE_DIR})
link_libraries(${OPENGL_gl_LIBRARY})

include_directories(${SDL_INCLUDE_DIR})
link_libraries(${SDL_LIBRARY})

include_directories(${PHYSFS_INCLUDE_DIR})
link_libraries(${PHYSFS_LIBRARY})

set(SRC ./src)

include_directories(${SRC})
add_executable(mindbender
	${SRC}/cl2/compiler/clcompiler.cpp
        ${SRC}/cl2/compiler/clcompiler.h
        ${SRC}/cl2/compiler/clifunction.cpp
        ${SRC}/cl2/compiler/clifunction.h
        ${SRC}/cl2/compiler/cliinstruction.cpp
        ${SRC}/cl2/compiler/cliinstruction.h
        ${SRC}/cl2/compiler/cllexer.cpp
        ${SRC}/cl2/compiler/cllexer.h
        ${SRC}/cl2/opt/clnamespace.cpp
        ${SRC}/cl2/opt/clnamespace.h
        ${SRC}/cl2/serialize/clserializer.h
        ${SRC}/cl2/serialize/clserialloader.cpp
        ${SRC}/cl2/serialize/clserialloader.h
        ${SRC}/cl2/serialize/clserialsaver.cpp
        ${SRC}/cl2/serialize/clserialsaver.h
        ${SRC}/cl2/serialize/cluserdataserializer.h
        ${SRC}/cl2/value/clarray.cpp
        ${SRC}/cl2/value/clarray.h
        ${SRC}/cl2/value/clexternalfunction.cpp
        ${SRC}/cl2/value/clexternalfunction.h
        ${SRC}/cl2/value/clfunction.cpp
        ${SRC}/cl2/value/clfunction.h
        ${SRC}/cl2/value/clobject.cpp
        ${SRC}/cl2/value/clobject.h
        ${SRC}/cl2/value/clstring.cpp
        ${SRC}/cl2/value/clstring.h
        ${SRC}/cl2/value/cltable.cpp
        ${SRC}/cl2/value/cltable.h
        ${SRC}/cl2/value/cluserdata.cpp
        ${SRC}/cl2/value/cluserdata.h
        ${SRC}/cl2/value/clvalue.cpp
        ${SRC}/cl2/value/clvalue.h
        ${SRC}/cl2/vm/clcollectable.cpp
        ${SRC}/cl2/vm/clcollectable.h
        ${SRC}/cl2/vm/clcontext.cpp
        ${SRC}/cl2/vm/clcontext.h
        ${SRC}/cl2/vm/clmathmodule.cpp
        ${SRC}/cl2/vm/clmathmodule.h
        ${SRC}/cl2/vm/clmodule.cpp
        ${SRC}/cl2/vm/clmodule.h
        ${SRC}/cl2/vm/clsysmodule.cpp
        ${SRC}/cl2/vm/clsysmodule.h
        ${SRC}/cl2/vm/clthread.cpp
        ${SRC}/cl2/vm/clthread.h
        ${SRC}/cl2/clopcode.cpp
        ${SRC}/cl2/clopcode.h
        ${SRC}/cl2/cl2.h

        ${SRC}/dcdraw/opengl_drv.cpp
        ${SRC}/dcdraw/opengl_drv.h
        ${SRC}/dcdraw/texture.cpp
        ${SRC}/dcdraw/texture.h
        ${SRC}/dcdraw/canvas.cpp
        ${SRC}/dcdraw/canvas.h
        ${SRC}/dcdraw/canvas_png.cpp
        ${SRC}/dcdraw/canvas_png.h
        ${SRC}/dcdraw/clip.h
        ${SRC}/dcdraw/color.h
        ${SRC}/dcdraw/transform.h
        ${SRC}/dcdraw/vector.h
        ${SRC}/dcdraw/dcdraw.cpp
        ${SRC}/dcdraw/dcdraw.h

        ${SRC}/tinyxml/tinystr.cpp
        ${SRC}/tinyxml/tinystr.h
        ${SRC}/tinyxml/tinyxml.cpp
        ${SRC}/tinyxml/tinyxml.h
        ${SRC}/tinyxml/tinyxmlerror.cpp
        ${SRC}/tinyxml/tinyxmlparser.cpp

        ${SRC}/object/actor.cpp
        ${SRC}/object/actor.h
        ${SRC}/object/bagitem.cpp
        ${SRC}/object/bagitem.h
        ${SRC}/object/font.cpp
        ${SRC}/object/font.h
        ${SRC}/object/item.cpp
        ${SRC}/object/item.h
        ${SRC}/object/room.cpp
        ${SRC}/object/room.h
        ${SRC}/object/roomobject.cpp
        ${SRC}/object/roomobject.h
        ${SRC}/object/shape.cpp
        ${SRC}/object/shape.h
        ${SRC}/object/sound.cpp
        ${SRC}/object/sound.h
        ${SRC}/object/sprite.cpp
        ${SRC}/object/sprite.h
        ${SRC}/object/tableobject.cpp
        ${SRC}/object/tableobject.h
        ${SRC}/object/timer.cpp
        ${SRC}/object/timer.h

        ${SRC}/resource/tools/physfsrwops.c
        ${SRC}/resource/tools/physfsrwops.h
        ${SRC}/resource/tools/polygonpath.cpp
        ${SRC}/resource/tools/polygonpath.h
        ${SRC}/resource/tools/tools.h
        ${SRC}/resource/font.cpp
        ${SRC}/resource/font.h
        ${SRC}/resource/font_bitmap.cpp
        ${SRC}/resource/font_bitmap.h
        ${SRC}/resource/image.cpp
        ${SRC}/resource/image.h
        ${SRC}/resource/loadxml.h
        ${SRC}/resource/manager.cpp
        ${SRC}/resource/manager.h
        ${SRC}/resource/polygon.cpp
        ${SRC}/resource/polygon.h
        ${SRC}/resource/resource.cpp
        ${SRC}/resource/resource.h
        ${SRC}/resource/sound.cpp
        ${SRC}/resource/sound.h
        ${SRC}/resource/sprite.cpp
        ${SRC}/resource/sprite.h
        ${SRC}/resource/sprite_anim.cpp
        ${SRC}/resource/sprite_anim.h
        ${SRC}/resource/sprite_simple.cpp
        ${SRC}/resource/sprite_simple.h

        ${SRC}/scene/gameloader.cpp
        ${SRC}/scene/gameloader.h
        ${SRC}/scene/scenelexer.cpp
        ${SRC}/scene/scenelexer.h
        ${SRC}/scene/sceneloader.cpp
        ${SRC}/scene/sceneloader.h
        ${SRC}/scene/sceneparser.cpp
        ${SRC}/scene/sceneparser.h

        ${SRC}/camera.cpp
        ${SRC}/camera.h
        ${SRC}/configuration.cpp
        ${SRC}/configuration.h
        ${SRC}/events.cpp
        ${SRC}/events.h
        ${SRC}/saveload.cpp
        ${SRC}/sound.cpp
        ${SRC}/sound.h
        ${SRC}/sushimodule.cpp
        ${SRC}/sushimodule.h
        ${SRC}/timers.cpp
        ${SRC}/timers.h
        ${SRC}/video.cpp
        ${SRC}/video.h
        ${SRC}/game.cpp
        ${SRC}/game.h
        ${SRC}/main.cpp
        ${SRC}/main.h
        )
