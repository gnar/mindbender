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

#ifndef BAGITEM_H
#define BAGITEM_H

#include "cl2/cl2.h"
#include "object/tableobject.h"

#include <string>

#define GET_BAGITEM(v)           ((BagItem*)(v).value.object)

class BagItem : public TableObject {
public:
    explicit BagItem(CLContext *context);
    ~BagItem() override;

    void Draw(int x, int y);
    void SetName(const std::string &name_) { name = name_; }
    const std::string &GetName() { return name; }

    void SetSprite(CLValue &s) { sprite = s; }
    CLValue GetSprite() { return sprite; }

    // SAVE & LOAD STATE //////////////////////////////////////////////
    static void Save(CLSerialSaver &S, BagItem *bagitem);
    static BagItem *Load(CLSerialLoader &S);

private:
    void markReferenced() override;
    void set(CLValue &key, CLValue &val) override;
    bool get(CLValue &key, CLValue &val) override; // returns true if key existed

    std::string name;
    CLValue sprite;
};

#endif

