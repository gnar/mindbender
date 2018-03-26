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

include_directories(.)

add_executable(mindbender
        cl2/src/compiler/clcompiler.cpp
        cl2/src/compiler/clcompiler.h
        cl2/src/compiler/clifunction.cpp
        cl2/src/compiler/clifunction.h
        cl2/src/compiler/cliinstruction.cpp
        cl2/src/compiler/cliinstruction.h
        cl2/src/compiler/cllexer.cpp
        cl2/src/compiler/cllexer.h
        cl2/src/opt/clnamespace.cpp
        cl2/src/opt/clnamespace.h
        cl2/src/serialize/clserializer.h
        cl2/src/serialize/clserialloader.cpp
        cl2/src/serialize/clserialloader.h
        cl2/src/serialize/clserialsaver.cpp
        cl2/src/serialize/clserialsaver.h
        cl2/src/serialize/cluserdataserializer.h
        cl2/src/value/clarray.cpp
        cl2/src/value/clarray.h
        cl2/src/value/clexternalfunction.cpp
        cl2/src/value/clexternalfunction.h
        cl2/src/value/clfunction.cpp
        cl2/src/value/clfunction.h
        cl2/src/value/clobject.cpp
        cl2/src/value/clobject.h
        cl2/src/value/clstring.cpp
        cl2/src/value/clstring.h
        cl2/src/value/cltable.cpp
        cl2/src/value/cltable.h
        cl2/src/value/cluserdata.cpp
        cl2/src/value/cluserdata.h
        cl2/src/value/clvalue.cpp
        cl2/src/value/clvalue.h
        cl2/src/vm/clcollectable.cpp
        cl2/src/vm/clcollectable.h
        cl2/src/vm/clcontext.cpp
        cl2/src/vm/clcontext.h
        cl2/src/vm/clmathmodule.cpp
        cl2/src/vm/clmathmodule.h
        cl2/src/vm/clmodule.cpp
        cl2/src/vm/clmodule.h
        cl2/src/vm/clsysmodule.cpp
        cl2/src/vm/clsysmodule.h
        cl2/src/vm/clthread.cpp
        cl2/src/vm/clthread.h
        cl2/src/cl2.h
        cl2/src/clopcode.cpp
        cl2/src/clopcode.h
        cl2/cl2.h

        dcdraw/opengl/opengl_drv.cpp
        dcdraw/opengl/opengl_drv.h
        dcdraw/opengl/texture.cpp
        dcdraw/opengl/texture.h
        dcdraw/canvas.cpp
        dcdraw/canvas.h
        dcdraw/canvas_png.cpp
        dcdraw/canvas_png.h
        dcdraw/clip.h
        dcdraw/color.h
        dcdraw/dcdraw.cpp
        dcdraw/dcdraw.h
        dcdraw/shared_ptr.h
        dcdraw/transform.h
        dcdraw/vector.h

        tinyxml/tinystr.cpp
        tinyxml/tinystr.h
        tinyxml/tinyxml.cpp
        tinyxml/tinyxml.h
        tinyxml/tinyxmlerror.cpp
        tinyxml/tinyxmlparser.cpp

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
