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

#ifndef RES_SPRITE_SIMPLE_H
#define RES_SPRITE_SIMPLE_H

#include "sprite.h"

namespace Res
{
	class SpriteState_Simple : public SpriteState
	{
		virtual void Update(float time_passed) {}
		virtual void Rewind() {}

		virtual void SetTrack(const std::string &track) {}
		virtual const std::string &GetTrack() { static std::string d("default"); return d; }

		virtual bool IsFinished() { return false; } // a still sprite continues to display forever by definition

		virtual void Save(CLSerializer &S) {}
		virtual void Load(CLSerializer &S) {}
	};

	// A simple sprite with one frame only
	// (Just a wrapper around the Image resource)
	class Sprite_Simple : public Sprite
	{
	public:
		Sprite_Simple(const Resource::ID &id);
		virtual ~Sprite_Simple();

		virtual SpriteState *CreateSpriteState();
		virtual void Draw(int x, int y, SpriteState *state, SpriteVisual *visual);
		virtual bool HasTrack(const std::string &id) { return id == "default"; }

	private:
		virtual void Load();
		virtual void Unload();

		class Image *image;
	};
} //ns

#endif

