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

include_directories(. libs)

add_executable(mindbender
        libs/cl2/src/compiler/clcompiler.cpp
        libs/cl2/src/compiler/clcompiler.h
        libs/cl2/src/compiler/clcompiler_agent_ext.cpp
        libs/cl2/src/compiler/clcompiler_mindbender_ext.cpp
        libs/cl2/src/compiler/clifunction.cpp
        libs/cl2/src/compiler/clifunction.h
        libs/cl2/src/compiler/cliinstruction.cpp
        libs/cl2/src/compiler/cliinstruction.h
        libs/cl2/src/compiler/cllexer.cpp
        libs/cl2/src/compiler/cllexer.h
        libs/cl2/src/opt/clnamespace.cpp
        libs/cl2/src/opt/clnamespace.h
        libs/cl2/src/serialize/clserializer.h
        libs/cl2/src/serialize/clserialloader.cpp
        libs/cl2/src/serialize/clserialloader.h
        libs/cl2/src/serialize/clserialsaver.cpp
        libs/cl2/src/serialize/clserialsaver.h
        libs/cl2/src/serialize/cluserdataserializer.h
        libs/cl2/src/value/clarray.cpp
        libs/cl2/src/value/clarray.h
        libs/cl2/src/value/clexternalfunction.cpp
        libs/cl2/src/value/clexternalfunction.h
        libs/cl2/src/value/clfunction.cpp
        libs/cl2/src/value/clfunction.h
        libs/cl2/src/value/clobject.cpp
        libs/cl2/src/value/clobject.h
        libs/cl2/src/value/clstring.cpp
        libs/cl2/src/value/clstring.h
        libs/cl2/src/value/cltable.cpp
        libs/cl2/src/value/cltable.h
        libs/cl2/src/value/cluserdata.cpp
        libs/cl2/src/value/cluserdata.h
        libs/cl2/src/value/clvalue.cpp
        libs/cl2/src/value/clvalue.h
        libs/cl2/src/vm/clcollectable.cpp
        libs/cl2/src/vm/clcollectable.h
        libs/cl2/src/vm/clcontext.cpp
        libs/cl2/src/vm/clcontext.h
        libs/cl2/src/vm/clmathmodule.cpp
        libs/cl2/src/vm/clmathmodule.h
        libs/cl2/src/vm/clmodule.cpp
        libs/cl2/src/vm/clmodule.h
        libs/cl2/src/vm/clsysmodule.cpp
        libs/cl2/src/vm/clsysmodule.h
        libs/cl2/src/vm/clthread.cpp
        libs/cl2/src/vm/clthread.h
        libs/cl2/src/cl2.h
        libs/cl2/src/clopcode.cpp
        libs/cl2/src/clopcode.h
        libs/cl2/cl2.h

        libs/dcdraw/opengl/opengl_drv.cpp
        libs/dcdraw/opengl/opengl_drv.h
        libs/dcdraw/opengl/texture.cpp
        libs/dcdraw/opengl/texture.h
        libs/dcdraw/canvas.cpp
        libs/dcdraw/canvas.h
        libs/dcdraw/canvas_png.cpp
        libs/dcdraw/canvas_png.h
        libs/dcdraw/clip.h
        libs/dcdraw/color.h
        libs/dcdraw/dcdraw.cpp
        libs/dcdraw/dcdraw.h
        libs/dcdraw/shared_ptr.h
        libs/dcdraw/transform.h
        libs/dcdraw/vector.h

        libs/tinyxml/tinystr.cpp
        libs/tinyxml/tinystr.h
        libs/tinyxml/tinyxml.cpp
        libs/tinyxml/tinyxml.h
        libs/tinyxml/tinyxmlerror.cpp
        libs/tinyxml/tinyxmlparser.cpp

        object/actor.cpp
        object/actor.h
        object/bagitem.cpp
        object/bagitem.h
        object/font.cpp
        object/font.h
        object/item.cpp
        object/item.h
        object/room.cpp
        object/room.h
        object/roomobject.cpp
        object/roomobject.h
        object/shape.cpp
        object/shape.h
        object/sound.cpp
        object/sound.h
        object/sprite.cpp
        object/sprite.h
        object/tableobject.cpp
        object/tableobject.h
        object/timer.cpp
        object/timer.h

        resource/tools/physfsrwops.c
        resource/tools/physfsrwops.h
        resource/tools/polygonpath.cpp
        resource/tools/polygonpath.h
        resource/tools/tools.h
        resource/font.cpp
        resource/font.h
        resource/font_bitmap.cpp
        resource/font_bitmap.h
        resource/image.cpp
        resource/image.h
        resource/loadxml.h
        resource/manager.cpp
        resource/manager.h
        resource/polygon.cpp
        resource/polygon.h
        resource/resource.cpp
        resource/resource.h
        resource/sound.cpp
        resource/sound.h
        resource/sprite.cpp
        resource/sprite.h
        resource/sprite_anim.cpp
        resource/sprite_anim.h
        resource/sprite_simple.cpp
        resource/sprite_simple.h

        scene/gameloader.cpp
        scene/gameloader.h
        scene/scenelexer.cpp
        scene/scenelexer.h
        scene/sceneloader.cpp
        scene/sceneloader.h
        scene/sceneparser.cpp
        scene/sceneparser.h

        camera.cpp
        camera.h
        configuration.cpp
        configuration.h
        events.cpp
        events.h
        saveload.cpp
        sound.cpp
        sound.h
        sushimodule.cpp
        sushimodule.h
        timers.cpp
        timers.h
        video.cpp
        video.h
        game.cpp
        game.h
        main.cpp
        main.h
        )
