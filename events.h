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

#ifndef EVENTS_H
#define EVENTS_H

#include "cl2/cl2.h"

#include <string>
#include <vector>
#include <list>
#include <map>

class EventHandler
{
public:
	// CONSTRUCTOR/DESTRUCTOR //////////////////////////////////////
	EventHandler();
	EventHandler(const std::string &event_class, const std::string &event_name);
	~EventHandler();

	// MODIFY MATCHING EVENT ARGUMENTS /////////////////////////////
	void SetArgumentCount(size_t argc);
	void SetArgument(size_t n_arg, CLValue value);
	void SetArgument(size_t n_arg);
	void SetFunction(CLValue func);

	// CHECK IF A PARAMETER SET MATCHES ////////////////////////////
	const std::string &GetName() { return event_name; }
	const std::string &GetClass() { return event_class; }
	size_t GetArgumentCount() { return args.size(); }
	unsigned Match(const std::string &name, std::vector<CLValue> &values); // returns a score, or 0 if no match

	// EXECUTE EVENT ///////////////////////////////////////////////
	CLValue Execute(CLContext *context, std::vector<CLValue> &values); // execute, permitting multithreading

	// GARBAGE COLLECTION
	void MarkObjects();

	// SAVE & LOAD /////////////////////////////////////////////////
	void Save(CLSerialSaver &S);
	void Load(CLSerialLoader &S);

	// PRIVATE MEMBERS /////////////////////////////////////////////
private:
	std::string event_class;
	std::string event_name;
	CLValue event_function;

	struct EventArgument
	{
		EventArgument() : wildcard(true) {}
		~EventArgument() {}

		bool wildcard;
		CLValue value; // must be null if 'wildcard'
	};
	std::vector<EventArgument> args;
};

class EventManager
{
public:
	// CONSTRUCTOR/DESTRUCTOR //////////////////////////////////////
	EventManager();
	~EventManager();

	void Clear();
	void Update(float dt); // each frame

	// ADD EVENT HANDLERS //////////////////////////////////////////
	void AddEventHandler(EventHandler handler);

	// SUSPEND/RESUME EVENTS BY EVENT CLASS ////////////////////////
	void SuspendEvents(const std::string &event_class);
	void ResumeEvents(const std::string &event_class);
	bool IsSuspended(const std::string &event_class);

	// SIGNAL EVENTS ///////////////////////////////////////////////
	CLValue Signal(CLContext *context, const std::string &event_name, std::vector<CLValue> &values);

	// GARBAGE COLLECTION //////////////////////////////////////////
	void MarkObjects();

	// SAVE & LOAD /////////////////////////////////////////////////
	void Save(CLSerialSaver &S);
	void Load(CLSerialLoader &S);
	
	// PRIVATE MEMBERS /////////////////////////////////////////////
private:
	typedef std::list<EventHandler> EventHandlerList;
	EventHandlerList handlers;

	void AddEventClassEntry(const std::string &event_class);

	struct EventClass
	{
		EventClass() : suspended(0) {}
		~EventClass() {}

		int suspended; // event class disabled? (reference counted)
		CLValue thread; // currently running event handler.
	};

	typedef std::map<std::string, EventClass> EventClassMap;
	EventClassMap classes;
};

#endif

