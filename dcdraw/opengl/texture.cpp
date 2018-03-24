#include "texture.h"

#include <cstring>

namespace DCDraw_OpenGL
{
	using namespace DCDraw;

	// defined in opengl_drv.cpp
 	extern bool support_rectangular_textures;
	extern int max_texture_size;
	extern int max_rect_texture_size;
	
	Texture::Texture() : bound(false)
	{
		Reset();
	}
	
	Texture::Texture(Canvas &canvas) : bound(false)
	{
		Load(canvas);
	}
	
	Texture::~Texture()
	{
		Reset();
	}

	void Texture::Load(Canvas &canvas)
	{
		// Unload previous image, if any
		Reset();

		// Get content size from canvas
		width  = canvas.GetWidth();
		height = canvas.GetHeight();

		// Load canvas 
		Bind(canvas);
	}

	void Texture::Reset()
	{
		Unbind();
		width = height = 0;
	}

	void Texture::Bind(Canvas &canvas)
	{
		Unbind();

		GenerateSubTexMatrix();

		// If necessary, convert canvas to RGB or RGBA
		Canvas src_canvas = canvas;
		switch (src_canvas.GetFormat())
		{
			case Canvas::RGB565:   src_canvas = src_canvas.Convert(Canvas::RGB); break;
			case Canvas::ARGB4444:
			case Canvas::RGBA4444:
			case Canvas::ARGB:     src_canvas = src_canvas.Convert(Canvas::RGBA); break;
			case Canvas::RGB:
			case Canvas::RGBA:     break;
		}
		
		int pixelSize = src_canvas.GetFormat() == Canvas::RGB ? 3 : 4;

		// Allocate temporary texture buffer that is big enough for all sub-textures
		size_t rgba_size = 0;
		for (size_t s=0; s<subs.size(); ++s)
		{
			size_t a = subs[s].tex_w * subs[s].tex_h;
			if (rgba_size < a) rgba_size = a;
		}
		unsigned char *rgba = new unsigned char[rgba_size * pixelSize];
		
		// Generate all sub-textures
		for (size_t s=0; s<subs.size(); ++s)
		{
			SubTex &sub = subs[s];
				
			// Copy row by row into rgba
			for (int y=0; y<sub.h; ++y)
			{
				unsigned char *src = src_canvas.GetData() 
						   + (sub.y + y) * src_canvas.GetPitch()
						   + sub.x * pixelSize;
				unsigned char *dst = rgba + y * (sub.tex_w * pixelSize);
				memcpy(dst, src, pixelSize * sub.w);
			}

			// Create texture
			GLuint tex_t = sub.rect ? /*GL_TEXTURE_RECTANGLE_NV*/0 : GL_TEXTURE_2D;
			glGenTextures(1, &sub.tex_id);
			glBindTexture  (tex_t, sub.tex_id);
			glTexImage2D   (tex_t, 0, pixelSize, sub.tex_w, sub.tex_h, 0, (pixelSize == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, rgba);
			glTexParameteri(tex_t, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(tex_t, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		
		// Clean up
		delete [] rgba;

		// Done
		bound = true;
	}
	
	void Texture::Unbind()
	{
		if (!bound) return;

		for (size_t s=0; s<subs.size(); ++s)
		{
			SubTex &sub = subs[s];
			glDeleteTextures(1, &sub.tex_id);
		}
		bound = false;
	}

	void Texture::Draw(Transform &t, Color &col, Clip &clip)
	{
		if (!bound) return;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(t.GetTrans().x, t.GetTrans().y, 0);
		glRotatef(t.GetRotation(), 0, 0, 1.0f);
		glScalef(t.GetScale().x, t.GetScale().y, 0);
		glTranslatef(-t.GetOrigin().x, -t.GetOrigin().y, 0);
		glColor4ub(col.r, col.g, col.b, col.a);

		int clip_x, clip_y, clip_w, clip_h;
		if (!clip.GetClipping(clip_x, clip_y, clip_w, clip_h)) {
			clip_x = clip_y = 0;
			clip_w = width;
			clip_h = height;
		}
		int clip_x2 = clip_x + clip_w;
		int clip_y2 = clip_y + clip_h;

		const size_t num_subs = subs.size();
		for (size_t s=0; s<num_subs; ++s)
		{
			SubTex &sub = subs[s];
			int x1 = sub.x, y1 = sub.y;
			int x2 = sub.x + sub.w;
			int y2 = sub.y + sub.h;

			if (x1 >= clip_x2 || y1 >= clip_y2) continue;
			if (x2 <= clip_x  || y2 <= clip_y ) continue;

			float u1=0, v1=0, u2=sub.w, v2=sub.h;

			if (x2 > clip_x2) {
				u2 = clip_x2 - x1;
				x2 = clip_x2;
			}

			if (y2 > clip_y2) {
				v2 = clip_y2 - y1;
				y2 = clip_y2;
			}

			if (x1 < clip_x) {
				u1 = clip_x - x1;
				x1 = clip_x;
			}

			if (y1 < clip_y) {
				v1 = clip_y - y1;
				y1 = clip_y;
			}
			
			// Draw texture
			GLuint tex_t = sub.rect ? /*GL_TEXTURE_RECTANGLE_NV*/0 : GL_TEXTURE_2D;
			if (sub.rect) {
				//tex_t = GL_TEXTURE_RECTANGLE_NV;
			} else {
				u1 /= sub.tex_w; v1 /= sub.tex_h;
				u2 /= sub.tex_w; v2 /= sub.tex_h;
				tex_t = GL_TEXTURE_2D;
			}
			
			glEnable(tex_t);
			glBindTexture(tex_t, sub.tex_id);
			
			glBegin(GL_QUADS);
			glTexCoord2f(u1, v1); glVertex2f(x1-clip_x, y1-clip_y);
			glTexCoord2f(u2, v1); glVertex2f(x2-clip_x, y1-clip_y);
			glTexCoord2f(u2, v2); glVertex2f(x2-clip_x, y2-clip_y);
			glTexCoord2f(u1, v2); glVertex2f(x1-clip_x, y2-clip_y);
			glEnd();

			glDisable(tex_t);
			
		}

		glPopMatrix();
	}


	static int ToPOT(int size)
	{
		int pot_size = 2;
		while (pot_size < size) pot_size *= 2;
		return pot_size;
	}

	static std::vector<int> GetTextureSizes(int size)
	{
		std::vector<int> result;
		int pos = 0;
		while (pos < size)
		{
			int rest     = size - pos;
			int pot_rest = ToPOT(rest); if (pot_rest > max_texture_size) pot_rest = max_texture_size;
			if (rest < 3*pot_rest/4 && pot_rest > 64) {
				result.push_back(pot_rest/2);
				pos += pot_rest/2;
			} else {
				result.push_back(pot_rest);
				pos += pot_rest;
			}
		}
		return result;
	}

	void Texture::GenerateSubTexMatrix()
	{
		if (!support_rectangular_textures) // Don't use rectangular textures extension?
		{
			std::vector<int> x_sizes = GetTextureSizes(width);
			std::vector<int> y_sizes = GetTextureSizes(height);
				
			// Generate sub texture list
			int pos_y = 0;
			for (size_t y=0; y<y_sizes.size(); ++y)
			{
				int pos_x = 0;
				for (size_t x=0; x<x_sizes.size(); ++x)
				{
					SubTex sub;
					sub.rect = false; // POT-Texture
					sub.x = pos_x; sub.y = pos_y;
					sub.w = sub.tex_w = x_sizes[x];
					sub.h = sub.tex_h = y_sizes[y];
					if (sub.x + sub.w > width ) sub.w = width  - sub.x;
					if (sub.y + sub.h > height) sub.h = height - sub.y;
					subs.push_back(sub);

					pos_x += x_sizes[x];
				}
				pos_y += y_sizes[y];
			}
		} else { // Use rectangular textures extension
			SubTex sub;
			sub.rect = true;
			sub.x = 0;
			sub.y = 0;
			sub.w = sub.tex_w = width;
			sub.h = sub.tex_h = height;
			if (sub.tex_w & 3)
			{
				sub.tex_w &= ~3;
				sub.tex_w += 4;
			}
			subs.push_back(sub);
		}
	}
}

