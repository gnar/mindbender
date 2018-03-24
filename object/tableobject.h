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

#ifndef TABLEOBJECT_H
#define TABLEOBJECT_H

#include "cl2/cl2.h"

// Adds Table functionality to a CLUserData
class TableObject : public CLUserData {
public:
    explicit TableObject(CLContext *context);
    ~TableObject() override;

    void set(CLValue &key, CLValue &val) override;
    bool get(CLValue &key, CLValue &val) override; // returns true if key existed
    void markReferenced() override;

protected:
    // SAVE & LOAD STATE /////////////////////////////////////////////
    static void Save(CLSerialSaver &S, TableObject *tobj);
    static void Load(CLSerialLoader &S, TableObject *tobj);

private:
    CLValue table;
};

#endif

