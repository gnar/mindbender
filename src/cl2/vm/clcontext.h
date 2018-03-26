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

#ifndef CLCONTEXT_H
#define CLCONTEXT_H

#include "../value/clvalue.h"
#include "clthread.h"
#include "clmodule.h"
#include "clsysmodule.h"

#include <list>
#include <string>

class CLContext
{
public:
	CLContext();
	~CLContext();

	inline CLValue &getRootTable() { return root_table; }

	int countRunningThreads();
	void roundRobin(int timeout = -1);

	// Modules
	void addModule(CLModule *module);
	CLExternalFunctionPtr getExternalFunctionPtr(const std::string &func_id);

	// Save, Load, Clear complete context
	void clear();
	void save(class CLSerialSaver &S);
	void load(class CLSerialLoader &S);

	// Convenience functions
	void addGlobal(const std::string &id, CLValue v);
	CLValue getGlobal(const std::string &id);

private:
	CLValue root_table; // Global variables

	// Threads
	friend class CLThread;
	void   registerThread(CLValue thread); // Called by CLThread constructor
	void unregisterThread(CLValue thread); // Called by CLThread destructor
	std::list<CLValue> threads;

	// Modules
	std::list<CLModule*> modules;
	CLSysModule sys;

	// GC lists
	CLCollectable *gc_heap_list;          // double-linked list of all collectible objects on heap
	CLCollectable *gc_finalize_list;      // double-linked list of all objects waiting to be finalized

	friend CLCollectable::CLCollectable(CLContext *context);
	void addToHeapList(CLCollectable *C);       // add object to heap list
	void moveToFinalizedList(CLCollectable *C); // move object from heap list to finalized list
	void moveToHeapList(CLCollectable *C);      // move object from finalized list to heap list

	// Called by destructor and clear()
	void shutdown();

public:
	// GC
	void markObjects();
	void unmarkObjects();
	void sweepObjects();
	void finalizeObjects();
};

#endif

