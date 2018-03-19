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

#include "object/room.h"
#include "object/sprite.h"
#include "object/shape.h"

#include "object/roomobject.h"
#include "object/actor.h"

#include "game.h"

#include <iostream>
using namespace std;

#include <algorithm>

namespace
{
	// Functor: Compares 2 RoomObjects by their Y-Coordinate
	struct CompareByY
	{
		bool operator()(CLValue a, CLValue b)
		{
			return (GET_ROOMOBJECT(a)->GetPositionY()) < (GET_ROOMOBJECT(b)->GetPositionY());
		}
	};
};

////////////////////////////////////////////////////////////////
// CONSTRUCTOR/DESTRUCTOR                                     //
////////////////////////////////////////////////////////////////

Room::Room(CLContext *context, int width, int height)
	: TableObject(context),
	  need_resort(true), width(width), height(height), 
	  yx_ratio(30),
	  scale_mode(SCALE_CONSTANT), scale_enabled(true),
	  scale_y_horizon(0), scale_y_base(height), scale_constant(1.0f),

	  method_objectat(new CLExternalFunction(context, "adv_room_object_at")),
	  method_addobject(new CLExternalFunction(context, "adv_room_add_object")),
	  method_remobject(new CLExternalFunction(context, "adv_room_rem_object")),

	  method_set_constant_scaling(new CLExternalFunction(context, "adv_room_set_constant_scaling")),
	  method_set_perspective_scaling(new CLExternalFunction(context, "adv_room_set_perspective_scaling")),
	  method_enable_scaling(new CLExternalFunction(context, "adv_room_enable_scaling")),
	  method_disable_scaling(new CLExternalFunction(context, "adv_room_disable_scaling"))
{
}

Room::~Room()
{
}

void Room::ChangedPosition()
{
	need_resort = true;
}

////////////////////////////////////////////////////////////////
// DRAW, UPDATE ROOM                                          //
////////////////////////////////////////////////////////////////

void Room::Draw(PlaneID p)
{
	// 1. Draw background (if any)
	if (!planes[p].background.isNull())
	{
		Sprite *backgr = GET_SPRITE(planes[p].background);
		backgr->Draw(0, 0);
	}

	// 2. Draw all objects in this room plane
	for (size_t i=0; i<NUM_LAYERS; ++i)
	{
		std::list<CLValue>::iterator it, end = planes[p].objects[i].end();
		for (it=planes[p].objects[i].begin(); it!=end; ++it) GET_ROOMOBJECT(*it)->Draw();
	}

}

void Room::DrawActorTexts(int p)
{
	for (size_t i=0; i<NUM_LAYERS; ++i)
	{
		std::list<CLValue>::iterator it, end = planes[p].objects[i].end();
		for (it=planes[p].objects[i].begin(); it!=end; ++it)
		{
			//TODO: Avoid dynamic_cast
			RoomObject *obj = GET_ROOMOBJECT(*it);
			if (dynamic_cast<Actor*>(obj)) ((Actor*)obj)->DrawText();
		}
	}
}

void Room::DebugDraw()
{
}

void Room::Update(float dt)
{
	// Update and resort objects in each plane
	for (int p=0; p<3; ++p)
	{	
		// Update all objects in all layers
		for (size_t i=0; i<NUM_LAYERS; ++i)
		{
			std::list<CLValue>::iterator it, end = planes[p].objects[i].end();
			for (it=planes[p].objects[i].begin(); it!=end; ++it) GET_ROOMOBJECT(*it)->Update(dt);
		}

		// Resort all objects in all layers by y-coordinate, if needed
		if (need_resort) for (size_t i=0; i<NUM_LAYERS; ++i)
		{
			planes[p].objects[i].sort(CompareByY());
		}
	}

	need_resort = false;
}

////////////////////////////////////////////////////////////////
// ADD/REMOVE/FIND OBJECTS INTO ROOM                          //
////////////////////////////////////////////////////////////////

void Room::AddObject(RoomObject *obj, int p)
{
	std::list<CLValue> &obj_list = planes[p].objects[obj->GetLayer()]; // Get object list for the layer

	// TODO: Check that object is not currently inserted in another room!

	// add object to list
	obj_list.push_back(CLValue(obj));

	// let object know in which room it is in
	obj->room = CLValue(this);

	// we need to resort the object list
	need_resort = true;

	//cout << "added object to room : " << obj->GetName() <<  endl;
}

void Room::RemoveObject(RoomObject *obj)
{
	for (int p=0; p<3; ++p)
	{
		for (size_t i=0; i<NUM_LAYERS; ++i)
		{
			std::list<CLValue>::iterator it, end = planes[p].objects[i].end();

			for (it=planes[p].objects[i].begin(); it!=end; ++it) if (GET_ROOMOBJECT(*it) == obj)
			{ 
				obj->room = CLValue();
				planes[p].objects[i].erase(it);
				return; 
			}
		}
	}

	clog << "Warning: Remove object failed." << endl;
}

RoomObject *Room::GetObjectAt(int x, int y)
{
	// look from front plane to back plane
	for (int p=2; p>=0; --p)
	{
		// look from top layer to bottom
		for (int l=NUM_LAYERS-1; l>=0; --l)
		{
			std::list<CLValue>::reverse_iterator it = planes[p].objects[l].rbegin(), end = planes[p].objects[l].rend();
			for (;it!=end;++it)
			{
				if (GET_ROOMOBJECT(*it)->Hit(x, y)) return GET_ROOMOBJECT(*it);
			}
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////
// ACTOR SCALING                                              //
////////////////////////////////////////////////////////////////
float Room::GetScalingAt(int x, int y, float prev_scale)
{
	float scale = prev_scale;

	if (scale_enabled) switch (scale_mode)
	{
		case SCALE_CONSTANT:
			scale = scale_constant;
			break;

		case SCALE_PERSPECTIVE:
			scale = float(y - scale_y_horizon) / float(scale_y_base - scale_y_horizon);
			break;
	}

	return scale;
}

////////////////////////////////////////////////////////////////
// CLObject                                                   //
////////////////////////////////////////////////////////////////

bool Room::get(CLValue &key, CLValue &val)
{
	if (key.type == CL_STRING)
	{
		const std::string k = GET_STRING(key)->get();
		
		if (k == "width") {
			val = CLValue(this->width); return true;
		} else if (k == "height") {
			val = CLValue(this->height); return true;
		} else if (k == "tilt") {
			val = CLValue(this->yx_ratio); return true;
		} else if (k == "background") {
			val = this->planes[1].background; return true; //TODO
		} else if (k == "path") {
			val = this->path; return true;

		} else if (k == "ObjectAt") {
			val = this->method_objectat; return true;
		} else if (k == "AddObject") {
			val = this->method_addobject; return true;
		} else if (k == "RemObject") {
			val = this->method_remobject; return true;
		} else if (k == "SetConstantScaling") {
			val = this->method_set_constant_scaling; return true;
		} else if (k == "SetPerspectiveScaling") {
			val = this->method_set_perspective_scaling; return true;
		} else if (k == "EnableScaling") {
			val = this->method_enable_scaling; return true;
		} else if (k == "DisableScaling") {
			val = this->method_disable_scaling; return true;
		}
	}

	return TableObject::get(key, val);
}

void Room::set(CLValue &key, CLValue &val)
{
	if (key.type == CL_STRING)
	{
		const std::string k = GET_STRING(key)->get();

		if (k == "width") {
			this->width = val.toInt(); return;
		} else if (k == "height") {
			this->height = val.toInt(); return;
		} else if (k == "tilt") {
			this->yx_ratio = val.toInt(); return;
		} else if (k == "background") {
			this->planes[1].background = val; return; //TODO
		} else if (k == "path") {
			this->path = val; return;

		} else if (k == "xxxxxxxxxx") { // TODO
			clog << "xxxxxxxxxx member is read-only. " << endl; return;
		}
	}

	TableObject::set(key, val); return;
}

void Room::markReferenced() // GC
{
	TableObject::markReferenced();

	for (int p=0; p<3; ++p)
	{
		planes[p].background.markObject();
		for (size_t i=0; i<NUM_LAYERS; ++i)
		{
			std::list<CLValue>::iterator 
				it = planes[p].objects[i].begin(), 
				end = planes[p].objects[i].end();
			for (;it!=end;++it) (*it).markObject();
		}
	}
	path.markObject();

	method_objectat.markObject();
	method_addobject.markObject();
	method_remobject.markObject();

	method_set_constant_scaling.markObject();
	method_set_perspective_scaling.markObject();
	method_enable_scaling.markObject();
	method_disable_scaling.markObject();
}

//////////////////////////////////////////////////////////////////
// SAVE & LOAD STATE                                            //
//////////////////////////////////////////////////////////////////
void Room::Save(CLSerialSaver &S, Room *room)
{
	/*int tmp;

	// width/height/yx_ratio of room
	S.IO(room->width);
	S.IO(room->height);
	S.IO(room->yx_ratio);

	CLValue::save(S, room->background); // the standard background sprite
	CLValue::save(S, room->path); // path of room

	// all room objects in this room, by layers
	for (size_t l=0; l<NUM_LAYERS; ++l)
	{
		CLValue::saveList(S, room->objects[l]);
	}

	S.IO(tmp = static_cast<int>(room->scale_mode));
	S.IO(tmp = static_cast<int>(room->scale_enabled));
	S.IO(room->scale_y_horizon); S.IO(room->scale_y_base);
	S.IO(room->scale_constant);

	// save tableobject
	TableObject::Save(S, room);*/
}

Room *Room::Load(CLSerialLoader &S)
{
	/*int tmp;

	// width/height/yx_ratio of room
	int width, height;
	S.IO(width);
	S.IO(height);

	Room *room = new Room(width, height); S.addPtr(room);

	S.IO(room->yx_ratio);

	// the standard background sprite
	room->background = CLValue::load(S);

	// path of room
	room->path = CLValue::load(S);

	// all room objects in this room, by layers
	for (size_t l=0; l<NUM_LAYERS; ++l)
	{
		room->objects[l] = CLValue::loadList(S);
	}

	S.IO(tmp); room->scale_mode = static_cast<ScaleMode>(tmp);
	S.IO(tmp); room->scale_enabled = static_cast<bool>(tmp);
	S.IO(room->scale_y_horizon); S.IO(room->scale_y_base);
	S.IO(room->scale_constant);

	// load tableobject
	TableObject::Load(S, room);

	return room;*/
	return 0;
}



