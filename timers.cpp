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

#include "timers.h"

#include "object/timer.h"

#include <iostream>
using namespace std;

////////////////////////////////////////////////////////////////
// CONSTRUCTION                                               //
////////////////////////////////////////////////////////////////
TimerManager::TimerManager()
{
	Clear();
}

TimerManager::~TimerManager()
{
}

void TimerManager::Clear()
{
	timers.clear();
}

void TimerManager::Update(float dt)
{
	std::list<CLValue>::iterator it = timers.begin(), end = timers.end();
	for (;it!=end;++it)
	{
		Timer *T = GET_TIMER(*it);
		T->Update(dt);
	}
}

////////////////////////////////////////////////////////////////
// ADD/REMOVE TIMERS                                          //
////////////////////////////////////////////////////////////////
void TimerManager::AddTimer(CLValue timer)
{
	timers.push_back(timer);
}

void TimerManager::DelTimer(CLValue timer)
{
	Timer *T = GET_TIMER(timer);

	std::list<CLValue>::iterator it = timers.begin(), end = timers.end();
	for (;it!=end;++it)
	{
		if (GET_TIMER(*it) == T)
		{
			it = timers.erase(it);
		}
	}
}

////////////////////////////////////////////////////////////////
// SAVE & LOAD                                                //
////////////////////////////////////////////////////////////////
void TimerManager::Save(CLSerializer &S)
{
}

void TimerManager::Load(CLSerializer &S)
{
}

////////////////////////////////////////////////////////////////
// GARBAGE COLLECTION                                         //
////////////////////////////////////////////////////////////////
void TimerManager::MarkObjects()
{
	std::list<CLValue>::iterator it = timers.begin(), end = timers.end();
	for (;it!=end;++it)
	{
		//if (hat ein callback) it->markObject();
	}
}


