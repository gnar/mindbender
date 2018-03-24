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

#include "object/sound.h"
#include "game.h"

using namespace std;

//////////////////////////////////////////////////////////////////
// CONSTRUCTION/DESTRUCTION                                     //
//////////////////////////////////////////////////////////////////

Sound::Sound(CLContext *context, const std::string &res_id)
        : TableObject(context),
          method_play(new CLExternalFunction(context, "adv_sound_play")),
          method_stop(new CLExternalFunction(context, "adv_sound_stop")),
          sound_res(Res::Manager.GetSound(res_id)) {
    sound_ctx = nullptr;
    volume = 100;
    panning = 0;
    loop_mode = 0;
}

Sound::~Sound() {
    Stop();
}

void Sound::Update(float dt) {
}

//////////////////////////////////////////////////////////////////
// PLAY SOUND                                                   //
//////////////////////////////////////////////////////////////////

void Sound::Play() {
    if (sound_ctx) Stop();

    sound_res->Lock();
    sound_ctx = sound_res->CreateContext();
    sound_ctx->Play();

    UpdateVolume();
    UpdatePanning();
    UpdateLooping();
}

void Sound::Stop() {
    if (!sound_ctx) return;

    sound_ctx->Stop();
    delete sound_ctx;
    sound_ctx = nullptr;
    sound_res->Unlock();
}

//////////////////////////////////////////////////////////////////
// CLObject                                                     //
//////////////////////////////////////////////////////////////////

void Sound::markReferenced() // GC
{
    TableObject::markReferenced();

    method_play.markObject();
    method_stop.markObject();
}

void Sound::set(CLValue &key, CLValue &val) {
    if (key.type == CL_STRING) {
        const std::string k = GET_STRING(key)->get();

        // Methods
        if (k == "Play") {
            return; //TODO
        } else if (k == "Stop") {
            return; //TODO

            // Properties
        } else if (k == "panning") {
            SetPanning(val.toInt());
            UpdatePanning();
            return;
        } else if (k == "volume") {
            SetVolume(val.toInt());
            UpdateVolume();
            return;
        } else if (k == "loop") {
            SetLoopMode(val.toInt());
            UpdateLooping();
            return;
        } /*else if (k == "mute") {
			//this->mute_intensity = val.toInt();
			//UpdateMuteFilter();
			return;
		} else if (k == "echo") {
			//this->echo_intensity = val.toInt();
			//UpdateEchoFilter();
			return;
		}*/
    }

    TableObject::set(key, val);
}

bool Sound::get(CLValue &key, CLValue &val) {
    if (key.type == CL_STRING) {
        const std::string k = GET_STRING(key)->get();

        // Methods
        if (k == "Play") {
            val = this->method_play;
            return true;
        } else if (k == "Stop") {
            val = this->method_stop;
            return true;

            // Properties
        } else if (k == "panning") {
            val = CLValue(GetPanning());
            return true;
        } else if (k == "volume") {
            val = CLValue(GetVolume());
            return true;
        } else if (k == "loop") {
            val = CLValue(GetLoopMode());
            return true;
        }/* else if (k == "mute") {
			//val = CLValue(this->mute_intensity);
			return true;
		} else if (k == "echo") {
			//val = CLValue(this->echo_intensity);
			return true;
		}*/
    }

    return TableObject::get(key, val);
}

void Sound::SetVolume(int v) {
    if (v < 0) v = 0;
    if (v > 100) v = 100;
    this->volume = v;
}

void Sound::SetPanning(int p) {
    if (p < -100) p = -100;
    if (p > +100) p = +100;
    this->panning = p;
}

void Sound::SetLoopMode(int m) {
    if (m < 0) m = 0;
    if (m > 2) m = 0;
    this->loop_mode = m;
}

void Sound::UpdateVolume() {
    if (!sound_ctx) return;
    sound_ctx->SetVolume(volume);
}

void Sound::UpdatePanning() {
    if (!sound_ctx) return;
    sound_ctx->SetPanning(panning);
}

void Sound::UpdateLooping() {
    if (!sound_ctx) return;
    sound_ctx->SetLoopMode(loop_mode);
}

//////////////////////////////////////////////////////////////////
// SAVE & LOAD STATE                                            //
//////////////////////////////////////////////////////////////////
void Sound::Save(CLSerialSaver &S, Sound *sound) {
    // resource id
    std::string res_id = sound->sound_res->GetID();
    S.IO(res_id);

    // save tableobject data
    TableObject::Save(S, sound);
}

Sound *Sound::Load(CLSerialLoader &S) {
    // resource id
    std::string res_id;
    S.IO(res_id);

    auto *sound = new Sound(S.getContext(), res_id);
    S.addPtr(sound);

    // load tableobject data
    TableObject::Load(S, sound);

    return sound;
}


