#ifndef DCDRAW_H
#define DCDRAW_H

#include "vector.h"
#include "color.h"
#include "canvas.h"
#include "canvas_png.h"
#include "clip.h"
#include "transform.h"

namespace DCDraw {
    void SetPathPrefix(const std::string &prefix);

    const std::string &GetPathPrefix();
}

#define DCDRAW_USE_OPENGL

#if defined(DCDRAW_USE_OPENGL)

#include "opengl/opengl_drv.h"
#include "opengl/texture.h"

namespace DCDraw {
    using namespace DCDraw_OpenGL;
}

#define DCDRAW_BEGIN_TRANS() {}

#elif defined(DCDRAW_USE_KOS)
#include "kos/kos_drv.h"
#include "kos/texture.h"
namespace DCDraw 
{ 
    using namespace DCDraw_Kos;
}

#define DCDRAW_BEGIN_TRANS() {DCDraw::PvrBeginTrans();}

#else
#error "No platform selected! #define either DCDRAW_USE_OPENGL or DCDRAW_USE_KOS"
#endif

#endif //DCDRAW_H

