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

#ifndef RES_POLYGON_H
#define RES_POLYGON_H

#include "resource.h"
#include "resource/tools/polygonpath.h"

#include <list>

namespace Res
{
	class Polygon : public Resource
	{
	public:
		Polygon(const Resource::ID &id);
		virtual ~Polygon();

		bool Hit(int x, int y);
		std::list<Vertex> FindPath(Vertex start, Vertex goal);

	private:
		void LoadXML();

		virtual void Load();
		virtual void Unload();

		PolygonPath path;
	};

	Polygon *CreatePolygonResource(const Resource::ID &id);
} //ns

#endif
