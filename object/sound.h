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

#ifndef SOUND_H
#define SOUND_H

#include "cl2/cl2.h"
#include "object/tableobject.h"
#include "resource/manager.h"
#include "resource/sound.h"

#include <memory>
#include <string>

#define GET_SOUND(v)           ((Sound*)(v).value.object)

class Sound : public TableObject {
public:
    // CONSTRUCTION/DESTRUCTION //////////////////////////////////////
    Sound(CLContext *context, const std::string &res_id);
    ~Sound() override;

    void Update(float dt);

    // PLAY SOUND/ ///////////////////////////////////////////////////
    void Play();
    void Stop();

    void SetVolume(int v); // 0..100
    void SetPanning(int p); // -100..100
    void SetLoopMode(int m); // 0,1

    int GetVolume() { return volume; }
    int GetPanning() { return panning; }
    int GetLoopMode() { return loop_mode; }

    // SAVE & LOAD STATE /////////////////////////////////////////////
    static void Save(CLSerialSaver &S, Sound *sound);
    static Sound *Load(CLSerialLoader &S);

private:
    // CLObject //////////////////////////////////////////////////////
    void markReferenced() override; // GC
    void set(CLValue &key, CLValue &val) override;
    bool get(CLValue &key, CLValue &val) override;

    // PRIVATE ///////////////////////////////////////////////////////
private:
    CLValue method_play;
    CLValue method_stop;

    Res::Sound *sound_res;
    Res::SoundContext *sound_ctx;

    int volume;
    int panning;
    int loop_mode;

    void UpdateVolume();
    void UpdatePanning();
    void UpdateLooping();
};

#endif

