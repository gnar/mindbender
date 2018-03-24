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

#ifndef RES_RESOURCE_H
#define RES_RESOURCE_H

#include <string>

namespace Res {
    class Resource {
        friend class Manager_;

    public:
        typedef std::string ID;

        enum Type {
            IMAGE,  // An image, used by sprites
            SPRITE, // animated or still sprites
            POLYGON,// description of a polygon
            FONT,   // A ttf or a bitmap font
            SOUND,  // A .wav sample or .ogg stream
        };

        Resource(Resource::Type type, const Resource::ID &id);
        virtual ~Resource() = default;

        void Lock();
        void Unlock();

        ID GetID() const { return id; }
        Type GetType() const { return type; }
        int GetRefCount() const { return ref_count; }

        void TryUnload() { if (ref_count == 0) Unload(); }
        bool IsLoaded() { return loaded; }

    private:
        virtual void Load() = 0;
        virtual void Unload() = 0;

        int ref_count;

    protected:
        bool loaded;

    private:
        const Type type;
        const ID id;
    };
} //ns

#endif

