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

#include "game.h"
#include "main.h"

#include <cl2/cl2.h>

#include <fstream>

#include <iostream>
using namespace std;

#include "object/shape.h"
#include "object/sprite.h"
#include "object/item.h"
#include "object/bagitem.h"
#include "object/actor.h"
#include "object/sound.h"
#include "object/font.h"
#include "object/room.h"
#include "object/timer.h"

template <class T>
static bool IsA(CLObject *obj)
{
	return (dynamic_cast<T*>(obj) != 0);
}

class SushiSerializer : public CLUserDataSerializer
{
	static const int ID_SHAPE_RECT = 0;
	static const int ID_SHAPE_CIRCLE = 1;
	static const int ID_SHAPE_POLYGON = 2;
	static const int ID_ROOM = 10;
	static const int ID_ITEM = 20;
	static const int ID_ACTOR = 21;
	static const int ID_SPRITE = 30;
	static const int ID_BAGITEM = 40;
	static const int ID_SOUND = 50;
	static const int ID_FONT = 60;
	static const int ID_TIMER = 70;

public:
	void save(CLSerialSaver &S, CLUserData *userdata) 
	{
		int tmp;

		if (IsA<RectangularShape>(userdata)) { // ID_SHAPE_RECT
			S.IO(tmp = ID_SHAPE_RECT);
			RectangularShape::Save(S, (RectangularShape*)userdata);
		} else if (IsA<CircularShape>(userdata)) { // ID_SHAPE_CIRCLE
			S.IO(tmp = ID_SHAPE_CIRCLE);
			CircularShape::Save(S, (CircularShape*)userdata);
		} else if (IsA<PolygonShape>(userdata)) { // ID_SHAPE_POLYGON
			S.IO(tmp = ID_SHAPE_POLYGON);
			PolygonShape::Save(S, (PolygonShape*)userdata);

		} else if (IsA<Room>(userdata)) { // ID_ROOM
			S.IO(tmp = ID_ROOM);
			Room::Save(S, (Room*)userdata);

		} else if (IsA<Item>(userdata)) { // ID_ITEM
			S.IO(tmp = ID_ITEM); 
			Item::Save(S, (Item*)userdata);
		} else if (IsA<Actor>(userdata)) { // ID_ACTOR
			S.IO(tmp = ID_ACTOR);
			Actor::Save(S, (Actor*)userdata);

		} else if (IsA<Sprite>(userdata)) { // ID_SPRITE
			S.IO(tmp = ID_SPRITE);
			Sprite::Save(S, (Sprite*)userdata);

		} else if (IsA<BagItem>(userdata)) { // ID_BAGITEM
			S.IO(tmp = ID_BAGITEM); 
			BagItem::Save(S, (BagItem*)userdata);

		} else if (IsA<Sound>(userdata)) { // ID_SOUND
			S.IO(tmp = ID_SOUND);
			Sound::Save(S, (Sound*)userdata);
			clog << "save: unimplemented (sound)" << endl;

		} else if (IsA<FontSet>(userdata)) { // ID_FONT
			S.IO(tmp = ID_FONT);
			FontSet::Save(S, (FontSet*)userdata);

		} else if (IsA<Timer>(userdata)) { // ID_TIMER
			S.IO(tmp = ID_TIMER);
			Timer::Save(S, (Timer*)userdata);

		} else {
			assert(0);
		}
	}

	CLUserData *load(CLSerialLoader &S)
	{
		int tmp;
		S.IO(tmp);

		switch (tmp)
		{
			case ID_SHAPE_RECT:    return RectangularShape::Load(S);
			case ID_SHAPE_CIRCLE:  return CircularShape::Load(S);
			case ID_SHAPE_POLYGON: return PolygonShape::Load(S);
			case ID_ROOM:          return Room::Load(S);
			case ID_ITEM:          return Item::Load(S);
			case ID_ACTOR:         return Actor::Load(S);
			case ID_SPRITE:        return Sprite::Load(S);
			case ID_BAGITEM:       return BagItem::Load(S);
			case ID_SOUND:         return Sound::Load(S);
			case ID_FONT:          return FontSet::Load(S);
			case ID_TIMER:         return Timer::Load(S);
			default: assert(0);
		}

		return 0;
	}
};



void Game_::Save(const std::string &filename)
{
	GarbageCollect(); // first, perform gc

	SushiSerializer my_userdata_serializer;
	std::ofstream outputfile(filename.c_str(), ios::out | ios::binary);
	CLSerialSaver S(outputfile);
	S.setUserDataSerializer(&my_userdata_serializer);

	S.magic(0x4321);

	// save frame count
	int tmp = (int)frame_count; S.IO(tmp);

	// Dump CL2 context
	context.save(S);

	// Save camera data
	camera.Save(S);

	// Save event manager data
	event_manager.Save(S);

	// save timer manager data
	timer_manager.Save(S);

	clog << "Game saved to " << filename << endl;
}

void Game_::Load(const std::string &filename)
{
	Stop();
	
	SushiSerializer my_userdata_serializer;
	std::ifstream inputfile(filename.c_str(), ios::in | ios::binary);
	CLSerialLoader S(inputfile, &context);
	S.setUserDataSerializer(&my_userdata_serializer);

	S.magic(0x4321);

	// save frame_count, restore last_time
	int tmp; S.IO(tmp); frame_count = tmp; //TODO
	last_time = GetTime();

	// Load CL2 context
	context.load(S);

	// Load camera data
	camera.Load(S);

	// Load event manager data
	event_manager.Load(S);

	// load timer manager data
	timer_manager.Load(S);

	clog << "Game loaded from " << filename << endl;

	loaded = true;
}


