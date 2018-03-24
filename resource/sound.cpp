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

#include "sound.h"
#include "manager.h"

//#include <fmod.h>
//#include <fmod_errors.h>

using namespace std;

namespace Res {
    void SoundContext::SetVolume(int vol) {
        // vol in [0..100]
        float fmod_vol = float(vol) / 100.0f;
        //FMOD_Channel_SetVolume(channel, fmod_vol);
    }

    void SoundContext::SetPanning(int pan) {
        // pan in [-100..0..+100]
        float fmod_pan = float(pan) / 100.0f;
        //FMOD_Channel_SetPan(channel, fmod_pan);
    }

    void SoundContext::SetLoopMode(int lm) {
        // lm in {0,1}
        /*FMOD_MODE mode = 0;
        switch (lm)
        {
            case 0: mode |= FMOD_LOOP_OFF; break;
            case 1: mode |= FMOD_LOOP_NORMAL; break;
            case 2: mode |= FMOD_LOOP_BIDI; break;
        }
        FMOD_Channel_SetMode(channel, mode);*/
    }

    void SoundContext::Play() {
        if (is_playing) Stop();

        /*FMOD_RESULT result = FMOD_System_PlaySound(SoundSystem(), FMOD_CHANNEL_FREE, sound, true, &channel);
        FMOD_Channel_SetPaused(channel, false);*/
        is_playing = true;
    }

    void SoundContext::Stop() {
        if (!is_playing) return;

        //FMOD_Channel_Stop(channel);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////

    Sound::Sound(const Resource::ID &id, bool stream) : Resource(Resource::SOUND, id), stream(stream), sound(nullptr) {
        loaded = false;
    }

    Sound::~Sound() {
        Unload();
    }

    void Sound::Load() {
        if (loaded) return;
        /*if (stream) {
            FMOD_RESULT result = FMOD_System_CreateStream(SoundSystem(), GetID().c_str(), FMOD_DEFAULT, 0, &sound); // FMOD_CREATECOMPRESSEDSAMPLE??!
        } else {
            FMOD_RESULT result = FMOD_System_CreateStream(SoundSystem(), GetID().c_str(), FMOD_DEFAULT, 0, &sound);
        }*/
        loaded = true;
    }

    void Sound::Unload() {
        if (!loaded) return;

        /*FMOD_RESULT result = FMOD_Sound_Release(sound); sound = 0;
        if (result != FMOD_OK) cerr << "Could not release FMOD sound object!" << endl;
        */

        loaded = false;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////

    Sound *CreateSoundResource(const Resource::ID &id) {
        if (!Manager.ExistsFile(id)) return nullptr; // file not found

        // Set sound streamed or buffered
        /*std::string ext = GetFileExtension(id);
        if (ext == "wav")
        {
            return new Sound(id, false); // buffered
        }

        if (ext == "ogg" || ext == "mp3")
        {
            return new Sound(id, true); // streamed
        }*/

        return nullptr; // unknown file extension
    }

} //ns

