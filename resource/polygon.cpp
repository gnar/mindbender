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

#include "polygon.h"
#include "manager.h"

#include "loadxml.h"

#include <cstdio>

#include <memory>
#include <iostream>
using namespace std;

namespace Res
{
	Polygon::Polygon(const Resource::ID &id) : Resource(Resource::POLYGON, id)
	{
		LoadXML();
	}

	Polygon::~Polygon()
	{
		Unload();
	}

	void Polygon::Load()
	{
		loaded = true;	
	}

	void Polygon::Unload()
	{
		loaded = false;
	}

	void Polygon::LoadXML()
	{
		std::unique_ptr<TiXmlDocument> doc(LoadXMLDocument(Manager.OpenFile(GetID())));

		TiXmlElement *polygon_elem = doc->FirstChildElement("polygon");
		TiXmlElement *vertex_elem  = polygon_elem ? polygon_elem->FirstChildElement("vertex") : 0;

		// TODO: Support holes, multiple contours
		path.addPolygon(path.suggestPolygonName());

		while (vertex_elem)
		{
			Vertex V;
			const char *pos_attr = vertex_elem->Attribute("pos");
			if (pos_attr && 2 == sscanf(pos_attr, "%i,%i", &V.x, &V.y))
			{
				path.addVertex(V);
			} else {
				cout << "Ignoring invalid <vertex ...>" << endl;
			}

			vertex_elem = vertex_elem->NextSiblingElement("vertex");
		}

		path.adjustOrientation(); // make clock-wise order
	}

	bool Polygon::Hit(int x, int y) 
	{
		int polygon;
		return path.findPolygonAt(polygon, x, y);
	}

	std::list<Vertex> Polygon::FindPath(Vertex start, Vertex goal) 
	{
		return path.findPath(start.x, start.y, goal.x, goal.y);
	}

	Polygon *CreatePolygonResource(const Resource::ID &id)
	{
		if (!Manager.ExistsFile(id)) return 0; // file not found

		std::string ext = GetFileExtension(id);
		if (ext == "polygon")
		{
			return new Polygon(id);
		}

		return 0;
	}
} //ns

