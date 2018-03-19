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

#include "object/bagitem.h"
#include "object/sprite.h"

#include <iostream>
using namespace std;

BagItem::BagItem(CLContext *context)
	: TableObject(context)
{
	SetName("<unnamed bag item>");
}

BagItem::~BagItem()
{
}

void BagItem::Draw(int y, int x)
{
	if (!sprite.isNull()) 
	{
		Sprite *spr = GET_SPRITE(sprite);
		spr->Draw(x, y);
	}
}

void BagItem::set(CLValue &key, CLValue &val)
{
	if (key.type == CL_STRING)
	{
		const std::string k = GET_STRING(key)->get();

		if (k == "sprite") {
			this->sprite = val; return;
		} else if (k == "name") {
			this->name = GET_STRING(val)->get(); return;
		}
	}

	TableObject::set(key, val); return;
}

bool BagItem::get(CLValue &key, CLValue &val)
{
	if (key.type == CL_STRING)
	{
		const std::string k = GET_STRING(key)->get();

		if (k == "sprite") {
			val = this->sprite; return true;
		} else if (k == "name") {
			val = CLValue(new CLString(getContext(), this->name.c_str())); return true;
		}
	}

	return TableObject::get(key, val);
}

void BagItem::markReferenced()
{
	TableObject::markReferenced();

	sprite.markObject();
}

///////////////////////////////////////////////////////////////////
// SAVE & LOAD STATE                                             //
///////////////////////////////////////////////////////////////////
void BagItem::Save(CLSerialSaver &S, BagItem *bagitem)
{
	// Name
	S.IO(bagitem->name);

	// Sprite
	CLValue::save(S, bagitem->sprite);

	// save tableobject
	TableObject::Save(S, bagitem);
}

BagItem *BagItem::Load(CLSerialLoader &S)
{
	BagItem *bagitem = new BagItem(S.getContext()); S.addPtr(bagitem);

	// Name
	std::string name;
	S.IO(name);
	bagitem->SetName(name);

	// Sprite
	bagitem->sprite = CLValue::load(S);

	// load tableobject
	TableObject::Load(S, bagitem);

	return bagitem;
}

