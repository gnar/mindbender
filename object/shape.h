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

#ifndef SHAPE_H
#define SHAPE_H

#include "cl2/cl2.h"
#include "resource/tools/polygonpath.h"
#include "resource/polygon.h"

#include "object/tableobject.h"

#include <list>
#include <string>

#define GET_SHAPE(v)           ((Shape*)(v).value.object)

class Shape : public TableObject
{
public:
	Shape(CLContext *context);
	virtual ~Shape();

	typedef std::list<Vertex> Path;

	virtual bool Hit(int x, int y) = 0;
	virtual Path FindPath(Vertex start, Vertex goal) = 0;

	virtual bool FindVertical(int x, int y, int range, int &y_result);

private:
	CLValue method_hit;
	CLValue method_find_vertical;

	// gc
	void set(CLValue &key, CLValue &val);
	bool get(CLValue &key, CLValue &val); // returns true if key existed
	void markReferenced();
};

class RectangularShape : public Shape
{
public:
	RectangularShape(CLContext *context, int pos_x, int pos_y, int width, int height);
	virtual ~RectangularShape();

	virtual bool Hit(int x, int y);
	virtual Path FindPath(Vertex start, Vertex goal);

	// SAVE & LOAD STATE //////////////////////////////////////////////
	static void Save(CLSerialSaver &S, RectangularShape *shape);
	static RectangularShape *Load(CLSerialLoader &S);

private:
	int pos_x, pos_y, width, height;

	friend class Shape;
};

class CircularShape : public Shape
{
public:
	CircularShape(CLContext *context, int pos_x, int pos_y, int radius);
	virtual ~CircularShape();

	virtual bool Hit(int x, int y);
	virtual Path FindPath(Vertex start, Vertex goal);

	// SAVE & LOAD STATE //////////////////////////////////////////////
	static void Save(CLSerialSaver &S, CircularShape *shape);
	static CircularShape *Load(CLSerialLoader &S);

private:
	int pos_x, pos_y, radius;

	friend class Shape;
};

class PolygonShape : public Shape // this shape is always loaded from a resource file
{
public:
	PolygonShape(CLContext *context, const std::string &res_id);
	virtual ~PolygonShape();

	virtual bool Hit(int x, int y);
	virtual Path FindPath(Vertex start, Vertex goal);

	// SAVE & LOAD STATE //////////////////////////////////////////////
	static void Save(CLSerialSaver &S, PolygonShape *shape);
	static PolygonShape *Load(CLSerialLoader &S);

private:
	std::string res_id;
	Res::Polygon *polygon;

	friend class Shape;
};

#endif

