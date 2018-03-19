#ifndef KOS_DRV_H
#define KOS_DRV_H

#include "../color.h"

namespace DCDraw_Kos
{
	void InitGraphics(int screen_width, int screen_height);
	void DoneGraphics();

	void SetClearColor(DCDraw::Color col);
	void ClearScreen();

	void BeginFrame();
	void EndFrame();

	// PLATFORM DEPENDENT FROM HERE
	
	void PvrBeginList(int list);
	int  PvrGetList();
	void PvrEndList();
	void PvrBeginOpaque();
	void PvrBeginTrans();
};

#endif
