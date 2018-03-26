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

#ifndef ROOM_OBJECT_H
#define ROOM_OBJECT_H

#include "cl2/cl2.h"
#include "object/tableobject.h"

#define GET_ROOMOBJECT(v)           ((RoomObject*)(v).value.object)

// base class for Actor and Item

class RoomObject : public TableObject {
    friend class Room;

public:
    explicit RoomObject(CLContext *context);
    ~RoomObject() override;

    inline const std::string &GetName() { return name; }
    inline void SetName(const std::string &new_name) { name = new_name; }

    inline void SetHotspot(CLValue &spot) { hotspot = spot; }
    inline CLValue GetHotspot() { return hotspot; }

    inline void SetPosition(int X, int Y) {
        x = X;
        y = Y;
    }

    inline void SetPositionX(int X) { x = X; }
    void SetPositionY(int Y);
    inline int GetPositionX() { return x; }
    inline int GetPositionY() { return y; }

    inline CLValue GetRoom() { return room; }

    size_t GetLayer();
    void SetLayer(size_t l);

    bool Hit(int X, int Y);

    virtual void Update(float dt) {}
    virtual void Draw() = 0;
    virtual void DebugDraw() = 0;

    void AddToInventory(CLValue bagitem);
    void RemoveFromInventory(CLValue bagitem);
    bool HasInInventory(CLValue bagitem);

    void Hide(bool yes = true) { this->hidden = yes; }
    bool IsHidden() { return this->hidden; }

    // SAVE & LOAD STATE /////////////////////////////////////////////
    static void Save(CLSerialSaver &S, RoomObject *obj);
    static void Load(CLSerialLoader &S, RoomObject *obj);

protected:
    void markReferenced() override;
    void set(CLValue &key, CLValue &val) override;
    bool get(CLValue &key, CLValue &val) override; // returns true if key existed

private:
    // room object properties
    int x, y;
    size_t layer;
    std::string name;
    CLValue hotspot;
    bool hidden;

    // the room that contains this object
    CLValue room;
    std::list<CLValue> inventory; // the belongings of this actor/item

    // a cached list of the inventory
    CLValue cached_inventory;
    void RebuildCachedInventory();

    // methods
    CLValue method_add_bagitem;
    CLValue method_rem_bagitem;
    CLValue method_has_bagitem;
};

#endif

