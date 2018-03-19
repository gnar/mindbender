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

#include "game.h"
#include "object/shape.h"
#include "resource/manager.h"

#include <iostream>
using namespace std;

Shape::Shape(CLContext *context) 
	: TableObject(context),
	  method_hit(new CLExternalFunction(context, "adv_shape_hit")),
	  method_find_vertical(new CLExternalFunction(context, "adv_shape_find_vertical"))
{
}

Shape::~Shape()
{
}

bool Shape::FindVertical(int x, int y, int range, int &y_result)
{
	// TODO: Specialise this for rectangular, circular shapes!!

	if (Hit(x, y))
	{
		y_result = y;
		return true;
	} else {
		// scan
		for (int s=4; s<range; s += 4)
		{
			if (Hit(x, y+s)) { y_result = y+s; return true; }
			if (Hit(x, y-s)) { y_result = y-s; return true; }
		}
		return false;
	}
}

//////////////////////////////////////////////////////////////////
// CLObject                                                     //
//////////////////////////////////////////////////////////////////

void Shape::set(CLValue &key, CLValue &val)
{
	if (key.type == CL_STRING)
	{
		// TODO: Error messages
		const std::string k = GET_STRING(key)->get();
		if (k == "Hit") {
			return;
		} else if (k == "FindVertical") {
			return;
		} 
	}

	TableObject::set(key, val);
}

bool Shape::get(CLValue &key, CLValue &val)
{
	if (key.type == CL_STRING)
	{
		const std::string k = GET_STRING(key)->get();
		if (k == "Hit") {
			val = method_hit; return true;
		} else if (k == "FindVertical") {
			val = method_find_vertical; return true;
		} 
	}

	return TableObject::get(key, val);
}

void Shape::markReferenced()
{
	TableObject::markReferenced();

	method_hit.markObject();
	method_find_vertical.markObject();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Rectangular shapes                                                                     //
////////////////////////////////////////////////////////////////////////////////////////////

RectangularShape::RectangularShape(CLContext *context, int pos_x, int pos_y, int width, int height)
	: Shape(context),
	  pos_x(pos_x), pos_y(pos_y), width(width), height(height)
{
}

RectangularShape::~RectangularShape()
{
}


bool RectangularShape::Hit(int x, int y)
{
	if ((x < pos_x) || (x >= pos_x + width)) return false;
	if ((y < pos_y) || (y >= pos_y + height)) return false;
	return true;
}

Shape::Path RectangularShape::FindPath(Vertex start, Vertex goal)
{
	std::list<Vertex> result;
	result.push_back(start);
	result.push_back(goal);
	return result;
}

void RectangularShape::Save(CLSerialSaver &S, RectangularShape *shape)
{
	S.IO(shape->pos_x);
	S.IO(shape->pos_y);
	S.IO(shape->width);
	S.IO(shape->height);
	
	TableObject::Save(S, shape); // save tableobject data
}

RectangularShape *RectangularShape::Load(CLSerialLoader &S)
{
	int pos_x, pos_y, width, height;
	S.IO(pos_x); S.IO(pos_y); 
	S.IO(width); S.IO(height);

	RectangularShape *shape = new RectangularShape(S.getContext(), pos_x, pos_y, width, height); S.addPtr(shape);
	TableObject::Load(S, shape); // load tableobject data
	return shape;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Circular shapes                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

CircularShape::CircularShape(CLContext *context, int pos_x, int pos_y, int radius)
	: Shape(context),
	  pos_x(pos_x), pos_y(pos_y), radius(radius)
{
}

CircularShape::~CircularShape()
{
}

bool CircularShape::Hit(int x, int y)
{
	int dx = (pos_x - x) * (pos_x - x);
	int dy = (pos_y - y) * (pos_y - y);
	return ((dx + dy) < (radius * radius));
}

Shape::Path CircularShape::FindPath(Vertex start, Vertex goal)
{
	std::list<Vertex> result;
	result.push_back(start);
	result.push_back(goal);
	return result;
}

void CircularShape::Save(CLSerialSaver &S, CircularShape *shape)
{
	S.IO(shape->pos_x);
	S.IO(shape->pos_y);
	S.IO(shape->radius);
	
	TableObject::Save(S, shape); // save tableobject data
}

CircularShape *CircularShape::Load(CLSerialLoader &S)
{
	int pos_x, pos_y, radius;
	S.IO(pos_x); S.IO(pos_y); 
	S.IO(radius);

	CircularShape *shape = new CircularShape(S.getContext(), pos_x, pos_y, radius); S.addPtr(shape);
	TableObject::Load(S, shape); // load tableobject data
	return shape;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Polygonal shapes                                                                       //
////////////////////////////////////////////////////////////////////////////////////////////

PolygonShape::PolygonShape(CLContext *context, const std::string &str_id)
	: Shape(context),
	  res_id(str_id)
{
	polygon = Res::Manager.GetPolygon(str_id);
	polygon->Lock();
}

PolygonShape::~PolygonShape()
{
	polygon->Unlock();
}

bool PolygonShape::Hit(int x, int y)
{
	return polygon->Hit(x, y);
}

Shape::Path PolygonShape::FindPath(Vertex start, Vertex goal)
{
	return polygon->FindPath(start, goal);
}

void PolygonShape::Save(CLSerialSaver &S, PolygonShape *shape)
{
	S.IO(shape->res_id);

	TableObject::Save(S, shape); // save tableobject data
}

PolygonShape *PolygonShape::Load(CLSerialLoader &S)
{
	std::string res_id;
	S.IO(res_id);

	PolygonShape *shape = new PolygonShape(S.getContext(), res_id); S.addPtr(shape);
	TableObject::Load(S, shape); // load tableobject data
	return shape;
}


