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

#include "manager.h"
#include "image.h"
#include "sprite.h"
#include "polygon.h"
#include "font.h"
#include "sound.h"

#include <iostream>
using namespace std;

#include "physfs/physfs.h"

namespace Res
{
	Manager_ Manager;
	
	void Manager_::ShutDown()
	{
		// Resolve dependencies (resources locking each other)
		int cnt, old_cnt = -1;
		bool done = false;
		while (!done)
		{
			cnt = 0;
			for (size_t i=0; i<resources.size(); ++i)
			{
				Resource *R = resources[i];
				R->TryUnload();
				if (R->GetRefCount() > 0)
				{
					++cnt;
				}
			}
			
			if (cnt == 0) { // Everything resolved?
				done = true;
			} else { // Else, reiterate
				if (cnt == old_cnt) done = true; // Couldn't resolve?
			}
			old_cnt = cnt;
		}
		cout << "Resource manager: " << cnt << " locked resources at shutdown" << endl;
		
		for (size_t i=0; i<resources.size(); ++i)
		{
			Resource *R = resources[i];
			if (R && R->GetRefCount() == 0)
			{
				delete R;
			} else {
				cout << "Resource (id=" << R->GetID() << ") is still locked." << endl;
			}
		}
	}

	void Manager_::AddSource(const std::string &path)
	{
		PHYSFS_addToSearchPath(path.c_str(), 1);
	}

	Resource *Manager_::Find(Resource::Type type, const Resource::ID &id)
	{
		for (size_t i=0; i<resources.size(); ++i)
		{
			Resource *R = resources[i];
			if (R->GetType() == type && R->GetID() == id) return R;
		}
		return 0;
	}

	Resource *Manager_::Get(Resource::Type type, const Resource::ID &id)
	{
		Resource *R = Find(type, id);

		if (!R) // resource not found? => Try to create
		{
			switch (type)
			{
				case Resource::IMAGE:   R = CreateImageResource(id); break;
				case Resource::SPRITE:  R = CreateSpriteResource(id); break;
				case Resource::POLYGON: R = CreatePolygonResource(id); break;
				case Resource::FONT:    R = CreateFontResource(id); break;
				case Resource::SOUND:   R = CreateSoundResource(id); break;
			}

			if (R) {
				resources.push_back(R);
			} else {
				cout << "Could not load resource "<< id << "! (file not found / wrong file extension or format)" << endl;
				return 0;
			}
		}

		return R;
	}

	PHYSFS_file *Manager_::OpenFile(const std::string &id)
	{
		return PHYSFS_openRead(id.c_str()); 
	}
	
	bool Manager_::ExistsFile(const std::string &id)
	{
		return PHYSFS_exists(id.c_str());
	}
	

	std::string GetFileExtension(const std::string &path)
	{
		size_t pos = path.find_last_of('.');
		if (pos == std::string::npos) return "";
		return StringToLower(path.substr(pos + 1));
	}

	std::string StringToLower(const std::string &str)
	{
		std::string result;
		result.resize(str.size(), ' ');
		for (size_t i=0; i<str.size(); ++i) result[i] = tolower(str[i]);
		return result;
	}

	std::string SimplifyPath(const std::string &Path)
	{
		std::string path = StringToLower(Path);
		for (size_t i=0; i<path.size(); ++i) 
		{
			if (path[i] == '\\') path[i] = '/';
		}
		return path;
	}
	
	namespace {
	}

	SDL_RWops *CreatePhysfsRWops(PHYSFS_file *handle)
	{
		return 0;
	}
} //ns

