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

#ifndef TOOLS_H
#define TOOLS_H

#include <cmath>

#define PI 3.1416f

#include "polygonpath.h"

namespace GeometryTools
{
	bool lineCollision( float x1, float y1, float x2, float y2, float X1, float Y1, float X2, float Y2)
	{
		float den  = (x2-x1)*(Y2-Y1) - (y2-y1)*(X2-X1);
		float num1 = (y1-Y1)*(X2-X1) - (x1-X1)*(Y2-Y1);
		float num2 = (y1-Y1)*(x2-x1) - (x1-X1)*(y2-y1);

		if (den == 0) // Parallel?
		{
			return false;
			// if num1 == 0 then lines are colinear.  !!!
		}

		float r = num1/den;
		float s = num2/den;

		if ((r >= 0.0f) && (r <= 1.0f) && (s >= 0.0f) && (s <= 1.0f)) return true;

		return false;
	}

	float leftAngle(float x1, float y1, float x2, float y2, float x3, float y3)
	{
		float dx1 = x2 - x1, dy1 = y2 - y1;
		float dx2 = x2 - x3, dy2 = y2 - y3;

		float dot = dx1 * dx2 + dy1 * dy2;
		float len1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
		float len2 = std::sqrt(dx2 * dx2 + dy2 * dy2);

		float angle = std::acos(dot / (len1 * len2));

		if (dx1 * dy2 - dy1 * dx2 < 0) angle = 2 * PI - angle;

		return angle;
	}

	bool is_concave(int x1, int y1, int x2, int y2, int x3, int y3)
	{
		int dx1 = x2 - x1, dy1 = y2 - y1;
		int dx2 = x2 - x3, dy2 = y2 - y3;
		return (dx1 * dy2 - dy1 * dx2 < 0);
	}

	int point_in_poly(const std::vector<Vertex> &pts, const Vertex &test_point)
	{
		int npol = pts.size();
		int x = test_point.x;
		int y = test_point.y;
		int i, j, c = 0;
		for (i = 0, j = npol-1; i < npol; j = i++) 
		{
			if ( (((pts[i].y<=y) && (y<pts[j].y)) || ((pts[j].y<=y) && (y<pts[i].y))) &&
			     (x < (pts[j].x - pts[i].x) * (y - pts[i].y) / (pts[j].y - pts[i].y) + pts[i].x)) c = !c;
		}
		return c;
	}

	// check if a polygon is in clock-wise order
	bool is_poly_cw(const std::vector<Vertex> &poly_verts)
	{
		// find vector with y=max, if more than one found, get rightmost vector.
		int best = 0;
		const int N = poly_verts.size();

		//assert(N >= 3);

		for (int i=0; i<N; ++i)
		{
			if (poly_verts[i].y >= poly_verts[best].y)
			{
				if (poly_verts[i].y > poly_verts[best].y || poly_verts[i].x > poly_verts[best].x)
				{
					best = i;
				}
			}
		}

		// iff this vertex is convex, the polygon is clockwise
		const int prev = (best - 1 + N) % N;
		const int next = (best + 1) % N;
		return !is_concave(
			poly_verts[prev].x, poly_verts[prev].y,
			poly_verts[best].x, poly_verts[best].y,
			poly_verts[next].x, poly_verts[next].y);
	}

	float line_distance_to_point(int x1, int y1, int x2, int y2, int X, int Y)
	{
		// C=(X|Y), A=(x1|y1), B=(x2|y2), P: Nearest point to C on line AB

		const int AC_x = X - x1, AC_y = Y - y1;
		const int AB_x = x2 - x1, AB_y = y2 - y1;

		const float ACdotAB = static_cast<float>(AC_x * AB_x + AC_y * AB_y);
		const float lineLenSqr = static_cast<float>((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
		float r = ACdotAB / lineLenSqr;

		if (r > 1.0f) r = 1.0f;
		if (r < 0.0f) r = 0.0f;

		const float P_x = x1 + r*AB_x;
		const float P_y = y1 + r*AB_y;

		return std::sqrt((P_x-X)*(P_x-X) + (P_y-Y)*(P_y-Y));
	}

	int point_distance(int x1, int y1, int x2, int y2)
	{
		return int(std::sqrt(float((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))));
	}

}; //namespace GeometryTools

#endif


