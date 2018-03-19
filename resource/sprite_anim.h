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

#ifndef RES_SPRITE_ANIM_H
#define RES_SPRITE_ANIM_H

#include "sprite.h"
#include "image.h"

#include <vector>

class TiXmlElement;

namespace Res
{
	class SpriteState_Anim : public SpriteState
	{
	public:
		SpriteState_Anim(class Sprite_Anim *sprite);
		virtual ~SpriteState_Anim();

		virtual void Update(float time_passed);
		virtual void Rewind();

		virtual void SetTrack(const std::string &track_id);
		virtual const std::string &GetTrack();

		virtual bool IsFinished() { return finished && time >= 0.0f; }

		virtual void Save(CLSerializer &S);
		virtual void Load(CLSerializer &S);
		
	private:
		friend class Sprite_Anim;

		class Sprite_Anim *sprite;
		int track_idx;   // current track
		int track_pos;   // current position in track
		int display_pos; // track display-item that 
		float time;      // for delay-statements
		bool forward;    // ..or backward playing
		bool finished;   // is animation finished?
	};

	// An animated sprite
	class Sprite_Anim : public Sprite
	{
	public:
		Sprite_Anim(const Resource::ID &id);
		virtual ~Sprite_Anim();

		virtual SpriteState *CreateSpriteState();
		virtual void Draw(int x, int y, SpriteState *state, SpriteVisual *visual);

		virtual bool HasTrack(const std::string &id);

	private:
		friend class SpriteState_Anim;

		virtual void Load();
		virtual void Unload();

		struct Rect
		{
			Rect() : full(true) {}
			Rect(int x, int y, int w, int h) : full(false), x(x), y(y), w(w), h(h) {}
			~Rect() {}

			bool full; // Use full sprite?
			int x, y, w, h;
		};
		Rect ParseRectAttribute(TiXmlElement *elem);

		struct TrackItem
		{
			enum Type { DISPLAY, DELAY } type;

			// type = DISPLAY
			Image *image;
			Rect rect; // part of image to be displayed
			int origin_x, origin_y;

			// type = DELAY
			int delay;
		};

		struct Track
		{
			std::string id;
			int default_delay;
			int default_origin_x, default_origin_y;
			enum LoopMode { REWIND, HALT, PINGPONG } loop_mode;
			std::vector<TrackItem> items;
		};

		std::vector<Track> tracks;

		void LoadXML();
		void LoadTrack(TiXmlElement *item);
		void LoadTrackItem(TiXmlElement *item, Track &track);

		void UpdateState(SpriteState_Anim &state, float time_passed);
	};
} //ns

#endif

