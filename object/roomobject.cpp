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

#include "object/roomobject.h"
#include "object/room.h"
#include "object/shape.h"

using namespace std;

RoomObject::RoomObject(CLContext *context)
        : TableObject(context),
          x(0), y(0), layer(1), name("<unnamed room object>"), hidden(false),
          method_add_bagitem(new CLExternalFunction(context, "adv_roomobject_add_bagitem")),
          method_rem_bagitem(new CLExternalFunction(context, "adv_roomobject_rem_bagitem")),
          method_has_bagitem(new CLExternalFunction(context, "adv_roomobject_has_bagitem")) {
    RebuildCachedInventory();
}

RoomObject::~RoomObject() = default;

bool RoomObject::Hit(int X, int Y) {
    if (IsHidden()) return false;

    // do we have a hotspot?
    if (!hotspot.isNull()) {
        if (GET_SHAPE(hotspot)->Hit(X - x, Y - y)) return true;
    }

    return false;
}

size_t RoomObject::GetLayer() {
    return this->layer;
}

void RoomObject::SetLayer(size_t l) {
    if ((l < 0) || (l >= NUM_LAYERS)) {
        cout << "Warning: Layer selection out of range : " << l << endl;
        if (l < 0) l = 0;
        if (l >= NUM_LAYERS) l = NUM_LAYERS - 1;
    }

    if (this->layer == l) return;

    // Remove from room, change layer, add to room again, or just set layer if not in room
    if (!room.isNull()) {
        Room *room = GET_ROOM(this->room);
        room->RemoveObject(this);
        this->layer = l;
        room->AddObject(this);
    } else {
        this->layer = l;
    }
}

void RoomObject::SetPositionY(int Y) {
    if ((y != Y) && (!room.isNull())) GET_ROOM(room)->ChangedPosition();
    y = Y;
}

void RoomObject::set(CLValue &key, CLValue &val) {
    if (key.type == CL_STRING) {
        const std::string k = GET_STRING(key)->get();

        // properties
        if (k == "x") {
            this->x = val.toInt();
            return;
        } else if (k == "y") {
            this->y = val.toInt();
            return;
        } else if (k == "layer") {
            SetLayer(val.toInt());
            return;
        } else if (k == "hide") {
            Hide(val.toBool());
            return;
        } else if (k == "name") {
            this->name = GET_STRING(val)->get();
            return;
        } else if (k == "room") { //ro
            clog << "<actor/item>.room is read-only!" << endl;
            return;
        } else if (k == "hotspot") {
            this->hotspot = val;
            return;
        } else if (k == "bag") { //ro
            clog << "<actor/item>.bag is read-only!" << endl;
            return;

            // methods, all ro
        } else if (k == "AddBagItem") { //ro
            clog << "<actor/item>.AddBagItem is read-only!" << endl;
            return;
        } else if (k == "RemBagItem") { //ro
            clog << "<actor/item>.RemBagItem is read-only!" << endl;
            return;
        } else if (k == "HasBagItem") { //ro
            clog << "<actor/item>.HasBagItem is read-only!" << endl;
            return;
        }
    }

    TableObject::set(key, val);
}

bool RoomObject::get(CLValue &key, CLValue &val) // returns true if key existed
{
    if (key.type == CL_STRING) {
        const std::string k = GET_STRING(key)->get();

        // properties
        if (k == "x") {
            val = CLValue(this->x);
            return true;
        } else if (k == "y") {
            val = CLValue(this->y);
            return true;
        } else if (k == "layer") {
            val = CLValue(static_cast<int>(GetLayer()));
            return true;
        } else if (k == "hide") {
            val = IsHidden() ? CLValue::True() : CLValue::False();
            return true;
        } else if (k == "name") {
            val = CLValue(new CLString(getContext(), name.c_str()));
            return true;
        } else if (k == "room") {
            val = this->room;
            return true;
        } else if (k == "hotspot") {
            val = this->hotspot;
            return true;
        } else if (k == "bag") {
            val = this->cached_inventory;
            return true;

            // methods
        } else if (k == "AddBagItem") {
            val = method_add_bagitem;
            return true;
        } else if (k == "RemBagItem") {
            val = method_rem_bagitem;
            return true;
        } else if (k == "HasBagItem") {
            val = method_has_bagitem;
            return true;
        }
    }

    return TableObject::get(key, val);
}

////////////////////////////////////////////////////////////////////
// Inventory handling                                             //
////////////////////////////////////////////////////////////////////

bool RoomObject::HasInInventory(CLValue bagitem) {
    std::list<CLValue>::iterator it, end = inventory.end();
    for (it = inventory.begin(); it != end; ++it) {
        if (bagitem.isEqual(*it)) return true;
    }
    return false;
}

void RoomObject::AddToInventory(CLValue bagitem) {
    if (HasInInventory(bagitem)) return;

    inventory.push_back(bagitem);
    RebuildCachedInventory();
}

void RoomObject::RemoveFromInventory(CLValue bagitem) {
    std::list<CLValue>::iterator it, end = inventory.end();
    for (it = inventory.begin(); it != end; ++it) {
        if (bagitem.isEqual(*it)) {
            inventory.erase(it);
            RebuildCachedInventory();
            return;
        }
    }
}

void RoomObject::RebuildCachedInventory() {
    cached_inventory = CLValue(new CLArray(getContext()));
    std::list<CLValue>::iterator it, end = inventory.end();

    int idx = 0;
    for (it = inventory.begin(); it != end; ++it, ++idx) {
        cached_inventory.set(CLValue(idx), *it);
    }
}

// GC
void RoomObject::markReferenced() {
    TableObject::markReferenced();

    room.markObject();
    hotspot.markObject();
    cached_inventory.markObject();

    method_add_bagitem.markObject();
    method_rem_bagitem.markObject();
    method_has_bagitem.markObject();

    std::list<CLValue>::iterator it, end = inventory.end();
    for (it = inventory.begin(); it != end; ++it) (*it).markObject();
}

//////////////////////////////////////////////////////////////////
// SAVE & LOAD STATE                                            //
//////////////////////////////////////////////////////////////////
void RoomObject::Save(CLSerialSaver &S, RoomObject *obj) {
    unsigned int utmp;

    S.IO(obj->x);
    S.IO(obj->y);
    S.IO(utmp = static_cast<unsigned int>(obj->layer));
    S.IO(utmp = static_cast<unsigned int>(obj->hidden));

    S.IO(obj->name);

    CLValue::save(S, obj->hotspot);
    CLValue::save(S, obj->room);

    CLValue::saveList(S, obj->inventory);
}

void RoomObject::Load(CLSerialLoader &S, RoomObject *obj) {
    unsigned int utmp;

    S.IO(obj->x);
    S.IO(obj->y);
    S.IO(utmp);
    obj->layer = static_cast<size_t>(utmp);
    S.IO(utmp);
    obj->hidden = utmp;

    S.IO(obj->name);

    obj->hotspot = CLValue::load(S);
    obj->room = CLValue::load(S);

    obj->inventory = CLValue::loadList(S);
    obj->RebuildCachedInventory();
}


