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

#ifndef SPRITE_H
#define SPRITE_H

#include "cl2/cl2.h"

#include "resource/manager.h"
#include "resource/sprite.h"

#include "object/tableobject.h"

#include <string>

#define GET_SPRITE(v)           ((Sprite*)(v).value.object)

class Sprite : public TableObject {
public:
    // CONSTRUCTION/DESTRUCTION //////////////////////////////////////
    Sprite(CLContext *context, const Res::Resource::ID &res);
    ~Sprite() override;

    // DRAW SPRITE ///////////////////////////////////////////////////
    void Draw(int x, int y);
    void Draw();
    void Update();
    void Rewind();

    bool IsFinished(); // is animation finished?

    void SetTrack(const std::string &track_id);
    bool HasTrack(const std::string &track_id);

    void SetScaling(float s) { this->scale = s; }
    float GetScaling() { return this->scale; }

    void Translate(int x, int y) {
        this->trans_x = x;
        this->trans_y = y;
    }

    // SAVE & LOAD STATE /////////////////////////////////////////////
    static void Save(CLSerialSaver &S, Sprite *sprite);
    static Sprite *Load(CLSerialLoader &S);

private:
    // CLObject //////////////////////////////////////////////////////
    void set(CLValue &key, CLValue &val) override;
    bool get(CLValue &key, CLValue &val) override;
    void markReferenced() override; //gc

    // PRIVATE MEMBERS ///////////////////////////////////////////////
    CLValue method_draw;
    CLValue method_rewind;
    CLValue method_is_finished;
    CLValue method_translate;

    unsigned long last_tick;
    long last_time;

    int x, y; // current sprite position
    float scale; // current sprite scaling
    int trans_x, trans_y;

    Res::Sprite *sprite_res; // the sprite resource to be used
    Res::SpriteState *sprite_state; // the sprite state
};

#endif
