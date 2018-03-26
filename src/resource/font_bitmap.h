/*
    MindBender - The MindBender adventure engine
    Copyright (C) 2006  Gunnar Selke

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef RES_FONT_BITMAP_H
#define RES_FONT_BITMAP_H

#include "resource.h"
#include "font.h"
#include "image.h"

#include <vector>
#include <string>

class TiXmlElement;

namespace Res {
    class Font_Bitmap : public Font {
    public:
        explicit Font_Bitmap(const Resource::ID &id);
        ~Font_Bitmap() override;

        void Draw(int x, int y, const std::string &text, FontVisual *visual = nullptr) override;
        int GetHeight() override;
        int GetWidth(char ch) override;

    private:
        void LoadXML();
        void LoadGlyphElement(TiXmlElement *glyph_elem);

        void ParseImage(size_t img_idx, const char *file_id, int min, int max);

        void Load() override;
        void Unload() override;

        struct Glyph {
            Glyph(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
            ~Glyph() {}

            size_t image_idx;
            int x, y, w, h;
        };

        int height; // font height
        std::vector<Glyph *> glyphs; // glyph for all ascii codes
        std::vector<Image *> images;
    };
} //ns

#endif

