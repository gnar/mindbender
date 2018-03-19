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

#include "object/item.h"
#include "object/sprite.h"

#include "object/shape.h"

#include <iostream>
using namespace std;

Item::Item(CLContext *context)
	: RoomObject(context)
{
	SetName("<unnamed item>");
	SetPosition(0, 0);
}

Item::~Item()
{
}

void Item::Draw()
{
	if (IsHidden()) return;

	if (!on_draw.isNull()) { // Drawing overridden by user?
		CLThread *T = new CLThread(getContext());
		T->init(on_draw, std::vector<CLValue>(), CLValue(this));	
		T->enableYield(false);
		T->run();
	} else if (!sprite.isNull()) { // Else, draw default sprite if available
		Sprite *spr = GET_SPRITE(sprite);
		spr->Draw(GetPositionX(), GetPositionY());
	}
}

void Item::DebugDraw()
{
}

void Item::set(CLValue &key, CLValue &val)
{
	if (key.type == CL_STRING)
	{
		const std::string k = GET_STRING(key)->get();

		if (k == "sprite") {
			this->sprite = val; return;
		} else if (k == "OnDraw") {
			this->on_draw = val; return;
		}
	}

	RoomObject::set(key, val); return;
}

bool Item::get(CLValue &key, CLValue &val)
{
	if (key.type == CL_STRING)
	{
		const std::string k = GET_STRING(key)->get();

		if (k == "sprite") {
			val = this->sprite; return true;
		} else if (k == "OnDraw") {
			val = this->on_draw; return true;
		}
	}

	return RoomObject::get(key, val);
}

void Item::markReferenced()
{
	RoomObject::markReferenced();

	sprite.markObject();
	on_draw.markObject();
}

///////////////////////////////////////////////////////////////////
// SAVE & LOAD STATE                                             //
///////////////////////////////////////////////////////////////////

void Item::Save(CLSerialSaver &S, Item *item)
{
	CLValue::save(S, item->sprite);
	CLValue::save(S, item->on_draw);

	RoomObject::Save(S, item); // save roomobject
	TableObject::Save(S, item); // save tableobject
}

Item *Item::Load(CLSerialLoader &S)
{
	Item *item = new Item(S.getContext()); S.addPtr(item);

	item->sprite = CLValue::load(S);
	item->on_draw = CLValue::load(S);

	RoomObject::Load(S, item); // load roomobject
	TableObject::Load(S, item); // load tableobject

	return item;
}



