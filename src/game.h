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

#ifndef GAME_H
#define GAME_H

#include <string>

#include "events.h"
#include "timers.h"
#include "camera.h"

#include "sushimodule.h"

#include "cl2/cl2.h"

class Game_ &Game(); // singleton instance getter

class Game_ {
public:
    // CONSTRUCTOR/DESTRUCTOR //////////////////////////////////
    Game_();
    ~Game_();

    void ShutDown(); // must be called before program quits and ClanLib is deinitialized

    // GLOBAL GAME STATE CONTROL ///////////////////////////////
    bool IsLoaded() { return loaded; }

    void Start(); // State a new game
    void Stop(); // Kill game

    // SAVE & LOAD STATES FROM DISK ////////////////////////////
    // (implemented in saveload.cpp)
    void Save(const std::string &filename);

    void Load(const std::string &filename);

    // FRAME COUNTER, FPS, TIMER ///////////////////////////////
    long GetTime(); // get time in milliseconds
    unsigned long GetFrameCount(); // return number of frames since game start

    // UPDATE/DRAW FRAMES //////////////////////////////////////
    void Mainloop(); // Update & Draw

    // LISTENER FOR UI-EVENTS //////////////////////////////////
    void OnButtonDown(const std::string &key_name, int key_ascii);
    void OnMouseDown(int x, int y, int btn);
    void OnMouseMove(int x, int y);

    // SCRIPTING HELPER FUNCTIONS //////////////////////////////
    CLValue ExecuteScript(const std::string &file); // execute script by name, without threading
    void GarbageCollect(); // perform garbage collection

    // EVENT MANAGER ///////////////////////////////////////////
    EventManager &GetEventManager() { return event_manager; }

    // TIMER MANAGER ///////////////////////////////////////////
    TimerManager &GetTimerManager() { return timer_manager; }

    // CAMERA //////////////////////////////////////////////////
    Camera &GetCamera() { return camera; }

    // MEMBER VARIALBES ////////////////////////////////////////
private:
    bool loaded; // is a game active?

    // frame counter, fps
    unsigned long last_time;
    unsigned long frame_count;

    Camera camera;              // camera
    EventManager event_manager; // event manager
    TimerManager timer_manager; // timer manager

    // script engine context
    CLContext context;
    CLMathModule math_module; // math module
    SushiModule sushi_module; // bindings
};

#endif

