#include "kos_drv.h"

#include "kos.h"
#include <plx/matrix.h>

namespace DCDraw_Kos
{
	static pvr_init_params_t params =
	{
		/* Enable opaque and translucent polygons with size 16 */
		{ PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_0 },
		
		/* Vertex buffer size 512K */
		512*1024,

		/* enable dma */
		1,

		/* disable fsaa */
		0
	};

	// DMA buffers. This should ideally be in separate memory banks to take
	// advantage of greater speed, but this will work for now.
	uint8 dmabuffers[2][1*1024*1024] __attribute__((aligned(32)));

	void InitGraphics(int screen_width, int screen_height)
	{
		struct Res
		{
			int width, height;
			bool pal;
			int vid_mode;
		} res[] = {
			{320, 240, true, DM_320x240_PAL},
		};

		const int num_res = sizeof(res) / sizeof(res[0]);

		Res *find = 0;
		for (int i=0; i<num_res; ++i)
		{
			if (res[i].width == screen_width && res[i].height == screen_height)
			{
				find = &res[i];
				break;
			}
		}
		
		if (find)
		{
			vid_set_mode(find->vid_mode, PM_RGB565);

			if (pvr_init(&params) < 0) dbgio_printf("pvr_init returned error\n");
			pvr_set_bg_color(0, 0, 0);
			vid_border_color(0, 0, 0);

			pvr_set_vertbuf(PVR_LIST_OP_POLY, dmabuffers[0], 1*1024*1024);
			pvr_set_vertbuf(PVR_LIST_TR_POLY, dmabuffers[1], 1*1024*1024);
		}
	}
	
	void DoneGraphics()
	{
	}

	void SetClearColor(DCDraw::Color col)
	{
		pvr_set_bg_color(col.r, col.g, col.b);
	}
	
	void ClearScreen()
	{
	}



	static int current_list = -1;

	void PvrBeginList(int list)
	{
		if (current_list != -1) PvrEndList();
		pvr_list_begin(list);
		current_list = list;
	}

	void PvrEndList()
	{
		if (current_list != -1)
		{
			pvr_list_finish();
			current_list = -1;
		}
	}
	
	void BeginFrame()
	{
		pvr_wait_ready();
		pvr_scene_begin();

		//PvrBeginOpaque();
	}
	
	void EndFrame()
	{
		//PvrEndList();
		pvr_scene_finish();
	}

	void PvrBeginOpaque()
	{
		if (current_list == PVR_LIST_OP_POLY) return;
		PvrEndList();
		PvrBeginList(PVR_LIST_OP_POLY);
	}

	void PvrBeginTrans()
	{
		if (current_list == PVR_LIST_TR_POLY) return;
		PvrEndList();
		PvrBeginList(PVR_LIST_TR_POLY);
	}

	int PvrGetList()
	{
		return current_list;
	}

};

