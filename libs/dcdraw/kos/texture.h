#ifndef DCDRAW_KOS_TEXTURE_H
#define DCDRAW_KOS_TEXTURE_H

#include <string>

#include "../canvas.h"
#include "../transform.h"
#include "../color.h"
#include "../clip.h"

#include <kos.h>
#include <dc/pvr.h>

namespace DCDraw_Kos
{
	class Texture
	{
		private:
			Texture(Texture &other); // unimplemented
			Texture &operator=(Texture &other); // unimplemented

		public:
			Texture(); // Create empty texture
			Texture(DCDraw::Canvas &canvas); // Create texture from canvas
			~Texture();

			void Load(DCDraw::Canvas &canvas); // (Re)load texture from canvas
			void Reset(); // Clear texture

			int GetWidth() { return con_width; }
			int GetHeight() { return con_height; }

			void Draw(DCDraw::Transform &t, 
			          DCDraw::Color &col,
			          DCDraw::Clip &clip);

			inline void Draw(DCDraw::Transform &t,
			                 DCDraw::Color &col)
			{
				static DCDraw::Clip clip; // no clipping
				Draw(t, col, clip);
			}
			
			inline void Draw(DCDraw::Transform &t)
			{
				static DCDraw::Clip clip; // no clipping
				static DCDraw::Color col; // no tinting
				Draw(t, col, clip);
			}

			////////////////////////////////////
			// Platform dependant from here.. //
			////////////////////////////////////

			int GetTextureWidth() { return tex_width; }
			int GetTextureHeight() { return tex_height; }

		private:
			void Bind(DCDraw::Canvas &canvas);
			void Unbind();

			bool bound;
			int tex_width, tex_height; // n^2, texture size
			int con_width, con_height; // content size
			
			int fmt;		// PVR texture format (e.g., PVR_TXRFMT_ARGB4444)
			pvr_ptr_t ptr;		// Pointer to the PVR memory

			pvr_poly_cxt_t cxt_opaque; // PVR polygon contexts for each list for this texture
			pvr_poly_cxt_t cxt_trans;
			pvr_poly_hdr_t hdr_opaque; // PVR polygon headers for each list for this texture
			pvr_poly_hdr_t hdr_trans;
	};
}

#endif

