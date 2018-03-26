#include "opengl_drv.h"

#if defined(__APPLE__) && defined(__DARWIN__)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include <cstring>

//#include <rect_ext.h>

#include <stdio.h>

namespace DCDraw
{
	using namespace DCDraw;
	
 	bool support_rectangular_textures; // Is GL_NV_texture_rectangle supported?
	int max_texture_size;
	int max_rect_texture_size;

	static int width = -1;
	static int height = -1;
	
	void InitGraphics(int screen_width, int screen_height)
	{
		width = screen_width;
		height = screen_height;
		
		glClearColor(0, 0, 0, 0);
		glColor3f(1.0f, 1.0f, 1.0f);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, screen_width, screen_height, 0, -1, 1);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		glPointSize(1.0f);

		SetClearColor(Color(0, 0, 0, 1));

		// Check max texture width
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

		// Check for GL_NV_texture_rectangle extension
		const GLubyte *exts = glGetString(GL_EXTENSIONS);
		support_rectangular_textures =    std::strstr((char*)exts, "GL_NV_texture_rectangle")
		                               || std::strstr((char*)exts, "GL_ARB_texture_rectangle")
		                               || std::strstr((char*)exts, "GL_EXT_texture_rectangle");
		                               
		if (support_rectangular_textures) {
			//glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_NV, &max_rect_texture_size);
			//printf("%i\n", max_rect_texture_size);
		} else {
			max_rect_texture_size = 0;
		}
		
        support_rectangular_textures = false;
	}
	
	void DoneGraphics()
	{
	}

	void SetClearColor(Color col)
	{
		glClearColor(col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f);
	}

	void ClearScreen()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void BeginFrame()
	{
		ClearScreen();
	}
	
	void EndFrame()
	{
	}

	void DrawPoint(int x, int y, DCDraw::Color &col)
	{
		++x; ++y;
		glColor4ub(col.r, col.g, col.b, col.a);
		glBegin(GL_POINTS);
		glVertex3i(x, y, 0);
		glEnd();
	}

	void DrawLine(int x1, int y1, int x2, int y2, DCDraw::Color &col)
	{
		glColor4ub(col.r, col.g, col.b, col.a);
		
		glBegin(GL_LINES);
		glVertex3i(x1, y1, 0);
		glVertex3i(x2, y2, 0);
		glEnd();
		
		glBegin(GL_POINTS);
		glVertex3i(x2, y2, 0);
		glEnd();
	}

	void OpenGLClipScreen(bool enable)
	{
		if (enable) {
			glEnable(GL_SCISSOR_TEST);
		} else {
			glDisable(GL_SCISSOR_TEST);
		}
	}
	
	void OpenGLClipScreen(bool enable, int x, int y, int w, int h)
	{
		OpenGLClipScreen(enable);
		glScissor(x, height - y - h, w, h);
	}
}

