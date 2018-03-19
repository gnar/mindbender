#include "texture.h"

#include "../transform.h"
#include "../color.h"
#include "../canvas.h"

#include "kos_drv.h"

#include <kos.h>
#include <dc/pvr.h>
#include <dc/matrix3d.h>

#include <math.h> // for M_PI

namespace DCDraw_Kos
{
	using namespace DCDraw;
	
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

		// Calculate PVR texture size, which must be 2^n * 2^m 
		tex_width = tex_height = 4;
		while (tex_width  < canvas.GetWidth() ) tex_width  *= 2;
		while (tex_height < canvas.GetHeight()) tex_height *= 2;

		// Get content size from canvas
		con_width  = canvas.GetWidth();
		con_height = canvas.GetHeight();

		// Load canvas 
		Bind(canvas);
	}

	void Texture::Reset()
	{
		Unbind();

		tex_width = tex_height = con_width = con_height = 0;
		ptr = 0;
	}

	void Texture::Bind(Canvas &canvas)
	{
		if (bound) Unbind();

		// If necessary, convert canvas to a new color format
		Canvas src_canvas = canvas;
		switch (src_canvas.GetFormat())
		{
			// with alpha
			case Canvas::ARGB:    
			case Canvas::RGBA:    
			case Canvas::RGBA4444:
				src_canvas = src_canvas.Convert(Canvas::ARGB4444); 
			case Canvas::ARGB4444:
				this->fmt = PVR_TXRFMT_ARGB4444;
				break;

			// without alpha
			case Canvas::RGB:
				src_canvas = src_canvas.Convert(Canvas::RGB565); 
			case Canvas::RGB565:
				this->fmt = PVR_TXRFMT_RGB565;
				break;
		}
	
		// Allocate PVR video memory
		this->ptr = pvr_mem_malloc(tex_width * tex_height * 2);

		// Did we actually get the memory? 
		//if (txr->ptr == NULL) {

		// Setup the poly context structs 
		pvr_poly_cxt_txr(&cxt_opaque, PVR_LIST_OP_POLY, fmt, tex_width, tex_height, ptr, PVR_FILTER_BILINEAR);
		pvr_poly_cxt_txr(&cxt_trans,  PVR_LIST_TR_POLY, fmt, tex_width, tex_height, ptr, PVR_FILTER_BILINEAR);
		
		//
		cxt_opaque.depth.comparison = PVR_DEPTHCMP_ALWAYS;
		cxt_trans.depth.comparison  = PVR_DEPTHCMP_ALWAYS;
		cxt_opaque.depth.write = PVR_DEPTHWRITE_DISABLE;
		cxt_trans.depth.write  = PVR_DEPTHWRITE_DISABLE;

		cxt_opaque.gen.culling = PVR_CULLING_NONE;
		cxt_trans.gen.culling = PVR_CULLING_NONE;

		// Compile the poly headers
		pvr_poly_compile(&hdr_opaque, &cxt_opaque);
		pvr_poly_compile(&hdr_trans, &cxt_trans);
		
		// Realign src image if necessary, the load up image to pvr ram
		if (tex_width == con_width) {
			pvr_txr_load_ex((void*)src_canvas.GetData(), ptr, tex_width, tex_height, PVR_TXRLOAD_16BPP);
		} else { // realign?
			unsigned char *data = new unsigned char [tex_width * tex_height * 2];
			for (int y=0; y<con_height; ++y)
			{
				unsigned char *src = src_canvas.GetData() + y * (con_width * 2);
				unsigned char *dst = data + y * (tex_width * 2);
				memcpy((void*)dst, (void*)src, con_width * 2);
			}
			pvr_txr_load_ex(data, ptr, tex_width, tex_height, PVR_TXRLOAD_16BPP);
			delete [] data;
		}

		bound = true;
	}
	
	void Texture::Unbind()
	{
		if (bound)
		{
			pvr_mem_free(ptr);
			ptr = 0;
			bound = false;
		}
	}

	void Texture::Draw(Transform &t, Color &col, Clip &clip)
	{
		if (bound)
		{
			pvr_vertex_t vert;

			// Send the right poly header
#if 0
			switch (PvrGetList()) 
			{
				case PVR_LIST_OP_POLY: pvr_prim(&hdr_opaque, sizeof(hdr_opaque)); break;
				case PVR_LIST_TR_POLY: pvr_prim(&hdr_trans,  sizeof(hdr_trans)); break;
				default: assert_msg( 0, "Invalid list specification" );
			}
#endif
			// Select list and send poly header
			int list = 0;
			switch (this->fmt)
			{
				case PVR_TXRFMT_RGB565:
					list = PVR_LIST_OP_POLY;
					pvr_list_prim(list, &hdr_opaque, sizeof(hdr_opaque)); break;
					
				case PVR_TXRFMT_ARGB4444:
					list = PVR_LIST_TR_POLY;
					pvr_list_prim(list, &hdr_trans,  sizeof(hdr_trans)); break;
			}
		
			// Set vertex color..
			if (/*PvrGetList()*/ list == PVR_LIST_TR_POLY) 
			{
				vert.argb  = PVR_PACK_COLOR(col.a / 255.f, col.r / 255.f, col.g / 255.f, col.b / 255.f);
			} else {
				vert.argb  = PVR_PACK_COLOR(1.0f, col.r / 255.f, col.g / 255.f, col.b / 255.f);
			}
			vert.oargb = 0;
			
			// Hack. (???)
			float layer;
			if (/*PvrGetList()*/ list == PVR_LIST_TR_POLY)
			{
				layer = 10.f;
			} else {
				layer = 9.f;
			}

			// Create transformation matrix
			const Vector &tv = t.GetTrans();
			const Vector &sv = t.GetScale();
			const Vector &ov = t.GetOrigin();

			mat_identity();
			mat_translate(tv.x, tv.y, 0);
			mat_rotate_z(-(float(M_PI) / 180.f) * t.GetRotation());
			mat_scale(sv.x, sv.y, 1.0f);
			mat_translate(-ov.x, -ov.y, 0);
		
			// Send all 4 vertices
			float dummy = 0;
			int clip_x, clip_y, clip_width, clip_height;
			if (clip.GetClipping(clip_x, clip_y, clip_width, clip_height)) {
				// clipping version
				float u1 = clip_x / float(tex_width);
				float v1 = clip_y / float(tex_height);
				float u2 = (clip_x + clip_width) / float(tex_width);
				float v2 = (clip_y + clip_height) / float(tex_height);
				
				vert.z = layer;
				vert.flags = PVR_CMD_VERTEX;

				vert.x = 0; vert.y = clip_height;
				vert.u = u1; vert.v = v2;
				mat_trans_single(vert.x, vert.y, dummy);
				pvr_list_prim(list, &vert, sizeof(vert));
				
				vert.x = 0; vert.y = 0; 
				vert.u = u1; vert.v = v1;
				mat_trans_single(vert.x, vert.y, dummy);
				pvr_list_prim(list, &vert, sizeof(vert));

				vert.x = clip_width; vert.y = clip_height;
				vert.u = u2; vert.v = v2;
				mat_trans_single(vert.x, vert.y, dummy);
				pvr_list_prim(list, &vert, sizeof(vert));

				vert.flags = PVR_CMD_VERTEX_EOL;

				vert.x = clip_width; vert.y = 0;
				mat_trans_single(vert.x, vert.y, dummy);
				vert.u = u2; vert.v = v1;
				pvr_list_prim(list, &vert, sizeof(vert));
			} else {
				// no clipping version
				// (u1 = v1 = 0.0f)
				float u2 = float(con_width) / float(tex_width);
				float v2 = float(con_height) / float(tex_height);
				
				vert.z = layer;
				vert.flags = PVR_CMD_VERTEX;

				vert.x = 0; vert.y = con_height;
				vert.u = 0; vert.v = v2;
				mat_trans_single(vert.x, vert.y, dummy);
				pvr_list_prim(list, &vert, sizeof(vert));
				
				vert.x = 0; vert.y = 0; 
				vert.u = 0; vert.v = 0;
				mat_trans_single(vert.x, vert.y, dummy);
				pvr_list_prim(list, &vert, sizeof(vert));

				vert.x = con_width; vert.y = con_height;
				vert.u = u2; vert.v = v2;
				mat_trans_single(vert.x, vert.y, dummy);
				pvr_list_prim(list, &vert, sizeof(vert));

				vert.flags = PVR_CMD_VERTEX_EOL;

				vert.x = con_width; vert.y = 0;
				mat_trans_single(vert.x, vert.y, dummy);
				vert.u = u2; vert.v = 0;
				pvr_list_prim(list, &vert, sizeof(vert));
			}
		}
	}

}

