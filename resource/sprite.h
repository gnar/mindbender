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

#ifndef RES_SPRITE_H
#define RES_SPRITE_H

#include "resource.h"

#include <cl2/cl2.h>

namespace Res {
    // Abstract; Contains and modifies sprite animation state
    class SpriteState {
    public:
        SpriteState() = default;
        virtual ~SpriteState() = default;

        virtual void Update(float time_passed) = 0;
        virtual void Rewind() = 0;

        virtual void SetTrack(const std::string &track) = 0;
        virtual const std::string &GetTrack() = 0;

        virtual bool IsFinished() = 0;

        virtual void Save(CLSerializer &S) = 0;
        virtual void Load(CLSerializer &S) = 0;
    };

    // Contains info on how to display sprite
    // (Tinting, Blending, ...)
    class SpriteVisual {
    public:
        float scale;
        //float R, G, B, A; // elem [0,1]
    };

    class Sprite : public Resource {
    public:
        explicit Sprite(const Resource::ID &id);
        virtual ~Sprite() = default;

        virtual SpriteState *CreateSpriteState() = 0;
        virtual void Draw(int x, int y, SpriteState *state, SpriteVisual *visual = nullptr) = 0;
        virtual bool HasTrack(const std::string &id) = 0;

    private:
        virtual void Load() = 0;
        virtual void Unload() = 0;
    };

    Sprite *CreateSpriteResource(const Resource::ID &id);
} //ns

#endif

