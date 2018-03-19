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

#ifndef GAMELOADER_H
#define GAMELOADER_H

#include "scenelexer.h"
#include "sceneparser.h"

#include "cl2/cl2.h"

#include <string>
#include <list>

class GameLoader : private SceneParser
{
public:
	GameLoader(CLContext *context, const std::string &fn);
	~GameLoader();

	void Parse();

	void RunPrologScripts();
	void ParseScenes();
	void RunSceneScripts();
	void RunStartScripts();

private:
	CLContext *context;
	
	std::list<std::string> prolog_scripts;
	std::list<std::string> run_scripts;
	std::list<class SceneLoader*> scene_loaders;
};

#endif
