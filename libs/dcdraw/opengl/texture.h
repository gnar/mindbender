#ifndef DCDRAW_GL_TEXTURE_H
#define DCDRAW_GL_TEXTURE_H

#include <string>
#include <vector>

#if defined(__APPLE__) && defined(__DARWIN__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
	
#include "../canvas.h"
#include "../transform.h"
#include "../color.h"
#include "../clip.h"

namespace DCDraw_OpenGL
{
	class Texture
	{
		Texture(Texture &other); // prevent copying
		Texture &operator=(Texture &other);

	public:
		Texture(); // empty texture
		Texture(DCDraw::Canvas &canvas); // from canvas
		~Texture();

		void Load(DCDraw::Canvas &canvas); // (Re)load texture from canvas
		void Reset(); // Clear texture

		int GetWidth () { return width; }
		int GetHeight() { return height; }

		void Draw(DCDraw::Transform &t, DCDraw::Color &col, DCDraw::Clip &clip);

		void Draw(DCDraw::Transform &t, DCDraw::Color &col)
		{
			static DCDraw::Clip clip; // no clipping
			Draw(t, col, clip);
		}
		
		void Draw(DCDraw::Transform &t)
		{
			static DCDraw::Clip clip; // no clipping
			static DCDraw::Color col; // no tinting
			Draw(t, col, clip);
		}

	private:
		void GenerateSubTexMatrix();
		void Bind(DCDraw::Canvas &canvas);
		void Unbind();

		bool bound;
		int width, height; // image size

		struct SubTex
		{
			GLuint tex_id;
			int tex_w, tex_h; // texture size
			int x, y, w, h;   // contained image
			bool rect;        // Is NPOT texture?
		};
		std::vector<SubTex> subs;
	};
}

#endif

