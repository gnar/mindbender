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

#ifndef CLVALUE_H
#define CLVALUE_H

#include <vector>
#include <list>
#include <string>

#define CL_RAW_NULL             0x00
#define CL_RAW_INTEGER          0x01
#define CL_RAW_FLOAT            0x02
#define CL_RAW_TABLE            0x03
#define CL_RAW_ARRAY            0x04
#define CL_RAW_STRING           0x05
#define CL_RAW_FUNCTION         0x06
#define CL_RAW_EXTERNALFUNCTION 0x07
#define CL_RAW_USERDATA         0x08
#define CL_RAW_THREAD           0x09
#define CL_RAW_BOOLEAN          0x10

#define CL_RAW_ISNUMERIC       0x1000
#define CL_RAW_ISOBJECT        0x2000

enum CLValueType
{
	CL_NULL              = CL_RAW_NULL,
	CL_BOOLEAN           = CL_RAW_BOOLEAN,
	CL_INTEGER           = CL_RAW_INTEGER          | CL_RAW_ISNUMERIC,
	CL_FLOAT             = CL_RAW_FLOAT            | CL_RAW_ISNUMERIC,

	CL_TABLE             = CL_RAW_TABLE            | CL_RAW_ISOBJECT,
	CL_ARRAY             = CL_RAW_ARRAY            | CL_RAW_ISOBJECT,
	CL_STRING            = CL_RAW_STRING           | CL_RAW_ISOBJECT,
	CL_USERDATA          = CL_RAW_USERDATA         | CL_RAW_ISOBJECT,
	CL_FUNCTION          = CL_RAW_FUNCTION         | CL_RAW_ISOBJECT,
	CL_EXTERNALFUNCTION  = CL_RAW_EXTERNALFUNCTION | CL_RAW_ISOBJECT,
	CL_THREAD            = CL_RAW_THREAD           | CL_RAW_ISOBJECT
};

#define GET_OBJECT(v)           ((v).value.object)
#define GET_TABLE(v)            ((CLTable*)(v).value.object)
#define GET_ARRAY(v)            ((CLArray*)(v).value.object)
#define GET_STRING(v)           ((CLString*)(v).value.object)
#define GET_FUNCTION(v)         ((CLFunction*)(v).value.object)
#define GET_EXTERNALFUNCTION(v) ((CLExternalFunction*)(v).value.object)
#define GET_USERDATA(v)         ((CLUserData*)(v).value.object)
#define GET_THREAD(v)           ((CLThread*)(v).value.object)

class CLValue
{
	//CLValue(char *);
	//CLValue(const char*);
public:
	static inline CLValue True()  { CLValue v; v.type = CL_BOOLEAN; v.value.boolean = true;  return v; }
	static inline CLValue False() { CLValue v; v.type = CL_BOOLEAN; v.value.boolean = false; return v; }
	static inline CLValue Null()  { return CLValue(); }

	// Construction & Copy ////////////////////////
	inline CLValue(const CLValue &other) : value(other.value), type(other.type) {}
	inline ~CLValue() {}

	inline CLValue &operator=(const CLValue &other) { value = other.value; type = other.type; return *this; }
	inline void setNull() { type = CL_NULL; }

	explicit inline CLValue() : type(CL_NULL) {}
	explicit inline CLValue(int i) : type(CL_INTEGER) { value.integer = i; }
	explicit inline CLValue(float f) : type(CL_FLOAT) { value.real = f; }
	explicit CLValue(class CLString *str);
	explicit CLValue(class CLTable *table);
	explicit CLValue(class CLArray *array);
	explicit CLValue(class CLFunction *func);
	explicit CLValue(class CLExternalFunction *extfunc);
	explicit CLValue(class CLUserData *userdata);
	explicit CLValue(class CLThread *thread);

	// clone inside object, or copy inside value //
	CLValue clone();

	// Equality & Identity check //////////////////
	bool isEqual(const CLValue &other);

	// Value retrieval/conversion /////////////////
	inline bool isNull()    { return type == CL_NULL; }
	inline bool isBoolean() { return type == CL_BOOLEAN; }
	inline bool isObject()  { return (type & CL_RAW_ISOBJECT)  != 0; }
	inline bool isNumeric() { return (type & CL_RAW_ISNUMERIC) != 0; }

	// type info
	std::string typeString();

	// type casting
	std::string toString();
	float       toFloat();
	int         toInt();
	bool        toBool();
	
	// value getters
	inline bool  getBoolUnsave() { return value.boolean; }
	inline int   getIntUnsave() { return value.integer; }
	inline float getFloatUnsave() { return value.real; }
	
	template <class T> T *getObjectUnsave() { return static_cast<T*>(value.object); }
	template <class T> T *getObject() { return isObject() ? dynamic_cast<T*>(value.object) : 0; }

	// Wrappers ///////////////////////////////////
	CLValue get(const CLValue &k);
	void set(const CLValue &k, const CLValue &v);

	// Type & Value ///////////////////////////////
	union {
		int integer;
		bool boolean;
		float real;
		class CLObject *object;
	} value;

	CLValueType type;

	// CLValue operations /////////////////////////
	// arithmetic
	CLValue op_add(CLValue other);
	CLValue op_sub(CLValue other);
	CLValue op_mul(CLValue other);
	CLValue op_div(CLValue other);
	CLValue op_neg();

	// integer arithmetic
	CLValue op_modulo(CLValue other);
	CLValue op_shl(CLValue other);
	CLValue op_shr(CLValue other);
	CLValue op_bitor(CLValue other);
	CLValue op_bitand(CLValue other);
	CLValue op_bitxor(CLValue other);

	// boolean arithmetic
	CLValue op_booland(CLValue other);
	CLValue op_boolor(CLValue other);
	CLValue op_boolnot();
	
	// comparison
	CLValue op_eq(CLValue other);
	CLValue op_lt(CLValue other);
	CLValue op_gt(CLValue other);
	CLValue op_le(CLValue other);
	CLValue op_ge(CLValue other);

	// load/save //////////////////////////////////
	static CLValue load(class CLSerialLoader &S);
	static void save(class CLSerialSaver &S, CLValue V);

	static std::vector<CLValue> loadVector(class CLSerialLoader &S);
	static void saveVector(class CLSerialSaver &S, std::vector<CLValue> V);

	static std::list<CLValue> loadList(class CLSerialLoader &S);
	static void saveList(class CLSerialSaver &S, std::list<CLValue> V);

	// GC: Mark object inside (if any) ////////////
	void markObject();
};

#endif

