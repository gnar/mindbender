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


#include "clvalue.h"
#include "clobject.h"

#include "clstring.h"
#include "cltable.h"
#include "clarray.h"
#include "clfunction.h"
#include "clexternalfunction.h"
#include "cluserdata.h"

#include "../vm/clthread.h"
#include "../vm/clcontext.h"

#include "../serialize/clserialloader.h"
#include "../serialize/clserialsaver.h"

#include <sstream>
#include <iomanip>
#include <cstring>
#include <assert.h>
#include <float.h>

CLValue::CLValue(CLString *str)
{
	type = CL_STRING;
	value.object = str;
}

CLValue::CLValue(CLTable *table)
{
	type = CL_TABLE;
	value.object = table;
}

CLValue::CLValue(CLArray *array)
{
	type = CL_ARRAY;
	value.object = array;
}

CLValue::CLValue(CLFunction *func)
{
	type = CL_FUNCTION;
	value.object = func;
}

CLValue::CLValue(CLExternalFunction *extfunc)
{
	type = CL_EXTERNALFUNCTION;
	value.object = extfunc;
}

CLValue::CLValue(CLUserData *userdata)
{
	type = CL_USERDATA;
	value.object = userdata;
}

CLValue::CLValue(CLThread *thread)
{
	type = CL_THREAD;
	value.object = thread;
}

std::string CLValue::toString()
{
	switch (type)
	{
		case CL_NULL: 	
			return "null";

		case CL_BOOLEAN:
			return value.boolean ? "true" : "false";

		case CL_INTEGER:
		{
			std::stringstream ss;
			ss << value.integer;
			return ss.str();
		}

		case CL_FLOAT:
		{
			if (value.real > FLT_MAX) return "<+inf>";
			if (value.real < FLT_MIN) return "<-inf>";
		
			std::stringstream ss;
			ss << std::fixed << value.real;
			return ss.str();
		}

		default: 
			assert(isObject());
			return GET_OBJECT(*this)->toString();
	}

	return "<error>";
}

std::string CLValue::typeString()
{
	switch (type)
	{
		case CL_NULL: return "null";
		case CL_BOOLEAN: return "boolean";
		case CL_INTEGER: return "integer";
		case CL_FLOAT: return "float";
		case CL_TABLE: return "table";
		case CL_ARRAY: return "array";
		case CL_STRING: return "string";
		case CL_FUNCTION: return "function";
		case CL_EXTERNALFUNCTION: return "external_function";
		case CL_USERDATA: return "userdata";
		case CL_THREAD: return "thread";
	}

	assert(0);
	return "<toString: internal error>";
}

float CLValue::toFloat()
{
	switch (type)
	{
		case CL_INTEGER:
			return float(value.integer);
		case CL_FLOAT:
			return value.real;
		default:
			return 0;
	}
	return 0;
}

int CLValue::toInt()
{
	switch (type)
	{
		case CL_INTEGER:
			return value.integer;
		case CL_FLOAT:
			return int(value.real);
		default:
			return 0;
	}
	return 0;
}

bool CLValue::toBool()
{
	switch (type)
	{
		case CL_BOOLEAN:
			return value.boolean;

		// casts
		case CL_INTEGER:
			return value.integer != 0;
		case CL_NULL:
			return false;
		default:
			if (isObject()) return true;
	}
	return false;
}

bool CLValue::isEqual(const CLValue &other)
{
	return op_eq(other).getBoolUnsave();
}

CLValue CLValue::clone()
{
	if (isObject()) {
		return value.object->clone();
	} else {
		return *this;
	}
}

CLValue CLValue::get(const CLValue &k)
{
	if (!isObject()) return CLValue::Null();

	CLValue key(k), value;
	GET_OBJECT(*this)->get(key, value);
	return value;
}

void CLValue::set(const CLValue &k, const CLValue &v)
{
	if (!isObject()) return;
	
	CLValue key(k), value(v);
	GET_OBJECT(*this)->set(key, value);
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations on Values                                                                                         //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ARITH_OPERATION(op, v1, v2)                                                         \
{                                                                                           \
	if (v1->isNumeric() && v2->isNumeric())                                             \
	{                                                                                   \
		if ((v1->type == CL_INTEGER) && (v2->type == CL_INTEGER)) {                 \
			return CLValue(int(v1->value.integer) op int(v2->value.integer));   \
		} else if ((v1->type == CL_INTEGER) && (v2->type == CL_FLOAT)) {            \
			return CLValue(float(v1->value.integer) op float(v2->value.real));  \
		} else if ((v1->type == CL_FLOAT) && (v2->type == CL_INTEGER)) {            \
			return CLValue(float(v1->value.real) op float(v2->value.integer));  \
		} else {                                                                    \
			return CLValue(float(v1->value.real) op float(v2->value.real));     \
		}                                                                           \
		assert(0);                                                                  \
	}                                                                                   \
}

CLValue CLValue::op_add(CLValue other)
{
	ARITH_OPERATION(+, this, (&other));
	return Null();
}

CLValue CLValue::op_sub(CLValue other)
{
	ARITH_OPERATION(-, this, (&other));
	return Null();
}

CLValue CLValue::op_mul(CLValue other)
{
	ARITH_OPERATION(*, this, (&other));
	return Null();
}

CLValue CLValue::op_div(CLValue other)
{
	if (type == CL_INTEGER)
	{
		if (other.type == CL_FLOAT) {
			return CLValue(float(value.integer) / float(other.value.real));
		} else if (other.type == CL_INTEGER) {
			return CLValue(float(value.integer) / float(other.value.integer));
		}
	} else if (type == CL_FLOAT) {
		if (other.type == CL_FLOAT) {
			return CLValue(float(value.real) / float(other.value.real));
		} else if (other.type == CL_INTEGER) {
			return CLValue(float(value.real) / float(other.value.integer));
		}
	}

	return CLValue::Null();
}

CLValue CLValue::op_neg()
{
	switch (this->type)
	{
		case CL_INTEGER: return CLValue(- this->value.integer);
		case CL_FLOAT: return CLValue(- this->value.real);
		default: assert(0);
	}
	return False();
}

#undef ARITH_OPERATION


#define INTEGER_OPERATION(op, v1, v2) \
{ \
	if ((v1->type == CL_INTEGER) && (v2->type == CL_INTEGER)) \
	{ \
		return CLValue(v1->value.integer op v2->value.integer); \
	} \
}

CLValue CLValue::op_modulo(CLValue other)
{
	INTEGER_OPERATION(%, this, (&other));
	return Null();
}

CLValue CLValue::op_shl(CLValue other)
{
	INTEGER_OPERATION(<<, this, (&other));
	return Null();
}

CLValue CLValue::op_shr(CLValue other)
{
	INTEGER_OPERATION(>>, this, (&other));
	return Null();
}

CLValue CLValue::op_bitor(CLValue other)
{
	INTEGER_OPERATION(|, this, (&other));
	return Null();
}

CLValue CLValue::op_bitand(CLValue other)
{
	INTEGER_OPERATION(&, this, (&other));
	return Null();
}

CLValue CLValue::op_bitxor(CLValue other)
{
	INTEGER_OPERATION(^, this, (&other));
	return Null();
}

#undef INTEGER_OPERATION



CLValue CLValue::op_booland(CLValue other)
{
	return CLValue(this->toBool() && other.toBool());
}

CLValue CLValue::op_boolor(CLValue other)
{
	return CLValue(this->toBool() || other.toBool());
}

CLValue CLValue::op_boolnot()
{
	return CLValue(!toBool());
}




#define ARITH_COMPARE_OPERATION(op, v1, v2)                                                            \
{                                                                                                      \
	if (v1->isNumeric() && v2->isNumeric())                                                        \
	{                                                                                              \
		if ((v1->type == CL_INTEGER) && (v2->type == CL_INTEGER)) {                            \
			return (int(v1->value.integer) op int(v2->value.integer)) ? True() : False();  \
		} else if ((v1->type == CL_INTEGER) && (v2->type == CL_FLOAT)) {                       \
			return (float(v1->value.integer) op float(v2->value.real)) ? True() : False(); \
		} else if ((v1->type == CL_FLOAT) && (v2->type == CL_INTEGER)) {                       \
			return (float(v1->value.real) op float(v2->value.integer)) ? True() : False(); \
		} else {                                                                               \
			return (float(v1->value.real) op float(v2->value.real)) ? True() : False();    \
		}                                                                                      \
		assert(0);                                                                             \
	}                                                                                              \
}


CLValue CLValue::op_eq(CLValue other)
{
	// two numbers are equal when they are the same value...
	ARITH_COMPARE_OPERATION(==, this, (&other));

	// values of different types other than numerics are never equal
	if (this->type != other.type) return CLValue::False();

	// null == null
	if (this->type == CL_NULL) return CLValue::True();
	
	// booleans
	if (this->type == CL_BOOLEAN) return CLValue(value.boolean == other.value.boolean);

	// from here on we compare 2 *objects* of the same type.

	// two objects are equal if they are identical
	if (this->value.object == other.value.object) return CLValue::True();

	// two strings are equal if they are ..equal
	if (this->type == CL_STRING)
	{
		if ((GET_STRING(other)->get() == GET_STRING(*this)->get())) return CLValue::True();
		return False();
	}
	
	// two external functions are equal if they have the same id
	if (this->type == CL_EXTERNALFUNCTION)
	{
		if ((GET_EXTERNALFUNCTION(other)->getFuncID() == GET_EXTERNALFUNCTION(*this)->getFuncID())) return True();
		return False();
	}

	return False();
}

CLValue CLValue::op_lt(CLValue other)
{
	ARITH_COMPARE_OPERATION(<, this, (&other));
	return False();
}

CLValue CLValue::op_gt(CLValue other)
{
	ARITH_COMPARE_OPERATION(>, this, (&other));
	return False();
}

CLValue CLValue::op_le(CLValue other)
{
	ARITH_COMPARE_OPERATION(<=, this, (&other));
	return False();
}

CLValue CLValue::op_ge(CLValue other)
{
	ARITH_COMPARE_OPERATION(>=, this, (&other));
	return False();
}

#undef ARITH_COMPARE_OPERATION

#define STACKREF 0x000000FF

/*static member*/
CLValue CLValue::load(class CLSerialLoader &S)
{
	int id, tmp;
	S.IO(id);
	
	switch (id)
	{
		case CL_RAW_NULL:
			return Null();

		case CL_RAW_INTEGER:
			S.IO(tmp);
			return CLValue(tmp);

		case CL_RAW_FLOAT:
		{
			float f;
			S.IO(f);
			return CLValue(f);
		}
			

		case CL_RAW_STRING:
		{
			CLString *s = CLString::load(S);
			return CLValue(s);
		}

		case CL_RAW_ARRAY:
		{
			CLArray *a = CLArray::load(S);
			return CLValue(a);
		}

		case CL_RAW_FUNCTION:
		{
			CLFunction *f = CLFunction::load(S);
			return CLValue(f);
		}

		case CL_RAW_EXTERNALFUNCTION:
		{
			CLExternalFunction *f = CLExternalFunction::load(S);
			return CLValue(f);
		}

		case CL_RAW_TABLE:
		{
			CLTable *t = CLTable::load(S);
			return CLValue(t);
		}

		case CL_RAW_USERDATA:
		{
			CLUserData *u = CLUserData::load(S);
			return CLValue(u);
		}

		case CL_RAW_THREAD:
		{
			CLThread *t = CLThread::load(S);
			return CLValue(t);
		}
			
		case STACKREF: 
		{
			int ref_id; S.IO(ref_id);
			CLObject *obj = (CLObject*)S.getPtr(ref_id);
			if (dynamic_cast<CLString*>(obj)) return CLValue((CLString*)obj);
			if (dynamic_cast<CLArray*>(obj)) return CLValue((CLArray*)obj);
			if (dynamic_cast<CLFunction*>(obj)) return CLValue((CLFunction*)obj);
			if (dynamic_cast<CLExternalFunction*>(obj)) return CLValue((CLExternalFunction*)obj);
			if (dynamic_cast<CLTable*>(obj)) return CLValue((CLTable*)obj);
			if (dynamic_cast<CLUserData*>(obj)) return CLValue((CLUserData*)obj);
			if (dynamic_cast<CLThread*>(obj)) return CLValue((CLThread*)obj);
			assert(0);
		}

	}
	
	assert(0);
	return CLValue::Null();
}

/*static member*/
void CLValue::save(class CLSerialSaver &S, CLValue V)
{
	int id, tmp;

	// handle non-objects
	switch (V.type)
	{
		case CL_NULL:
			S.IO(id = CL_RAW_NULL);
			return;

		case CL_INTEGER:
			S.IO(id = CL_RAW_INTEGER);
			S.IO(tmp = V.toInt());
			return;

		case CL_FLOAT:
		{
			float f = V.toFloat();
			S.IO(id = CL_RAW_FLOAT);
			S.IO(f);
			return;
		}

		default:
			break; //handled below
	}

	// handle objects
	int ref_id = S.findPtr(GET_OBJECT(V));
	if (ref_id != -1) // object was already written? -> write reference id
	{
		S.IO(id = STACKREF);
		S.IO(ref_id);
		return;
	}

	// object was not yet written? -> serialize it
	ref_id = S.addPtr(GET_OBJECT(V));
	switch (V.type)
	{
		case CL_STRING:
			S.IO(id = CL_RAW_STRING);
			CLString::save(S, GET_STRING(V));
			break;

		case CL_ARRAY:
			S.IO(id = CL_RAW_ARRAY);
			CLArray::save(S, GET_ARRAY(V));
			break;

		case CL_FUNCTION:
			S.IO(id = CL_RAW_FUNCTION);
			CLFunction::save(S, GET_FUNCTION(V));
			break;

		case CL_EXTERNALFUNCTION:
			S.IO(id = CL_RAW_EXTERNALFUNCTION);
			CLExternalFunction::save(S, GET_EXTERNALFUNCTION(V));
			break;
		
		case CL_TABLE:
			S.IO(id = CL_RAW_TABLE);
			CLTable::save(S, GET_TABLE(V));
			break; 

		case CL_USERDATA:
			S.IO(id = CL_RAW_USERDATA);
			CLUserData::save(S, GET_USERDATA(V));
			break; 

		case CL_THREAD:
			S.IO(id = CL_RAW_THREAD);
			CLThread::save(S, GET_THREAD(V));
			break;

		default: assert(0);
	}
}

#undef STACKREF

// static member
void CLValue::saveVector(CLSerialSaver &S, std::vector<CLValue> V)
{
	unsigned int size = V.size(); S.IO(size);
	std::vector<CLValue>::iterator it = V.begin(), end = V.end();
	for (; it!=end; ++it)
	{
		CLValue::save(S, *it);
	}
}

// static member
std::vector<CLValue> CLValue::loadVector(CLSerialLoader &S)
{
	std::vector<CLValue> V;
	unsigned int size; S.IO(size);
	for (unsigned i=0; i<size; ++i) 
	{
		V.push_back(CLValue::load(S));
	}
	return V;
}

/* static member */
void CLValue::saveList(CLSerialSaver &S, std::list<CLValue> V)
{
	unsigned int size = V.size(); S.IO(size);
	std::list<CLValue>::iterator it = V.begin(), end = V.end();
	for (; it!=end; ++it)
	{
		CLValue::save(S, *it);
	}
}

/* static member */
std::list<CLValue> CLValue::loadList(CLSerialLoader &S)
{
	std::list<CLValue> V;
	unsigned int size; S.IO(size);
	for (unsigned i=0; i<size; ++i) 
	{
		V.push_back(CLValue::load(S));
	}
	return V;
}

// GC: Mark object inside (if any)
void CLValue::markObject()
{
	if (isObject()) // objects are collectable
	{
		value.object->mark();
	}
}

