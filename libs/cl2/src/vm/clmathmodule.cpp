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

#include "clmathmodule.h"
#include "clcontext.h"
#include "clthread.h"

#include "../value/clvalue.h"
#include "../value/clstring.h"
#include "../value/cltable.h"
#include "../value/clexternalfunction.h"

#include <cmath>

#define DECL_FUNC(name) CLValue name(CLThread &thread, std::vector<CLValue> &args, CLValue self)

#ifndef M_PI
#define M_PI 3.1416
#endif

static float toDeg(float rad)
{
	return (180.0f * rad) / (float)M_PI;
}

static float toRad(float deg)
{
	return deg / 180.0f * (float)M_PI;
}

static DECL_FUNC(math_sin);
static DECL_FUNC(math_cos);
static DECL_FUNC(math_tan);
static DECL_FUNC(math_asin);
static DECL_FUNC(math_acos);
static DECL_FUNC(math_atan);
static DECL_FUNC(math_sqrt);
static DECL_FUNC(math_random);
static DECL_FUNC(math_atan2);
static DECL_FUNC(math_min);
static DECL_FUNC(math_max);
static DECL_FUNC(math_min_rank);
static DECL_FUNC(math_max_rank);


CLMathModule::CLMathModule() : CLModule("math")
{
	registerFunction("sin",      "math_sin",      &math_sin);
	registerFunction("cos",      "math_cos",      &math_cos);
	registerFunction("tan",      "math_tan",      &math_tan);
	registerFunction("asin",     "math_asin",     &math_asin);
	registerFunction("acos",     "math_acos",     &math_acos);
	registerFunction("atan",     "math_atan",     &math_atan);
	registerFunction("sqrt",     "math_sqrt",     &math_sqrt);
	registerFunction("random",   "math_random",   &math_random);
	registerFunction("atan2",    "math_atan2",    &math_atan2);
	registerFunction("min",      "math_min",      &math_min);
	registerFunction("max",      "math_max",      &math_max);
	registerFunction("max_rank", "math_max_rank", &math_max_rank);
	registerFunction("min_rank", "math_min_rank", &math_min_rank);
}

CLMathModule::~CLMathModule()
{
}

static inline float float_arg0(std::vector<CLValue> &args)
{
	float result = 0.0f;
	if (args.size() >= 1) result = args[0].toFloat();
	return result;
}

static DECL_FUNC(math_sin)
{
	return CLValue(float(std::sin(toRad(args[0].toFloat()))));
}

static DECL_FUNC(math_cos)
{
	return CLValue(float(std::cos(toRad(args[0].toFloat()))));
}

static DECL_FUNC(math_tan)
{
	return CLValue(float(std::tan(toRad(args[0].toFloat()))));
}

static DECL_FUNC(math_asin)
{
	float v = float_arg0(args);
	if (v < -1.0f || v > 1.0f) return CLValue::Null();
	return CLValue(toDeg(float(std::asin(v))));
}

static DECL_FUNC(math_acos)
{
	float v = float_arg0(args);
	if (v < -1.0f || v > 1.0f) return CLValue::Null();
	return CLValue(toDeg(float(std::acos(v))));
}

static DECL_FUNC(math_atan)
{
	return CLValue(toDeg(float(std::atan(float_arg0(args)))));
}

static DECL_FUNC(math_sqrt)
{
	float v = float_arg0(args);
	if (v < 0) return CLValue::Null();
	return CLValue(float(std::sqrt(v)));
}

static DECL_FUNC(math_random)
{
	switch (args.size())
	{
		case 0: return CLValue((int)std::rand());
		case 1: 
		{
			int i = args[0].toInt();
			return CLValue(int(std::rand() % i));
		}
	}	

	return CLValue::Null();
}

static DECL_FUNC(math_atan2)
{
	return CLValue(toDeg(std::atan2(args[0].toFloat(), args[1].toFloat())));
}

static DECL_FUNC(math_min)
{
	if (args.size() == 0) return CLValue::Null();

	CLValue result = args[0];
	float c_result = result.toFloat();
	for (size_t i=1; i<args.size(); ++i)
	{
		float c_i = args[i].toFloat();
		if (c_i < c_result)
		{
			result = args[i];
			c_result = c_i;
		}
	}
	return result;
}

static DECL_FUNC(math_max)
{
	if (args.size() == 0) return CLValue::Null();

	CLValue result = args[0];
	float c_result = result.toFloat();
	for (size_t i=1; i<args.size(); ++i)
	{
		float c_i = args[i].toFloat();
		if (c_i > c_result)
		{
			result = args[i];
			c_result = c_i;
		}
	}
	return result;
}

static DECL_FUNC(math_min_rank)
{
	if (args.size() == 0) return CLValue::Null();

	int result = 0;
	float max = args[0].toFloat();
	for (size_t i=1; i<args.size(); ++i)
	{
		float value = args[i].toFloat();
		if (value < max)
		{
			result = i;
			max = value;
		}
	}
	return CLValue(result);
}

static DECL_FUNC(math_max_rank)
{
	if (args.size() == 0) return CLValue::Null();

	int result = 0;
	float max = args[0].toFloat();
	for (size_t i=1; i<args.size(); ++i)
	{
		float value = args[i].toFloat();
		if (value > max)
		{
			result = i;
			max = value;
		}
	}
	return CLValue(result);
}
