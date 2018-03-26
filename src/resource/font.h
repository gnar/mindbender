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

#ifndef RES_FONT_H
#define RES_FONT_H

#include "resource.h"

namespace Res {
    // Contains info on how to display font
    // (Tinting, Blending, ...)
    class FontVisual {
    public:
        int r, g, b, a; // [0..255]
    };

    class Font : public Resource {
    public:
        explicit Font(const Resource::ID &id);
        virtual ~Font();

        virtual void Draw(int x, int y, const std::string &text, FontVisual *visual = nullptr) = 0;
        virtual void DrawCenter(int x, int y, const std::string &text, FontVisual *visual = nullptr);
        virtual void DrawBoxed(int x, int y, int width, const std::string &str, FontVisual *visual = nullptr);

        virtual int GetHeight() = 0; // get height in pixels
        virtual int GetWidth(char ch) = 0; // get width in pixels

        virtual int GetSpaceWidth() { return 1; } // get space between characters
        virtual int GetWidth(const std::string &text);

    private:
        virtual void Load() = 0;
        virtual void Unload() = 0;
    };

    Font *CreateFontResource(const Resource::ID &id);
} //ns

#endif

