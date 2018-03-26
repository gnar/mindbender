#ifndef DCDRAW_H
#define DCDRAW_H

#include "color.h"
#include "clip.h"
#include "transform.h"
#include "vector.h"

#include "canvas.h"
#include "canvas_png.h"

#include "texture.h"
#include "opengl_drv.h"

namespace DCDraw {
    void SetPathPrefix(const std::string &prefix);

    const std::string &GetPathPrefix();
}

#endif //DCDRAW_H

