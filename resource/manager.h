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

#ifndef RES_MEDIA_H
#define RES_MEDIA_H

#include <string>
#include <vector>

#include "resource.h"

#include "physfs/physfs.h"
#include <SDL/SDL.h>

namespace Res
{
	class Image;
	class Sprite;
	class Polygon;
	class Font;
	class Sound;

	class Manager_
	{
	public:
		void ShutDown();
		
		void AddSource(const std::string &path);

		Resource *Get(Resource::Type type, const Resource::ID &id);

		Image     *GetImage(const Resource::ID &id) { return (Image*)  Get(Resource::IMAGE,   id); }
		Sprite   *GetSprite(const Resource::ID &id) { return (Sprite*) Get(Resource::SPRITE,  id); }
		Polygon *GetPolygon(const Resource::ID &id) { return (Polygon*)Get(Resource::POLYGON, id); }
		Font       *GetFont(const Resource::ID &id) { return (Font*)   Get(Resource::FONT,    id); }
		Sound     *GetSound(const Resource::ID &id) { return (Sound*)  Get(Resource::SOUND,   id); }

		// return a physfs file which reads from resource 'id'
		PHYSFS_file  *OpenFile(const std::string &id);
		bool        ExistsFile(const std::string &id);

	private:
		Resource *Find(Resource::Type type, const Resource::ID &id);

		std::vector<Resource*> resources;
	};

	extern Manager_ Manager;

	std::string GetFileExtension(const std::string &path);
	std::string SimplifyPath(const std::string &path);
	std::string StringToLower(const std::string &str);

	// SDL_image over physfs glue code (SDL_RWops)
	SDL_RWops *CreatePhysfsRWops(PHYSFS_file *handle);
}

#endif

