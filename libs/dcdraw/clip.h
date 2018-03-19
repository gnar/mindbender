#ifndef DCDRAW_CLIP_H
#define DCDRAW_CLIP_H

namespace DCDraw
{
	class Clip
	{
	public:
		Clip() : do_clip(false), clip_x(0), clip_y(0), clip_width(0), clip_height(0) 
		{
		}

		Clip(int x, int y, int w, int h) 
			: do_clip(true), clip_x(x), clip_y(y), clip_width(w), clip_height(h) 
		{
		}

		// Set clipping to draw
		void SetClipping(int x, int y, int width, int height) 
		{ 
			do_clip = true; 
			clip_x = x; clip_y = y; clip_width = width; clip_height = height; 
		}
		
		void SetNoClipping() 
		{ 
			do_clip = false; 
		}
		
		bool GetClipping(int &x, int &y, int &width, int &height)
		{
			x = clip_x; y = clip_y; width = clip_width; height = clip_height;
			return do_clip;
		}

	private:
		// Clipping
		bool do_clip;
		int clip_x, clip_y;
		int clip_width, clip_height;
	};
}

#endif

