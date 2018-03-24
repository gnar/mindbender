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

#include "object/font.h"

#include "game.h"

using namespace std;

///////////////////////////////////////////////////////////
// CONSTRUCTOR/DESTRUCTOR                                //
///////////////////////////////////////////////////////////

FontSet::FontSet(CLContext *context, const std::string &res_id)
        : TableObject(context),
          x(0), y(0), r(255), g(255), b(255), a(255),
          method_print(new CLExternalFunction(context, "adv_font_print")),
          method_print_center(new CLExternalFunction(context, "adv_font_print_center")),
          method_get_length(new CLExternalFunction(context, "adv_font_get_length")),
          method_get_height(new CLExternalFunction(context, "adv_font_get_height")) {
    font_res = Res::Manager.GetFont(res_id);
}

FontSet::~FontSet() = default;

///////////////////////////////////////////////////////////
// PRINT STRINGS                                         //
///////////////////////////////////////////////////////////

void FontSet::Print(int X, int Y, const std::string &str) {
    Res::FontVisual V{};
    V.r = r;
    V.g = g;
    V.b = b;
    V.a = a;

    x = X;
    y = Y;
    font_res->Lock();
    font_res->Draw(x, y, str, &V);
    font_res->Unlock();
}

void FontSet::PrintCenter(int X, int Y, const std::string &str) {
    Res::FontVisual V{};
    V.r = r;
    V.g = g;
    V.b = b;
    V.a = a;

    x = X;
    y = Y;
    font_res->Lock();
    font_res->DrawCenter(x, y, str, &V);
    font_res->Unlock();
}

void FontSet::PrintBoxed(int X, int Y, int width, const std::string &str) {
    Res::FontVisual V{};
    V.r = r;
    V.g = g;
    V.b = b;
    V.a = a;

    x = X;
    y = Y;
    font_res->Lock();
    font_res->DrawBoxed(x, y, width, str, &V);
    font_res->Unlock();
}

void FontSet::Print(const std::string &str) {
    Print(x, y, str);
}

void FontSet::PrintCenter(const std::string &str) {
    PrintCenter(x, y, str);
}

///////////////////////////////////////////////////////////
// GET FONT INFORMATION                                  //
///////////////////////////////////////////////////////////

int FontSet::GetWidth(char ch) {
    return font_res->GetWidth(ch);
}

int FontSet::GetWidth(const std::string &str) {
    return font_res->GetWidth(str);
}

int FontSet::GetHeight() {
    return font_res->GetHeight();
}

///////////////////////////////////////////////////////////
// CLObject                                              //
///////////////////////////////////////////////////////////

void FontSet::set(CLValue &key, CLValue &val) {
    if (key.type == CL_STRING) {
        const std::string k = GET_STRING(key)->get();

        if (k == "x") {
            this->x = val.toInt();
        } else if (k == "y") {
            this->y = val.toInt();

        } else if (k == "r") {
            this->r = val.toInt();
        } else if (k == "g") {
            this->g = val.toInt();
        } else if (k == "b") {
            this->b = val.toInt();
        } else if (k == "a") {
            this->a = val.toInt();
        }
    }

    TableObject::set(key, val);
}

bool FontSet::get(CLValue &key, CLValue &val) {
    if (key.type == CL_STRING) {
        const std::string k = GET_STRING(key)->get();

        if (k == "x") {
            val = CLValue(x);
            return true;
        } else if (k == "y") {
            val = CLValue(y);
            return true;

        } else if (k == "r") {
            val = CLValue(r);
            return true;
        } else if (k == "g") {
            val = CLValue(g);
            return true;
        } else if (k == "b") {
            val = CLValue(b);
            return true;
        } else if (k == "a") {
            val = CLValue(a);
            return true;

        } else if (k == "Print") {
            val = method_print;
            return true;
        } else if (k == "PrintCenter") {
            val = method_print_center;
            return true;
        } else if (k == "GetWidth") {
            val = method_get_length;
            return true;
        } else if (k == "GetHeight") {
            val = method_get_height;
            return true;
        }
    }

    return TableObject::get(key, val);
}

void FontSet::markReferenced() // GC
{
    TableObject::markReferenced();

    method_print.markObject();
    method_print_center.markObject();
    method_get_length.markObject();
    method_get_height.markObject();
}

///////////////////////////////////////////////////////////
// SAVE & LOAD STATE                                     //
///////////////////////////////////////////////////////////
void FontSet::Save(CLSerialSaver &S, FontSet *font) {
    std::string font_res = font->font_res->GetID();
    S.IO(font_res);

    S.IO(font->x);
    S.IO(font->y);

    S.IO(font->r);
    S.IO(font->g);
    S.IO(font->b);
    S.IO(font->a);
}

FontSet *FontSet::Load(CLSerialLoader &S) {
    std::string res_id;
    S.IO(res_id);

    auto *font = new FontSet(S.getContext(), res_id);
    S.addPtr(font);

    S.IO(font->x);
    S.IO(font->y);

    S.IO(font->r);
    S.IO(font->g);
    S.IO(font->b);
    S.IO(font->a);

    return font;
}


