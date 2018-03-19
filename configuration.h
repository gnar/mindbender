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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "tinyxml/tinyxml.h"

#include <string>
#include <vector>

class Configuration
{
public:
	Configuration();

	// RETRIEVE CONFIGURATION //////////////////////////////////
	const std::vector<std::string> &GetSources() { return sources; }

	const std::string &GetTitle() { return title; }
	const std::string &GetSceneFile() { return scene_file; }

	void GetVideoSettings(int &width, int &height, bool &fullscreen);
	int  GetVideoHeight() { return video.height; }
	int  GetVideoWidth() { return video.width; }
	bool GetFullscreen() { return video.fullscreen; }

	// PARSE CONFIG FILE ///////////////////////////////////////
	void Parse(const std::string &file);
	bool Error() { return err; }
	const std::string &ErrorString() { return error_string; }

private:
	struct 
	{
		int width, height;
		bool fullscreen;
	} video;

	std::string title;
	std::string scene_file;
	std::vector<std::string> sources;

	bool err;
	std::string error_string;
};

#endif

