/*
    MindBender - The MindBender adventure engine
    Copyright (C) 2006  Gunnar Selke

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "font.h"
#include "manager.h"

#include "font.h"
#include "font_bitmap.h"

#include "loadxml.h"
#include "tinyxml/tinyxml.h"

#include <cstdio>
#include <memory>

#include <iostream>
using namespace std;

extern DCDraw::Canvas LoadPNG(PHYSFS_file *file); // HACK

namespace Res
{
	Font_Bitmap::Font_Bitmap(const Resource::ID &id) : Font(id)
	{
		LoadXML();
	}

	Font_Bitmap::~Font_Bitmap()
	{
		for (size_t i=0; i<256; ++i)
		{
			if (glyphs[i]) delete glyphs[i];
		}
		Unload();
	}

	void Font_Bitmap::Load()
	{
		if (loaded) return;
		
		std::vector<Image*>::iterator it, end = images.end();
		for (it=images.begin(); it!=end; ++it) (*it)->Lock();

		loaded = true;
	}

	void Font_Bitmap::Unload()
	{
		if (!loaded) return;
		
		std::vector<Image*>::iterator it, end = images.end();
		for (it=images.begin(); it!=end; ++it) (*it)->Unlock();

		loaded = false;
	}

	void Font_Bitmap::Draw(int x, int y, const std::string &text, FontVisual *visual)
	{
		size_t len = text.length(); 
		const char *txt = text.c_str();

		DCDraw::Transform trans; 
		DCDraw::Color color;

		if (visual) color = DCDraw::Color(visual->r, visual->g, visual->b, visual->a);

		int cursor_x = 0;
		for (size_t pos=0; pos<len; ++pos)
		{
			unsigned char ch = txt[pos];
			Glyph *glyph = glyphs[ch];

			if (glyph)
			{
				DCDraw::Clip clip(glyph->x, glyph->y, glyph->w, glyph->h);
				trans.SetTrans(x + cursor_x, y);
				images[glyph->image_idx]->GetTexture()->Draw(trans, color, clip);
				
				cursor_x += glyph->w + 1;
			} else {
				cursor_x += 8;
			}
		}
	}

	int Font_Bitmap::GetHeight()
	{
		return height;
	}

	int Font_Bitmap::GetWidth(char ch)
	{
		unsigned char ch2 = (unsigned char)ch;
		if (glyphs[ch2]) return glyphs[ch2]->w;
		if (glyphs[' ']) return glyphs[' ']->w;
		return height/2;
	}

	void Font_Bitmap::LoadXML()
	{
		std::auto_ptr<TiXmlDocument> doc(LoadXMLDocument(Manager.OpenFile(GetID())));

		glyphs.resize(256, (Glyph*)0);

		TiXmlElement *font_elem = doc->FirstChildElement("font");
		TiXmlElement *glyphs_elem  = font_elem ? font_elem->FirstChildElement("glyphs") : 0;
		while (glyphs_elem)
		{
			LoadGlyphElement(glyphs_elem);
			glyphs_elem = glyphs_elem->NextSiblingElement("glyphs");
		}
	}

	void Font_Bitmap::LoadGlyphElement(TiXmlElement *glyph_elem)
	{
		const char *image_attr = glyph_elem->Attribute("image"); assert(image_attr);
		Image *image = Manager.GetImage(image_attr);
		this->images.push_back(image);

		const char *range_attr = glyph_elem->Attribute("range");
		int range_min, range_max;
		if (2 == std::sscanf(range_attr, "%i,%i", &range_min, &range_max))
		{
			ParseImage(this->images.size()-1 /*last item idx*/, image_attr, range_min, range_max);
		} else {
			assert(0);
		}
	}

	void Font_Bitmap::ParseImage(size_t img_idx, const char *file_id, int min, int max)
	{
		DCDraw::Canvas canvas = LoadPNG(Manager.OpenFile(file_id)); // HACK
		this->height = canvas.GetHeight(); // HACK

		unsigned char sep_r, sep_g, sep_b, sep_a;
		canvas.GetPixel(0, 0, sep_r, sep_g, sep_b, sep_a);

		int scan_x = 0;
		int glyph = min;

		while ((scan_x < canvas.GetWidth()) && (glyph < max))
		{
			unsigned char r, g, b, a;
			
			canvas.GetPixel(scan_x, 0, r, g, b, a);
			if (r == sep_r && g == sep_g && b == sep_b) {
				++scan_x; // skip separator pixels
			} else {
				// here, the first pixel of the current character (which is not a separator pixel) 
				// is reached by scan_x

				// upper left corner of glyph rectangle
				const int x1 = scan_x; 
				const int y1 = 0;

			next:
				canvas.GetPixel(scan_x, 0, r, g, b, a);
				if ((r != sep_r || g != sep_g || b != sep_b) && (scan_x < canvas.GetWidth()))
				{
					++scan_x;
					goto next;
				}
				
				if (scan_x < canvas.GetWidth()) 
				{
					// lower right corner
					const int x2 = scan_x;
					const int y2 = this->height;

					// size of glyph
					const int width = x2 - x1;
					const int height = y2 - y1;
					glyphs[glyph] = new Glyph(x1, y1, width, height);
					glyphs[glyph]->image_idx = img_idx;

					//cout << "LADE: img=" << img_idx << " glyph=" << glyph << endl;

					++glyph;
				}
			}
		}

	}


} //ns

