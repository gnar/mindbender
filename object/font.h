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

#ifndef FONT_H
#define FONT_H

#include "cl2/cl2.h"

#include "object/tableobject.h"

#include "resource/manager.h"
#include "resource/font.h"

#include <string>

#define GET_FONT(v)           ((FontSet*)(v).value.object)

// 'Font' collides with a X-Windows typedef, so we call it FontSet
class FontSet : public TableObject
{
public:
	// CONSTRUCTOR/DESTRUCTOR /////////////////////////////////
	FontSet(CLContext *context, const std::string &res_id);
	virtual ~FontSet();

	// PRINT STRINGS //////////////////////////////////////////
	void Print(int X, int Y, const std::string &str);
	void PrintCenter(int X, int Y, const std::string &str);
	void PrintBoxed(int X, int Y, int width, const std::string &str);
	void Print(const std::string &str);
	void PrintCenter(const std::string &str);

	// GET FONT INFORMATION ///////////////////////////////////
	int GetWidth(char ch);
	int GetWidth(const std::string &str);
	int GetHeight();

	// SAVE & LOAD STATE //////////////////////////////////////
	static void Save(CLSerialSaver &S, FontSet *font);
	static FontSet *Load(CLSerialLoader &S);

private:
	// CLObject ///////////////////////////////////////////////
	void markReferenced(); // GC
	void set(CLValue &key, CLValue &val);
	bool get(CLValue &key, CLValue &val);

	// PRIVATE MEMBERS ////////////////////////////////////////
private:
	int x, y;
	int r, g, b, a; // color
	Res::Font *font_res;

	CLValue method_print;
	CLValue method_print_center;
	CLValue method_get_length;
	CLValue method_get_height;
};

#endif

