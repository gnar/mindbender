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

#include "configuration.h"

#include <iostream>
using namespace std;

#include <sstream>

Configuration::Configuration() : 
	title("<no title set>"), err(true)
{
}

void Configuration::GetVideoSettings(int &width, int &height, bool &fullscreen)
{
	width = video.width;
	height = video.height;
	fullscreen = video.fullscreen;
}

void Configuration::Parse(const std::string &config_file)
{
	err = false;

	TiXmlDocument doc(config_file.c_str());
	doc.LoadFile();

	if (doc.Error())
	{
		stringstream ss; ss << "Error in config file " << config_file << ": " << doc.ErrorDesc() << endl; 
		err = true;
		error_string = ss.str();
		return;
	}

	TiXmlElement *game = doc.FirstChildElement("game");
	if (!game)
	{
		error_string = "Missing top-level <game> element in configuration file.";
		err = true;
		return;
	}

	// optional game title attribute
	const char *title_c = game->Attribute("title");
	if (title_c == 0)
	{
		title = "<no title set>";
	} else {
		title = std::string(title_c);
	}

	// required scene-file attribute
	const char *scene_file_c = game->Attribute("scene-file");
	if (!scene_file_c) {
		error_string = "Missing 'scene-file' argument in <game> element."; 
		err = true;
		return;
	}
	scene_file = scene_file_c;

	TiXmlElement *child = game->FirstChildElement();
	while (child)
	{
		const std::string v = child->Value();

		if (v == "video") {
			const char *width_str      = child->Attribute("width");		
			const char *height_str     = child->Attribute("height");
			const char *fullscreen_str = child->Attribute("fullscreen");

			if (!width_str || !height_str)
			{
				error_string = "Need width and height arguments in <video> element.";
				err = true;
				return;
			}
	
			video.width      = std::atoi(width_str);
			video.height     = std::atoi(height_str);
			video.fullscreen = fullscreen_str ? (0 != std::atoi(fullscreen_str)) : false;
			
		} else if (v == "audio") {
			// TODO
		} else if (v == "source") {
			const char *path = child->Attribute("path");
			if (!path) {
				error_string = "Missing 'path' argument in <source> element.";
				err = true;
				return;
			} 
			sources.push_back(path);
		}

		child = child->NextSiblingElement();
	}
}

