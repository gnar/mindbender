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

#ifndef RES_SOUND_H
#define RES_SOUND_H

#include "resource.h"

#include <memory>

struct FMOD_SOUND;
struct FMOD_CHANNEL;

namespace Res
{
	class SoundContext 
	{
	public:
		SoundContext(FMOD_SOUND *snd) : is_playing(false), sound(snd), channel(0) {}
		~SoundContext() {}
		
		void Play();
		void Stop();
		void SetVolume(int vol);  // 0..100
		void SetPanning(int pan); // -100..0..+100
		void SetLoopMode(int lm);

	private:
		bool is_playing;
		FMOD_SOUND *sound;
		FMOD_CHANNEL *channel;
	};

	class Sound : public Resource
	{
	public:
		Sound(const Resource::ID &id, bool stream);
		virtual ~Sound();

		virtual SoundContext *CreateContext()
		{
			return new SoundContext(sound);
		}

	private:
		virtual void Load();
		virtual void Unload();

		bool stream;
		FMOD_SOUND *sound;
	};

	Sound *CreateSoundResource(const Resource::ID &id);
}

#endif

