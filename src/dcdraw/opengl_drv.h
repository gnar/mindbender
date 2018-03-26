#ifndef OPENGL_DRV_H
#define OPENGL_DRV_H

#include "color.h"

namespace DCDraw
{
	void InitGraphics(int screen_width, int screen_height);
	void DoneGraphics();

	void SetClearColor(DCDraw::Color col);
	void ClearScreen();

	void BeginFrame();
	void EndFrame();
	
	void DrawPoint(int x, int y, DCDraw::Color &col);
	void DrawLine(int x1, int y1, int x2, int y2, DCDraw::Color &col);
	
	// PLATFORM DEPENDENT FROM HERE
	void OpenGLClipScreen(bool enable);
	void OpenGLClipScreen(bool enable, int x, int y, int w, int h);
}

#endif

