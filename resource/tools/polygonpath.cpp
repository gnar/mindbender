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

#include "polygonpath.h"
#include "tools.h"

#include <cmath>
#include <limits>
#include <algorithm>

#include <assert.h>

#include <iostream>
using namespace std;

using namespace GeometryTools;

#ifdef WIN32
#pragma warning(disable:4244) // konvertierung von a nach b
#pragma warning(disable:4018) // size_t vs. int
#endif

PolygonPath::PolygonPath()
{
	clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Add/Modify path objects                                                                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PolygonPath::clear()
{
        polys.clear();
	select(-1, -1, -1);
}

bool PolygonPath::hasPolygon(const std::string &name) const
{
	return getPolygon(name) != -1;
}

int PolygonPath::getPolygon(const std::string &name) const
{
	int i;
	for (i=0; i<polys.size(); ++i) if (polys[i].name == name) return i;
	return -1;
}

std::vector<std::string> PolygonPath::getPolygonNames() const
{
	int i;
	std::vector<std::string> result;
	for (i=0; i<polys.size(); ++i) result.push_back(polys[i].name);
	return result;
}

std::string PolygonPath::suggestPolygonName() const
{
	int i = 0;
	char *name = new char[16];
	do
	{
		std::sprintf(name, "Polygon%i", i++);
	} while (hasPolygon(name));
	std::string result(name);
	delete [] name;
	return result;
}

void PolygonPath::select(int poly, int outline, int point)
{
	sel_poly = poly;
	sel_outline = outline;
	sel_vertice = point;
}

void PolygonPath::addPolygon(const std::string &name)
{
	Polygon P;
	P.name = name;
	P.outlines.push_back(Polygon::Outline());
	P.outlines[0].is_hole = false;
	P.valid_pathgraph = false;

	polys.push_back(P);

	sel_poly = polys.size() - 1; // select created polygon
	sel_outline = 0;
	sel_vertice = -1;
}

void PolygonPath::addHole()
{
	Polygon &P = polys[sel_poly];

	Polygon::Outline O;
	O.is_hole = true;

	P.outlines.push_back(O);

	sel_outline = P.outlines.size() - 1; // select created outline
	sel_vertice = -1;

	P.valid_pathgraph = false;
}

void PolygonPath::addVertex(int X, int Y)
{
	Polygon::Outline &O = polys[sel_poly].outlines[sel_outline];

	if (sel_vertice == -1)	// first vertice ??
	{
		O.points.push_back(Vertex(X, Y));
		sel_vertice = 0;
	} else {
		O.points.insert(O.points.begin() + sel_vertice + 1, Vertex(X, Y));
		++sel_vertice;
	}

	polys[sel_poly].valid_pathgraph = false;
}

void PolygonPath::addVertex(const Vertex &v)
{
	addVertex(v.x, v.y);
}

void PolygonPath::movePolygonRel(int dx, int dy)
{
	int save_sel_outline = sel_outline;

	for (sel_outline=0; sel_outline<polys[sel_poly].outlines.size(); ++sel_outline)
	{
		moveOutlineRel(dx, dy);
	}

	sel_outline = save_sel_outline;

	polys[sel_poly].valid_pathgraph = false;
}

void PolygonPath::moveOutlineRel(int dx, int dy)
{
	Polygon::Outline &O = polys[sel_poly].outlines[sel_outline];
	int i;
	for (i=0; i<O.points.size(); ++i)
	{
		O.points[i].x += dx;
		O.points[i].y += dy;
	}

	polys[sel_poly].valid_pathgraph = false;
}

void PolygonPath::moveVertexRel(int dx, int dy)
{
	Vertex &V = polys[sel_poly].outlines[sel_outline].points[sel_vertice];
	V.x += dx;
	V.y += dy;
	polys[sel_poly].valid_pathgraph = false;
}

void PolygonPath::moveVertex(int X, int Y)
{
	Vertex &V = polys[sel_poly].outlines[sel_outline].points[sel_vertice];
	V.x = X;
	V.y = Y;
	polys[sel_poly].valid_pathgraph = false;
}

void PolygonPath::adjustOrientation()
{
        int poly, outl;
        for (poly=0; poly<polys.size(); ++poly)
        {
                Polygon &P = polys[poly];
                for (outl=0; outl<P.outlines.size(); ++outl)
                {
                        Polygon::Outline &O = P.outlines[outl];
                        if (is_poly_cw(O.points))
                        {
                                std::reverse(O.points.begin(), O.points.end());
                                P.valid_pathgraph = false;
                        }
                }
        }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build visibility graph                                                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int PolygonPath::nodeDistance(int polygon, int outl1, int node1, int outl2, int node2) const
{
	const Polygon &P = polys[polygon];
	return point_distance(P.outlines[outl1].points[node1].x, P.outlines[outl1].points[node1].y,
	                      P.outlines[outl2].points[node2].x, P.outlines[outl2].points[node2].y);
}

bool PolygonPath::is_relevant(int polygon, int outline, int node) const
{
	// Hole -> A concave node is relevant
	// Main -> A convex node is relevant

	const Polygon::Outline &O = polys[polygon].outlines[outline];
	const int N = O.points.size();

	const Vertex &Node = O.points[node];
	const Vertex &Prev = O.points[(node - 1 + N) % N];
	const Vertex &Next = O.points[(node + 1) % N];

	return is_concave(Prev.x, Prev.y, Node.x, Node.y, Next.x, Next.y) ^ !O.is_hole;
}

bool PolygonPath::lineOfSight(int polygon, int x1, int y1, int x2, int y2, int ign_outl1, int ign_vert1, int ign_outl2, int ign_vert2) const
{
	const Polygon &P = polys[polygon];
	bool intersection = false;
	int outl, segment;
	for (outl=0; outl<P.outlines.size(); ++outl)
	{
		const int outline_size = P.outlines[outl].points.size();
		for (segment=0; segment<outline_size; ++segment)
		{
			const int v1 = segment; // start- and endpoints of segment
			const int v2 = (segment+1) % outline_size;

			if (outl == ign_outl1 && (v1 == ign_vert1 || v2 == ign_vert1)) continue; // ignore these as requested..
			if (outl == ign_outl2 && (v1 == ign_vert2 || v2 == ign_vert2)) continue;

			const int v1_x = P.outlines[outl].points[v1].x; // coordinates of the line v1-->v2
			const int v1_y = P.outlines[outl].points[v1].y;
			const int v2_x = P.outlines[outl].points[v2].x;
			const int v2_y = P.outlines[outl].points[v2].y;

			if (lineCollision(v1_x, v1_y, v2_x, v2_y, x1, y1, x2, y2))
			{
				intersection = true; break;
			}
		}
		if (intersection) break;
	}
	return !intersection;
}

void PolygonPath::addPathGraphEdge(int polygon, int pg_node1, int pg_node2) const
{
	const Polygon &P = polys[polygon];

	Polygon::PathGraphEdge edge;
	edge.other_node = pg_node2;
	edge.distance = nodeDistance(polygon,
		P.path_graph[pg_node1].outline, P.path_graph[pg_node1].node, 
		P.path_graph[pg_node2].outline, P.path_graph[pg_node2].node);
	P.path_graph[pg_node1].edges.push_back(edge);

	//cout <<	P.path_graph[pg_node1].outline << ":" << P.path_graph[pg_node1].node << " ----> " << 
	//	P.path_graph[pg_node2].outline << ":" << P.path_graph[pg_node2].node << endl;
}

void PolygonPath::calculateVisibilityGraph(int polygon) const
{
	const Polygon &P = polys[polygon];
	const int num_outlines = P.outlines.size();

	int outl1, outl2, node1, node2;

        P.path_graph.clear();

	// A. Decide which nodes are relevant for the pathfinder-visibility graph
	for (outl1=0; outl1<num_outlines; ++outl1)
	{
		const int N1 = P.outlines[outl1].points.size();
		for (node1=0; node1<N1; ++node1) 
		{
			if (is_relevant(polygon, outl1, node1))
			{
				// add to pathgraph vertice list:
				Polygon::PathGraphNode pgn;
				pgn.outline = outl1;
				pgn.node = node1;
				P.path_graph.push_back(pgn);
			}
		}
	}

	// build index-matrix for faster reference to node list from P.path_graph:
	std::vector<std::vector<int> > nodes; // nodes[outline][node], -1 = not included in graph
	nodes.resize(num_outlines);
	for (outl1=0; outl1<num_outlines; ++outl1) nodes[outl1].resize(P.outlines[outl1].points.size(), -1);

	int i;
	for (i=0; i<P.path_graph.size(); ++i)
	{
		nodes[P.path_graph[i].outline][P.path_graph[i].node] = i;
	}

	// C. compare each relevant node with each other relevant node and determine visibility
	for (outl1=0; outl1<num_outlines; ++outl1)
	{
		const int N1 = P.outlines[outl1].points.size();

		for (outl2=0; outl2<outl1+1; ++outl2)
		{
			const int N2 = P.outlines[outl2].points.size();
			const bool same_outl = (outl1 == outl2);

			for (node1=0; node1<N1; ++node1)
			{
				if (nodes[outl1][node1] == -1) continue;

				const int prev1 = (node1 - 1 + N1) % N1;
				const int next1 = (node1 + 1) % N1;

				const int node1_x = P.outlines[outl1].points[node1].x;
				const int node1_y = P.outlines[outl1].points[node1].y;
				const int prev1_x = P.outlines[outl1].points[prev1].x;
				const int prev1_y = P.outlines[outl1].points[prev1].y;

				// The following is needed for check II.:
				float angle = leftAngle( // calculate inner angle at node1
					P.outlines[outl1].points[next1].x, P.outlines[outl1].points[next1].y,
					node1_x, node1_y,	
					prev1_x, prev1_y
				);
				if (P.outlines[outl1].is_hole) angle = 2 * PI - angle; // if outl1 is a hole, convert to outer angle instead.

				for (node2=0; node2<N2; ++node2)
				{
					if (nodes[outl2][node2] == -1) continue;

					/////////////////////////////////////////////////////////////////////////////////////
					// I. Handle some trivial cases, when outl1 == outl2
					if (same_outl) 
					{
						// here we only need to check one 'harf'
						if (node2 > node1) break;

						// 'node1' and 'node2' are identical ? -> Line of sight, but don't notate
						if (node1 == node2) continue;
						
						// 'node1' and 'node2' lie adjacent to each other on a polygon segment ??? -> Line of sight.
						if ((node1 == (node2-1+N1) % N1) || (node1 == (node2+1) % N1))
						{	
							addPathGraphEdge(polygon, nodes[outl1][node1], nodes[outl2][node2]);
							addPathGraphEdge(polygon, nodes[outl2][node2], nodes[outl1][node1]);
							continue;
						}
					}


					//////////////////////////////////////////////////////////////////////////////////////
					// II. Check if the line node1-->node2 would leave the polygon immidiately 
                                        //     by comparing 2 angles.
						
					const int node2_x = P.outlines[outl2].points[node2].x;
					const int node2_y = P.outlines[outl2].points[node2].y;

					float angle2 = leftAngle(
						node2_x, node2_y,
						node1_x, node1_y,
						prev1_x, prev1_y
					);
					if (P.outlines[outl1].is_hole) angle2 = 2 * PI - angle2; // if outl1 is a hole, convert to outer angle instead.

					if (angle2 > angle) continue;

					//////////////////////////////////////////////////////////////////////////////////////
					// III. Check if the line from node1 to node2 intersects any segments of the polygon 
					//      (but ignore the segments which node1 or node2 are part of!!)
					if (!lineOfSight(polygon, node1_x, node1_y, node2_x, node2_y, outl1, node1, outl2, node2)) continue;

					//////////////////////////////////////////////////////////////////////////////////////
					// IV. Still here? => Line of sight between outl1:node1 & outl2:node2
					addPathGraphEdge(polygon, nodes[outl1][node1], nodes[outl2][node2]);
					addPathGraphEdge(polygon, nodes[outl2][node2], nodes[outl1][node1]);
 
				}
			}
		}
	}

	P.valid_pathgraph = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Find vertices/outlines/polygons in path based on coordinates                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool PolygonPath::findVertexAt(int X, int Y, int &polygon, int &outline, int &vertice)
{
	const int TOLERANCE = 3;
	int poly, outl, vert;
	
	for (poly=0; poly<polys.size(); ++poly)
	{
		Polygon &P = polys[poly];
		for (outl=0; outl<P.outlines.size(); ++outl)
		{
			Polygon::Outline &O = P.outlines[outl];
			for (vert=0; vert<O.points.size(); ++vert)
			{
				int dx = X - O.points[vert].x;
				int dy = Y - O.points[vert].y;
				if (dx*dx+dy*dy < TOLERANCE*TOLERANCE)
				{
					polygon = poly;
					outline = outl;
					vertice = vert;
					return true;
				} 
			}
		}
	}
	
	return false;
}

bool PolygonPath::findSegmentAt(int X, int Y, int &polygon, int &outline, int &vertice)
{
	const int TOLERANCE = 3;
	int poly, outl, vert;

	for (poly=0; poly<polys.size(); ++poly)
	{
		Polygon &P = polys[poly];
		for (outl=0; outl<P.outlines.size(); ++outl)
		{
			Polygon::Outline &O = P.outlines[outl];
			for (vert=0; vert<O.points.size(); ++vert)
			{
				int x1 = O.points[vert].x;
				int y1 = O.points[vert].y;
				int x2 = O.points[(vert+1)%O.points.size()].x;
				int y2 = O.points[(vert+1)%O.points.size()].y;
				if (line_distance_to_point(x1, y1, x2, y2, X, Y) <= TOLERANCE)
				{
					polygon = poly;
					outline = outl;
					vertice = vert;
					return true;
				} 
			}
		}
	}

	return false;
}

bool PolygonPath::findOutlineAt(int X, int Y, int &polygon, int &outline)
{
	int i;
	for (i=0; i<polys.size(); ++i)
	{
		if (point_in_poly(polys[i].outlines[0].points, Vertex(X, Y)))
		{
			polygon = i;
			outline = 0;

			int j;
			for (j=1; j<polys[i].outlines.size(); ++j)
			{
				if (point_in_poly(polys[i].outlines[j].points, Vertex(X, Y)))
				{
					outline = j;
					break;
				}
			}
			return true;
		}

	}
	return false;
}

bool PolygonPath::findPolygonAt(int &polygon, int X, int Y) const
{
	int i;
	for (i=0; i<polys.size(); ++i)
	{
		bool hit = false;
		// this assumes the first outline is the main outline, not a hole..
		if (point_in_poly(polys[i].outlines[0].points, Vertex(X, Y)))
		{
			hit = true;
			int j;
			for (j=1; j<polys[i].outlines.size(); ++j)
			{
				if (point_in_poly(polys[i].outlines[j].points, Vertex(X, Y)))
				{
					hit = false; break;
				}
			}
		}

		if (hit)
		{
			polygon = i;
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Find shortest path (Dijkstra on path graph)                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<PolygonPath::Polygon::PathGraphEdge> PolygonPath::getPointVisibility(int polygon, int X, int Y) const
{
	const Polygon &P = polys[polygon];
	std::vector<Polygon::PathGraphEdge> result;

	int pgnode;
	for (pgnode=0; pgnode<P.path_graph.size(); ++pgnode)
	{
		Polygon::PathGraphNode &current = P.path_graph[pgnode];
	
		const int X2 = P.outlines[current.outline].points[current.node].x;
		const int Y2 = P.outlines[current.outline].points[current.node].y;

		if (lineOfSight(polygon, X, Y, X2, Y2, current.outline, current.node))
		{
			Polygon::PathGraphEdge edge;
			edge.distance = point_distance(X, Y, X2, Y2);
			edge.other_node = pgnode;
			result.push_back(edge);
		}
	}

	return result;
}

std::list<Vertex> PolygonPath::findPath(int startX, int startY, int goalX, int goalY) const
{
	int polygon, tmp;
	int N;				// number of nodes in path graph (without goal node)

	const int INFTY = std::numeric_limits<int>().max();
	const int START_NODE = -1;
	int GOAL_NODE;

	std::vector<int> cost;          // cost[n]: Best cost from startX|startY to P.path_graph[n] (or to goalX|goalY if n = GOAL_NODE)
	std::vector<int> prev;		// pred[n]: Predessor to P.path_graph[n] (or to goalX|goalY if n = GOAL_NODE) of best path from startX|startY 
	std::vector<bool> open;		// open[n] = true: node is open
	std::vector<int> dist2goal;	// dist2goal[n]: distance from P.path_graph[n] to GOAL_NODE

	if (!findPolygonAt(polygon, startX, startY)) { goto no_path; }
	if (!findPolygonAt(tmp, goalX, goalY)) { goto no_path; }
	if (polygon != tmp) { goto no_path; }

	if (lineOfSight(polygon, startX, startY, goalX, goalY)) // Direct line from start to goal?
	{
		std::list<Vertex> result;
		result.push_back(Vertex(startX, startY));
		result.push_back(Vertex(goalX, goalY));
		return result;
	}

	if (!polys[polygon].valid_pathgraph) calculateVisibilityGraph(polygon);

	GOAL_NODE = N = polys[polygon].path_graph.size();

	cost.resize(N+1, INFTY);
	prev.resize(N+1);
	open.resize(N+1, true);

	{
		// edit cost of nodes which are visible from start point
		std::vector<Polygon::PathGraphEdge> start_vis = getPointVisibility(polygon, startX, startY);
		for (int i=0; i<start_vis.size(); ++i)
		{
			cost[start_vis[i].other_node] = start_vis[i].distance;
			prev[start_vis[i].other_node] = START_NODE;
		}

		// build dist2goal:
		std::vector<Polygon::PathGraphEdge> goal_vis = getPointVisibility(polygon, goalX, goalY);
		dist2goal.resize(N, INFTY);
		for (int i=0; i<goal_vis.size(); ++i)
		{
			dist2goal[goal_vis[i].other_node] = goal_vis[i].distance;
		}
	}

	while (true)
	{
		// find node in open set with cost = minimal
		int best_node, best_cost = INFTY;
		for (int i=0; i<N+1; ++i)
		{
			if (open[i] && cost[i] < best_cost)
			{
				best_node = i;
				best_cost = cost[i];
			}
		}

		if (best_cost == INFTY) goto no_path;

		open[best_node] = false;

		if (best_node == GOAL_NODE) {
			break;	// path found..
		} else {
			const std::vector<Polygon::PathGraphEdge> &edges = polys[polygon].path_graph[best_node].edges;
			for (int j=0; j<edges.size(); ++j)
			{
				int neighbour_node = edges[j].other_node;
				if (cost[neighbour_node] > cost[best_node] + edges[j].distance)
				{
					cost[neighbour_node] = cost[best_node] + edges[j].distance;
					prev[neighbour_node] = best_node;
				}
			}

			if (dist2goal[best_node] != INFTY)
			{
				if (cost[GOAL_NODE] > cost[best_node] + dist2goal[best_node])
				{
					cost[GOAL_NODE] = cost[best_node] + dist2goal[best_node];
					prev[GOAL_NODE] = best_node;
				}
			}
		}

	}

	if (cost[GOAL_NODE] == INFTY) goto no_path; // can not happen

	{	// construct path and return
		std::list<Vertex> result;
		int node = GOAL_NODE;
		result.push_front(Vertex(goalX, goalY));
		while (prev[node] != START_NODE)
		{
			node = prev[node];
			int outl = polys[polygon].path_graph[node].outline;
			int vert = polys[polygon].path_graph[node].node;
			result.push_front(polys[polygon].outlines[outl].points[vert]);
		}
		result.push_front(Vertex(startX, startY));
		return result;
	}

no_path:
	return std::list<Vertex>();


}



