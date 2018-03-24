#ifndef DDRAW_PNG_H
#define DDRAW_PNG_H

#include "canvas.h"

#include <string>

namespace DCDraw {
    class PNG {
    public:
        enum AlphaMask {
            NOALPHA,
            ALPHA,
        };

        static Canvas Load(const std::string &file, AlphaMask mask);
    };
};

#endif

