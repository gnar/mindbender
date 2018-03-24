/*
  Copyright (c) 2002 Gunnar Selke <gunnar@gmx.info>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

#ifndef POLYGONPATH_H
#define POLYGONPATH_H

#include <string>
#include <vector>
#include <list>

struct Vertex
{
        int x, y;

	Vertex() : x(0), y(0) {}

        Vertex(int X, int Y)
        {
                x = X; y = Y;
        }
};

class PolygonPath
{
public:
	PolygonPath();
    void clear();

	std::vector<std::string> getPolygonNames() const;
	std::string suggestPolygonName() const; // returns a free name: "Polygon00" etc.

	int getPolygon(const std::string &name) const;
	bool hasPolygon(const std::string &name) const;

	void select(int poly, int outline = 0, int point = 0);

	void addPolygon(const std::string &name);
	void addHole();
	void addVertex(int X, int Y);
	void addVertex(const Vertex &v);

	void movePolygonRel(int dx, int dy);
	void moveOutlineRel(int dx, int dy);
	void moveVertexRel(int dx, int dy);

        void moveVertex(int X, int Y);

        void adjustOrientation();        // make all outlines clockwise order

	bool findVertexAt(int X, int Y, int &polygon, int &outline, int &vertice);
	bool findSegmentAt(int X, int Y, int &polygon, int &outline, int &vertice);
	bool findOutlineAt(int X, int Y, int &polygon, int &outline);
	bool findPolygonAt(int &polygon, int X, int Y) const;

	std::list<Vertex> findPath(int startX, int startY, int goalX, int goalY) const;

private:
        struct Polygon
        {
		std::string name;                         // Polygon name

		struct Outline                            // Either the main polygon or a hole inside it
		{
                	std::vector<Vertex> points;
			bool is_hole;
		};
		std::vector<Outline> outlines;

		struct PathGraphEdge
		{
			int other_node;			// this references to path_graph[other_node]
			int distance;
		};

		struct PathGraphNode
		{
			int outline, node;
			std::vector<PathGraphEdge> edges;
		};

		mutable std::vector<PathGraphNode> path_graph;
		mutable bool valid_pathgraph; // Path graph is valid/up to date ??
	};

	typedef std::vector<Polygon> PolygonField;

	PolygonField polys;

	void addPathGraphEdge(int polygon, int pg_node1, int pg_node2) const;
	void calculateVisibilityGraph(int polygon) const;

	bool is_relevant(int polygon, int outline, int node) const;
	int nodeDistance(int polygon, int outl1, int node1, int outl2, int node2) const;
	bool lineOfSight(int polygon, int x1, int y1, int x2, int y2, int ign_outl1 = -1, int ign_vert1 = -1, int ign_outl2 = -1, int ign_vert2 = -1) const;

	std::vector<Polygon::PathGraphEdge> getPointVisibility(int polygon, int X, int Y) const; // helper for findPath

	int sel_poly, sel_outline, sel_vertice;
};

#endif

