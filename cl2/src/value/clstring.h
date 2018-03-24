/*
    This file is part of the CL2 script language interpreter.

    Gunnar Selke <gunnar@gmx.info>

    CL2 is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    CL2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CL2; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef CLSTRING_H
#define CLSTRING_H

#include "clobject.h"
#include "clvalue.h"

#include <string>

class CLString : public CLObject
{
public:
	CLString(class CLContext *context, const char *cstr = "");
	CLString(class CLContext *context, const std::string &str);
	virtual ~CLString();

	const std::string &get() { return value; }
	void set(const std::string &str) { cache_valid = false; value = str; }

	unsigned int hash();

	// access values by key
	void set(CLValue &key, CLValue &val);
	bool get(CLValue &key, CLValue &val);

	// iteration support
	CLValue begin();
	CLValue next(CLValue iterator, CLValue &key, CLValue &value);
	
	// load/save
	static CLString *load(class CLSerialLoader &S);
	static void save(class CLSerialSaver &S, CLString *O);

	virtual CLValue clone(); // clone
	virtual std::string toString(); // to string..

	// garbage collection
	virtual void markReferenced() {}

private:
	std::string value;
	unsigned int cached_hash;
	bool cache_valid;
};

#endif

