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

#ifndef RES_IMAGE_H
#define RES_IMAGE_H

#include "resource.h"

#include <memory>
#include "dcdraw/dcdraw.h"

namespace Res {
    class Image : public Resource {
        std::unique_ptr<DCDraw::Texture> texture;

    public:
        explicit Image(const Resource::ID &id);
        ~Image() override;

        DCDraw::Texture *GetTexture() { return texture.get(); }

    private:
        void Load() override;
        void Unload() override;
    };

    Image *CreateImageResource(const Resource::ID &id);
}

#endif

