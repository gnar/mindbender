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

#ifndef LOAD_XML_H
#define LOAD_XML_H

#include <sstream>
#include "tinyxml/tinyxml.h"

// Load an TiXmlDocument from a file
static TiXmlDocument *LoadXMLDocument(PHYSFS_File *handle) {
    auto *data = new char[64 * 1024];
    long size = PHYSFS_read(handle, data, 1, 64 * 1024);
    data[size] = 0;
    PHYSFS_close(handle);

    // load data into a stream
    std::stringstream sstr;
    sstr << data;
    delete[] data;

    // stream data into xml parser
    auto *doc = new TiXmlDocument();
    sstr >> (*doc); // Parse XML
    return doc;
}

#endif

