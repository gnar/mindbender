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

#include "clcontext.h"

#include "../value/clvalue.h"
#include "../value/cltable.h"
#include "../value/clstring.h"

#include "clmathmodule.h"

#include <assert.h>
#include <iostream>

#include "../serialize/clserialloader.h"
#include "../serialize/clserialsaver.h"

#include <stdexcept>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction                                                   //
////////////////////////////////////////////////////////////////////////////////

CLContext::CLContext() : gc_heap_list(0), gc_finalize_list(0)
{
	clear();
	addModule(&sys);
}

CLContext::~CLContext()
{
	shutdown();
}

////////////////////////////////////////////////////////////////////////////////
// Context state                                                              //
////////////////////////////////////////////////////////////////////////////////

void CLContext::clear()
{
	shutdown();

	root_table = CLValue(new CLTable(this));

	// reinit all modules
	std::list<CLModule*>::iterator it = modules.begin(), end = modules.end();
	for (;it!=end; ++it)
	{
		(*it)->init(this);
	}
}

void CLContext::shutdown()
{
	// I. Free root table
	root_table.setNull();

	// II. Move all remaining objects on heap to finalize list
	while (gc_heap_list)
	{
		moveToFinalizedList(gc_heap_list); 
	}

	// III. Free finalized objects
	finalizeObjects();

#ifdef DEBUG
	if (gc_heap_list != 0)      clog << "Internal error: gc_heap_list != 0 after shutdown" << endl;
	if (gc_finalize_list != 0)  clog << "Internal error: gc_finalize_list != 0 after shutdown" << endl;
	if (threads.size() != 0)    clog << "Internal error: threads.size() != 0 after shutdown" << endl;
#endif

	// Should be 0 anyway..
	gc_heap_list = 0;
	gc_finalize_list = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Threads                                                                    //
////////////////////////////////////////////////////////////////////////////////

void CLContext::registerThread(CLValue thread) // called by thread constructor
{
	assert(thread.type == CL_THREAD);

	threads.push_back(thread);
}

void CLContext::unregisterThread(CLValue thread) // called by thread destructor
{
	assert(thread.type == CL_THREAD);

	std::list<CLValue>::iterator it = threads.begin(), end = threads.end();
	for (;it!=end;++it)
	{
		if (GET_THREAD(thread) == GET_THREAD(*it)) 
		{
			it = threads.erase(it); 
			return; 
		}
	}

	clog << "Internal vm error: unregisterThread failed." << endl;
	assert(0);
}

int CLContext::countRunningThreads()
{
	int result = 0;

	std::list<CLValue>::iterator it = threads.begin(), end = threads.end();
	for (;it!=end;++it) if (GET_THREAD(*it)->isRunning()) ++result; 

	return result;
}

void CLContext::roundRobin(int timeout)
{
	std::list<CLValue>::iterator it = threads.begin(), end = threads.end();
	for (;it!=end;++it)
	{
		CLValue &thread = *it;
		if (GET_THREAD(thread)->isRunning()) GET_THREAD(thread)->run(timeout);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Modules                                                                    //
////////////////////////////////////////////////////////////////////////////////

CLExternalFunctionPtr CLContext::getExternalFunctionPtr(const std::string &func_id)
{
	std::list<CLModule*>::iterator it = modules.begin(), end = modules.end();
	for (;it!=end;++it)
	{
		CLExternalFunctionPtr p = (*it)->getExternalFunctionPtr(func_id);
		if (p) return p;
	}
	return 0;
}

void CLContext::addModule(CLModule *module)
{
	modules.push_back(module);
	module->init(this);
}

////////////////////////////////////////////////////////////////////////////////
// Convenience functions                                                      //
////////////////////////////////////////////////////////////////////////////////

void CLContext::addGlobal(const std::string &id, CLValue val)
{
	getRootTable().set(CLValue(new CLString(this, id)), val);
}

CLValue CLContext::getGlobal(const std::string &id)
{
	return getRootTable().get(CLValue(new CLString(this, id)));
}

////////////////////////////////////////////////////////////////////////////////
// Serialization                                                              //
////////////////////////////////////////////////////////////////////////////////

void CLContext::save(CLSerialSaver &S)
{
	CLValue::save(S, root_table); // save global environment

	unsigned int tmp;
	S.IO(tmp = threads.size());  // save number of threads

	std::list<CLValue>::iterator it = threads.begin(), end = threads.end();
	for (;it!=end;++it) // save each thread
	{
		CLValue::save(S, *it);
	}
}

void CLContext::load(CLSerialLoader &S)
{
	clear();

	root_table = CLValue::load(S); // load global environment

	unsigned int tmp;
	S.IO(tmp); // load number of threads
	
	for (unsigned i=0; i<tmp; ++i)
	{
		CLValue thr = CLValue::load(S);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Garbage collection main routines                                           //
////////////////////////////////////////////////////////////////////////////////

void CLContext::markObjects()
{
	// mark root table
	root_table.markObject();

	// mark all running threads
	std::list<CLValue>::iterator it = threads.begin(), end = threads.end();
	for (;it!=end;++it) 
	{
		if (GET_THREAD(*it)->isRunning()) it->markObject();
	}
}

void CLContext::sweepObjects()
{
	CLCollectable *it = gc_heap_list;
	while (it)
	{
		CLCollectable *next = it->next;
		if (!it->isMarked()) moveToFinalizedList(it);
		it = next;
	}
}

void CLContext::addToHeapList(CLCollectable *C)
{
	assert(C);

	// add to heap list
	assert(C->prev == 0);
	assert(C->next == 0);

	C->prev = 0;
	C->next = gc_heap_list;
	gc_heap_list = C;

	if (gc_heap_list->next) gc_heap_list->next->prev = gc_heap_list;
}

void CLContext::moveToFinalizedList(CLCollectable *C)
{
	assert(C);

	// remove from heap list
	if (C->prev) C->prev->next = C->next;
	if (C->next) C->next->prev = C->prev;
	if (C == gc_heap_list) gc_heap_list = C->next;

	// add to finalize list
	C->prev = 0;
	C->next = gc_finalize_list;
	gc_finalize_list = C;
	if (gc_finalize_list->next) gc_finalize_list->next->prev = gc_finalize_list;
}

void CLContext::moveToHeapList(CLCollectable *C)
{
	assert(C);

	// remove from finalize list
	if (C->prev) C->prev->next = C->next;
	if (C->next) C->next->prev = C->prev;
	if (C == gc_finalize_list) gc_finalize_list = C->next;

	// add to heap list
	C->prev = 0;
	C->next = gc_heap_list;
	gc_heap_list = C;
	if (gc_heap_list->next) gc_heap_list->next->prev = gc_heap_list;
}

void CLContext::finalizeObjects()
{
	/*
	 * TODO:
	 * Call finalize() on all (unmarked) objects to be finalized. If finalize() returns
	 * false, dont collect that object. Instead, this object is marked, which might
	 * also mark other objects in the finalized list. So then we need to move all marked 
	 * object in the finalize list back to the heap list.
	 */
	
	CLCollectable *it = gc_finalize_list;
	while (it)
	{
		CLCollectable *obj = it; it = it->next;
		delete obj;
	}

	gc_finalize_list = 0;
}

void CLContext::unmarkObjects()
{
	CLCollectable *it = gc_heap_list;
	while (it)
	{
		it->marked = false;
		it = it->next;
	}
}

