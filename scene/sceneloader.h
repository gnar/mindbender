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

#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "scenelexer.h"
#include "sceneparser.h"

#include "cl2/cl2.h"

#include "object/room.h"

#include <string>
#include <list>

#include <iostream>

class SceneLoader : private SceneParser
{
public:
	SceneLoader(CLContext *context, const std::string &fn);
	~SceneLoader();

	void Parse(); //run
	void RunAttachedScripts();

private:
	CLContext *context;
	
	CLValue ParseRoom();
	void ParsePlane(Room *room, Room::PlaneID pid);
	CLValue ParseItem();
	CLValue ParseBagItem();
	CLValue ParseSprite();
	CLValue ParseShape();

	void ParseRoomProperty(const std::string id, class Room *room);
	void ParsePlaneProperty(const std::string id, class Room *room, Room::PlaneID pid);
	void ParseItemProperty(const std::string id, class Item *item);
	void ParseBagItemProperty(const std::string id, class BagItem *bagitem);
	void ParseSetSlot(const std::string id, CLObject *obj);

	std::list<std::string> scripts;
};

#endif

