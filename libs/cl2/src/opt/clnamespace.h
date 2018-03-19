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

#ifndef CLNAMESPACE_H
#define CLNAMESPACE_H

#include "../value/clvalue.h"
#include "../value/cluserdata.h"
#include "../value/clobject.h"

#include "../serialize/clserializer.h"

#include <string>
#include <map>

typedef void(*CLNamespaceIntWriter)(int);
typedef int(*CLNamespaceIntReader)();

class CLNamespace : public CLUserData
{
public:
	CLNamespace(CLContext *context);
	virtual ~CLNamespace();

	bool Add(const std::string &id, int *int_ref, bool rw = true);
	bool Add(const std::string &id, std::string *str, bool rw = true);
	bool Add(const std::string &id, CLNamespace *ns);
	bool Add(const std::string &id, bool *bool_ref, bool rw = true);
	bool Add(const std::string &id, float *float_ref, bool rw = true);
	bool Add(const std::string &id, CLNamespaceIntReader r, CLNamespaceIntWriter w);
	bool Add(const std::string &id, CLUserData *obj_ref);

	// access values by key 
	virtual void set(CLValue &key, CLValue &val);
	virtual bool get(CLValue &key, CLValue &val);

	//virtual CLValue clone(); // clone
	//virtual std::string toString(); // to string..

	// iteration support
	//CLValue begin();
	//CLValue next(CLValue iterator, CLValue &key, CLValue &value);

	// load/save
	//static CLNamespace *load(class CLSerializer &ss);
	//static void save(class CLSerializer &ss, CLNamespace *O);

private:
	virtual void markReferenced(); // GC

	struct ValueRef
	{
		enum Type
		{
			INTEGER, // int
			STRING,  // std::string
			//CSTRING, // char *
			BOOLEAN,
			NAMESPACE,
			INTEGER_FN, // int, callbacks
			OBJECT // CLObject ref
		};
		
		Type t;
		bool rw; // writable?
		void *ref;
		void *ref2;
	};

	typedef std::map<std::string, ValueRef> MapType;
	MapType map;

	ValueRef *GetValueRef(const std::string &id);
	bool AddRef(const std::string &id, ValueRef::Type ref_type, void *ref, void *ref2, bool rw);
};

#endif
