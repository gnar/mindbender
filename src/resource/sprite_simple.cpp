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

#include "sprite_simple.h"
#include "manager.h"
#include "image.h"

namespace Res {
    Sprite_Simple::Sprite_Simple(const Resource::ID &id) : Sprite(id) {
        image = Manager.GetImage(id);
        assert(image);
    }

    Sprite_Simple::~Sprite_Simple() = default;

    void Sprite_Simple::Load() {
        loaded = true;
    }

    void Sprite_Simple::Unload() {
        loaded = false;
    }

    SpriteState *Sprite_Simple::CreateSpriteState() {
        return new SpriteState_Simple();
    }

    void Sprite_Simple::Draw(int x, int y, SpriteState *state, SpriteVisual *visual) {
        DCDraw::Transform t;
        t.SetTrans(x, y);
        if (visual) t.SetScale(visual->scale, visual->scale);

        image->Lock();
        image->GetTexture()->Draw(t);
        image->Unlock();
    }
} //ns


