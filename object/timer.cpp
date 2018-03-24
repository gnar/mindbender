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

#include "object/timer.h"
#include "game.h"

using namespace std;

//////////////////////////////////////////////////////////////////
// CONSTRUCTION/DESTRUCTION                                     //
//////////////////////////////////////////////////////////////////
Timer::Timer(CLContext *context)
        : TableObject(context),
          time(0) {
    Game().GetTimerManager().AddTimer(CLValue(this));
}

Timer::~Timer() {
    Game().GetTimerManager().DelTimer(CLValue(this));
}

void Timer::Update(float dt) {
    time += dt;
}

//////////////////////////////////////////////////////////////////
// CLObject                                                     //
//////////////////////////////////////////////////////////////////

void Timer::markReferenced() // GC
{
    TableObject::markReferenced();
}

void Timer::set(CLValue &key, CLValue &val) {
    if (key.type == CL_STRING) {
        const std::string k = GET_STRING(key)->get();
        if (k == "time") {
            this->time = val.toFloat();
            return;
        }
    }

    TableObject::set(key, val);
}

bool Timer::get(CLValue &key, CLValue &val) {
    if (key.type == CL_STRING) {
        const std::string k = GET_STRING(key)->get();
        if (k == "time") {
            val = CLValue(int(this->time));
            return true;
        }
    }

    return TableObject::get(key, val);
}

//////////////////////////////////////////////////////////////////
// SAVE & LOAD STATE                                            //
//////////////////////////////////////////////////////////////////

void Timer::Save(CLSerialSaver &S, Timer *timer) {
    // save time
    S.IO(timer->time);

    // save tableobject data
    TableObject::Save(S, timer);
}

Timer *Timer::Load(CLSerialLoader &S) {
    auto *timer = new Timer(S.getContext());
    S.addPtr(timer);

    // load time
    S.IO(timer->time);

    // load tableobject data
    TableObject::Load(S, timer);

    return timer;
}


