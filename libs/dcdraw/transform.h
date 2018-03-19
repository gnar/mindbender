#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vector.h"

namespace DCDraw
{
	class Transform
	{
	public:
		Transform() : scale(1.0f, 1.0f), rotate(0) {}
		
		// Get/Set translation
		void            SetTrans(const Vector &v) { trans = v; }
		void            SetTrans(int x, int y) { trans = Vector(x, y); }
		void            SetTrans(float x, float y) { trans = Vector(x, y); }

		const Vector&   GetTrans() const { return trans; }

		void            Trans(const Vector &v) { trans += v; } // relative
		void		Trans(int x, int y) { trans += Vector(x, y); }
		void		Trans(float x, float y) { trans += Vector(x, y); }

		// Get/Set rotation
		void            SetRotation(float r) { rotate = r; }
		float           GetRotation() const { return rotate; }
		
		void            Rotate(float add) { rotate += add; }

		// Get/Set scaling
		void            SetScale(const Vector & s) { scale = s; }
		void            SetScale(int x, int y) { scale = Vector(x, y); }
		void		SetScale(float x, float y) { scale = Vector(x, y); }
		
		const Vector&   GetScale() const { return scale; }
		 
		// Set origin 
		void            SetOrigin(const Vector &v) { origin = v; }
		void		SetOrigin(int x, int y) { origin = Vector(x, y); }
		void            SetOrigin(float x, float y) { origin = Vector(x, y); }
		
		const Vector&   GetOrigin() const { return origin; }
		
	private:
		// Transformation
		Vector		trans;		// Translation
		Vector		scale;		// Scaling
		Vector		origin;		// Origin
		float           rotate;         // Rotation
	};
}

#endif

