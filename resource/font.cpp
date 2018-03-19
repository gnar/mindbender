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

#include <iostream>
using namespace std;

namespace Res
{
	Font::Font(const Resource::ID &id) : Resource(Resource::FONT, id)
	{
	}

	Font::~Font()
	{
	}

	void Font::DrawCenter(int x, int y, const std::string &text, FontVisual *visual)
	{
		y -= GetHeight()/2;
		x -= GetWidth(text)/2;
		Draw(x, y, text, visual);
	}

	int Font::GetWidth(const std::string &text)
	{
		size_t str_len = text.size();
		int result = GetSpaceWidth() * (str_len-1);
		for (size_t i=0; i<str_len; ++i)
		{
			result += GetWidth(text[i]);
		}
		return result;
	}

	void Font::DrawBoxed(int x, int y, int width, const std::string &str, FontVisual *visual)
	{
		const char *txt = str.c_str();
		unsigned length = str.length();

		std::vector<std::string> lines;

		std::string line;
		int line_len = 0;

		std::string word;
		int word_len = 0;
		
		unsigned pos1 = 0;
		unsigned pos2 = 0;

		bool done = false;
		while (!done)
		{
			while ((txt[pos2] != ' ') && (pos2 < length)) pos2++;
			word = str.substr(pos1, pos2-pos1);
			pos1 = pos2++;

			word_len = GetWidth(word);

			if ((word_len + line_len <= width) && (line.length() > 0))
			{
				line_len += word_len;
				line += word;
			} else {
				lines.push_back(line);
				
				line = word;
				line_len = word_len;
			}
			
			if (pos2 >= length) done = true;
		}

		if (line.length() > 0) lines.push_back(line);

		int h = GetHeight();
		int num_lines = lines.size();
		for (size_t i=0; i<lines.size(); ++i)
		{
			DrawCenter(x, y - h*(num_lines-i), lines[i], visual); 
		}
	}

	Font *CreateFontResource(const Resource::ID &id)
	{
		if (!Manager.ExistsFile(id)) return 0; // file not found

		Font *S = 0; // return value

		// Choose between font implementations by file extension
		std::string ext = GetFileExtension(id);
		if (ext == "font")
		{
			return new Font_Bitmap(id);
		}

		if (ext == "ttf")
		{
			// TODO
		}

		return S;
	}


} //ns

