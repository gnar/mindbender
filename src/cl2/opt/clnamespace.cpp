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

#include "clnamespace.h"

#include "../vm/clcontext.h"
#include "../value/clstring.h"

#include <iostream>
using namespace std;

CLNamespace::CLNamespace(CLContext *context) : CLUserData(context)
{
}

CLNamespace::~CLNamespace()
{
}

bool CLNamespace::Add(const std::string &id, int *int_ref, bool rw)
{
	return AddRef(id, ValueRef::INTEGER, int_ref, 0, rw);
}

bool CLNamespace::Add(const std::string &id, bool *bool_ref, bool rw)
{
	return AddRef(id, ValueRef::BOOLEAN, bool_ref, 0, rw);
}

bool CLNamespace::Add(const std::string &id, CLNamespace *ns)
{
	return AddRef(id, ValueRef::NAMESPACE, ns, 0, false);
}

bool CLNamespace::Add(const std::string &id, std::string *str, bool rw)
{
	return AddRef(id, ValueRef::STRING, str, 0, rw);
}

bool CLNamespace::Add(const std::string &id, CLNamespaceIntReader r, CLNamespaceIntWriter w)
{
	bool rw = r && w;
	return AddRef(id, ValueRef::INTEGER_FN, (void*)r, (void*)w, rw);
}

bool CLNamespace::Add(const std::string &id, CLUserData *obj_ref)
{
	return AddRef(id, ValueRef::OBJECT, (void*)obj_ref, 0, false);
}


bool CLNamespace::AddRef(const std::string &id, ValueRef::Type ref_type, void *ref, void *ref2, bool rw)
{
	std::string::size_type pos = id.find('.', 0);
	if (pos == std::string::npos) {
		if (GetValueRef(id)) return false; // entry already exists for this key 'id'
		ValueRef r;
		r.ref = ref;
		r.ref2 = ref2;
		r.t = ref_type;
		r.rw = rw;
		map[id] = r;
		//cout << "Added " << id << endl;
		return true;
	} else {
		std::string ns_id(id, 0, pos);
		std::string sub_id(id.begin() + pos+1, id.end());
		
		//cout << " NS ID = " << ns_id << endl;
		//cout << "SUB ID = " << sub_id << endl;

		// Check if sub namespace already exists
		CLNamespace *sub_ns = 0;
		ValueRef *r = GetValueRef(ns_id);
		if (r) {
			if (r->t != ValueRef::NAMESPACE) return false;
			
			// Yes -> Get existing sub ns
			sub_ns = (CLNamespace*)(r->ref);
		} else {
			// No -> Add sub ns
			sub_ns = new CLNamespace(getContext());
			if (!AddRef(ns_id, ValueRef::NAMESPACE, sub_ns, 0, false)) return false;
		}
		
		return sub_ns->AddRef(sub_id, ref_type, ref, ref2, rw);
	}
}

void CLNamespace::set(CLValue &key, CLValue &val)
{
	if (key.type != CL_STRING) return;
	const std::string &id = GET_STRING(key)->get();

	ValueRef *r = GetValueRef(id);
	if (r && r->rw) switch (r->t)
	{
		case ValueRef::INTEGER:
			if (val.type != CL_INTEGER) return;
			*((int*)(r->ref)) = val.toInt();
			break;

		case ValueRef::STRING:  
			if (val.type != CL_STRING) return;
			*((std::string*)(r->ref)) = GET_STRING(val)->get();
			break;

		case ValueRef::BOOLEAN:
			*((bool*)(r->ref)) = val.getBoolUnsave();
			break;

		case ValueRef::INTEGER_FN:
			if (val.type != CL_INTEGER) return;
			CLNamespaceIntWriter(r->ref2)(val.toInt());
			break;

		case ValueRef::NAMESPACE: 
			break; // ns are always ro
	}
}

bool CLNamespace::get(CLValue &key, CLValue &val)
{
	if (key.type != CL_STRING) return false;
	const std::string &id = GET_STRING(key)->get();
	
	ValueRef *r = GetValueRef(id);
	if (r) switch (r->t)
	{
		case ValueRef::INTEGER:   val = CLValue(*((int*)(r->ref))); return true;
		case ValueRef::STRING:    val = CLValue(new CLString(getContext(), *((std::string*)(r->ref)))); return true;
		case ValueRef::NAMESPACE: val = CLValue((CLUserData*)(r->ref)); return true;
		case ValueRef::BOOLEAN:   val = *((bool*)(r->ref)) ? CLValue::True() : CLValue::False(); return true;
		case ValueRef::INTEGER_FN:val = CLValue(CLNamespaceIntReader(r->ref)()); return true;
		case ValueRef::OBJECT:    val = CLValue((CLUserData*)r->ref); return true;
	}

	return false;
}

CLNamespace::ValueRef *CLNamespace::GetValueRef(const std::string &id)
{
	MapType::iterator it = map.find(id);
	if (it == map.end()) 
		return 0; // not found
	else
		return &(it->second); // found
}

void CLNamespace::markReferenced()
{
	MapType::iterator it;
	for (it=map.begin(); it!=map.end(); ++it)
	{
		if (it->second.t == ValueRef::OBJECT)
		{
			CLValue c((CLUserData*)it->second.ref);
			c.markObject();
		}

		if (it->second.t == ValueRef::NAMESPACE)
		{
			CLValue c((CLNamespace*)it->second.ref);
			c.markObject();
		}
	}
}
