#ifndef DCDRAW_COLOR_H
#define DCDRAW_COLOR_H

namespace DCDraw
{
	struct Color
	{
		unsigned char a, r, g, b;

		Color() : a(255), r(255), g(255), b(255)
		{
		}

		Color(unsigned char r, unsigned char g, unsigned char b) : a(255), r(r), g(g), b(b)
		{
		}

		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : a(a), r(r), g(g), b(b)
		{
		}

		Color operator*(const Color & other) const 
		{
			return Color(a*other.a, r*other.r, g*other.g, b*other.b);
		}

		Color operator+(const Color & other) const 
		{
			return Color(a+other.a, r+other.r, g+other.g, b+other.b);
		}

		Color & operator*=(const Color & other) 
		{
			return (*this = *this * other);
		}

		Color & operator+=(const Color & other) 
		{
			return (*this = *this + other);
		}
	};
}

#endif

