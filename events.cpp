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

#include "events.h"

#include <memory>

#include "cl2/cl2.h"

#include <iostream>
using namespace std;

////////////////////////////////////////////////////////////////
// CONSTRUCTOR/DESTRUCTOR                                     //
////////////////////////////////////////////////////////////////

EventHandler::EventHandler()
	: event_class("<dummy>"), event_name("<dummy>")
{
	SetArgumentCount(0);
}

EventHandler::EventHandler(const std::string &event_class, const std::string &event_name)
	: event_class(event_class), event_name(event_name)
{
	SetArgumentCount(0);
}

EventHandler::~EventHandler()
{
}

////////////////////////////////////////////////////////////////
// MODIFY MATCHING EVENT ARGUMENTS                            //
////////////////////////////////////////////////////////////////

void EventHandler::SetArgumentCount(size_t argc)
{
	args.resize(argc);
}

void EventHandler::SetArgument(size_t n_arg, CLValue value)
{
	if (n_arg >= GetArgumentCount()) SetArgumentCount(n_arg + 1);

	args[n_arg].wildcard = false;
	args[n_arg].value = value;
}

void EventHandler::SetArgument(size_t n_arg)
{
	if (n_arg >= GetArgumentCount()) SetArgumentCount(n_arg + 1);

	args[n_arg].wildcard = true;
	args[n_arg].value = CLValue();
}

void EventHandler::SetFunction(CLValue func)
{
	event_function = func;
}

////////////////////////////////////////////////////////////////
// CHECK IF A PARAMETER SET MATCHES                           //
////////////////////////////////////////////////////////////////

unsigned EventHandler::Match(const std::string &name, std::vector<CLValue> &values) // returns a score, or 0 if no match
{
	// 1) No match if name is different
	if (name != GetName()) return 0;

	// 2) No match if argc is different
	if (values.size() != GetArgumentCount()) return 0;

	// 3) No match if a non-wildcard argument doesnt match
	// Score: +1 if a wildcard matches
	//        +2 if a non-wildcard matches
	unsigned score = 1;
	for (size_t i=0; i<GetArgumentCount(); ++i)
	{
		if (args[i].wildcard)
		{
			score += 1;
		} else {
			if (values[i].op_eq(args[i].value).getBoolUnsave())
				score += 2;
			else
				return 0;
		}
	}

	return score;
}


////////////////////////////////////////////////////////////////
// EXECUTE EVENT                                              //
////////////////////////////////////////////////////////////////

CLValue EventHandler::Execute(CLContext *context, std::vector<CLValue> &values) // execute, permit multithreading
{
	CLValue thread(new CLThread(context));
	GET_THREAD(thread)->init(event_function, values, CLValue::Null()/*self*/);
	return thread;
}

////////////////////////////////////////////////////////////////
// CL2 GARBAGE COLLECTION                                     //
////////////////////////////////////////////////////////////////
void EventHandler::MarkObjects()
{
	event_function.markObject();

	size_t argc = GetArgumentCount();
	for (size_t i=0; i<argc; ++i)
	{
		args[i].value.markObject();
	}
}

////////////////////////////////////////////////////////////////
// SAVE & LOAD                                                //
////////////////////////////////////////////////////////////////
void EventHandler::Save(CLSerialSaver &S)
{
	S.IO(event_class);
	S.IO(event_name);
	CLValue::save(S, event_function);

	unsigned int argc = (unsigned int)(args.size());
	S.IO(argc);

	for (size_t i=0; i<args.size(); ++i)
	{
		int wildcard = int(args[i].wildcard);
		S.IO(wildcard);

		if (!wildcard) CLValue::save(S, args[i].value);
	}
}

void EventHandler::Load(CLSerialLoader &S)
{
	S.IO(event_class);
	S.IO(event_name);
	event_function = CLValue::load(S);

	unsigned int argc; S.IO(argc);
	args.clear();
	args.resize(size_t(argc));

	for (size_t i=0; i<args.size(); ++i)
	{
		int wildcard; S.IO(wildcard);	
		args[i].wildcard = bool(wildcard);
		
		if (wildcard) 
			args[i].value = CLValue::Null();
		else
			args[i].value = CLValue::load(S);
	}
}



// E V E N T M A N A G E R /////////////////////////////////////


////////////////////////////////////////////////////////////////
// CONSTRUCTOR/DESTRUCTOR                                     //
////////////////////////////////////////////////////////////////

EventManager::EventManager()
{
	Clear();
}

EventManager::~EventManager()
{
}

void EventManager::Clear()
{
	handlers.clear();
	classes.clear();
}

void EventManager::Update(float dt) // each frame
{
	// Release references to finished threads, so they can be gc'd next time.
	EventClassMap::iterator ec_it = classes.begin(), ec_end = classes.end();
	for (;ec_it!=ec_end;++ec_it)
	{
		CLValue &V = ec_it->second.thread;
		if (!V.isNull() && !GET_THREAD(V)->isRunning()) V.setNull();
	}
}

////////////////////////////////////////////////////////////////
// ADD EVENT HANDLERS                                         //
////////////////////////////////////////////////////////////////

void EventManager::AddEventHandler(EventHandler handler)
{
	AddEventClassEntry(handler.GetClass());
	handlers.push_back(handler);
}

////////////////////////////////////////////////////////////////
// SUSPEND/RESUME EVENTS BY EVENT CLASS                       //
////////////////////////////////////////////////////////////////
void EventManager::AddEventClassEntry(const std::string &event_class)
{
	EventClassMap::iterator it = classes.find(event_class);
	if (it != classes.end()) return;

	classes.insert(std::make_pair(event_class, EventClass()));
}

void EventManager::SuspendEvents(const std::string &event_class)
{
	EventClassMap::iterator it = classes.find(event_class);

	if (it == classes.end()) {
		// no entry? create one, retry.
		AddEventClassEntry(event_class);
		SuspendEvents(event_class);
	} else {
		// increase reference count
		it->second.suspended++;
	}
}

void EventManager::ResumeEvents(const std::string &event_class)
{
	EventClassMap::iterator it = classes.find(event_class);

	if ((it == classes.end()) || (it->second.suspended == 0)) {
		clog << "Warning: Tried to resume event class that is not suspended." << endl;
	} else {
		it->second.suspended--;
	}
}

bool EventManager::IsSuspended(const std::string &event_class)
{
	EventClassMap::iterator it = classes.find(event_class);

	if (it == classes.end()) {
		return false;
	} else {
		return (it->second.suspended > 0);
	}
}

////////////////////////////////////////////////////////////////
// SIGNAL EVENTS                                              //
////////////////////////////////////////////////////////////////

CLValue EventManager::Signal(CLContext *context, const std::string &event_name, std::vector<CLValue> &values)
{
	EventHandlerList::iterator it = handlers.begin(), end = handlers.end();
	EventHandler *best_match = 0;
	unsigned best_score = 0;

	while (it != end)
	{
		EventHandler &handler = *it; ++it;
		unsigned score = handler.Match(event_name, values);

		if (score == 0) continue;
		if (score > best_score)
		{
			// check if event class is suspended
			EventClassMap::iterator ec_it = classes.find(handler.GetClass());
			if (ec_it->second.suspended > 0) continue;

			best_score = score;
			best_match = &handler;
		}
	}

	if (best_match == 0) return CLValue();

	// Kill old event thread if any.
	EventClassMap::iterator ec_it = classes.find(best_match->GetClass());
	EventClass &ec = ec_it->second;
	if (!ec.thread.isNull() && (GET_THREAD(ec.thread)->isRunning()))
	{
		//cout << "Killing thread, tag is " << GET_THREAD(ec.thread)->tag << endl;
		GET_THREAD(ec.thread)->kill();
	}
	
	ec.thread = best_match->Execute(context, values);

	return ec.thread;
}

////////////////////////////////////////////////////////////////
// SAVE & LOAD                                                //
////////////////////////////////////////////////////////////////
void EventManager::Save(CLSerialSaver &S)
{
	unsigned int utmp;

	// EventHandlerList handlers
	utmp = (unsigned int)(handlers.size());
	S.IO(utmp);

	for (EventHandlerList::iterator it=handlers.begin(); it!=handlers.end(); ++it) 
	{
		it->Save(S);
	}

	// EventClassMap classes
	utmp = (unsigned int)(classes.size());
	S.IO(utmp);

	for (EventClassMap::iterator it=classes.begin(); it!=classes.end(); ++it)
	{
		std::string class_id = it->first;
		EventClass &EC = it->second;

		S.IO(class_id);
		S.IO(EC.suspended);
		CLValue::save(S, EC.thread);
	}
}

void EventManager::Load(CLSerialLoader &S)
{
	unsigned int utmp;

	// EventHandlerList handlers
	S.IO(utmp);
	handlers.clear();
	handlers.resize(size_t(utmp));
	for (EventHandlerList::iterator it=handlers.begin(); it!=handlers.end(); ++it) 
	{
		it->Load(S);
	}

	// EventClassMap classes
	S.IO(utmp);
	size_t num_classes = size_t(utmp);

	for (size_t i=0; i<num_classes; ++i)
	{
		std::string class_id;
		S.IO(class_id);

		classes.insert(std::make_pair(class_id, EventClass()));
		S.IO(classes[class_id].suspended);
		classes[class_id].thread = CLValue::load(S);
	}
}

////////////////////////////////////////////////////////////////
// CL2 GARBAGE COLLECTION                                     //
////////////////////////////////////////////////////////////////

void EventManager::MarkObjects()
{
	EventHandlerList::iterator it = handlers.begin(), end = handlers.end();
	while (it != end)
	{
		it->MarkObjects(); 
		++it;
	}

	EventClassMap::iterator ec_it = classes.begin(), ec_end = classes.end();
	for (;ec_it!=ec_end;++ec_it)
	{
		ec_it->second.thread.markObject();
	}
}


