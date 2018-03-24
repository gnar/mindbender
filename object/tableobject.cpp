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

#include "object/tableobject.h"

TableObject::TableObject(CLContext *context)
        : CLUserData(context), table(new CLTable(context)) {
}

TableObject::~TableObject() = default;

void TableObject::set(CLValue &key, CLValue &val) {
    GET_TABLE(table)->set(key, val);
}

bool TableObject::get(CLValue &key, CLValue &val) {
    return GET_TABLE(table)->get(key, val);
}

void TableObject::markReferenced() {
    table.markObject();
}

//////////////////////////////////////////////////////////////////
// SAVE & LOAD STATE                                            //
//////////////////////////////////////////////////////////////////

void TableObject::Save(CLSerialSaver &S, TableObject *tobj) {
    CLValue::save(S, tobj->table);
}

void TableObject::Load(CLSerialLoader &S, TableObject *tobj) {
    tobj->table = CLValue::load(S);
}

