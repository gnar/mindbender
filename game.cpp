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

#include "game.h"
#include "configuration.h"
#include "main.h"

#include "scene/gameloader.h"

using namespace std;

////////////////////////////////////////////////////////////
// CONSTRUCTOR/DESTRUCTOR                                 //
////////////////////////////////////////////////////////////

Game_::Game_() : loaded(false) {
    context.addModule(&math_module);
    context.addModule(&sushi_module);
}

Game_::~Game_() = default;

Game_ &Game() // singleton instance getter
{
    static Game_ game;
    return game;
}

void Game_::ShutDown() {
    Stop();
}

////////////////////////////////////////////////////////////
// GAME STATE CONTROL                                     //
////////////////////////////////////////////////////////////

void Game_::Start() // (Re)Start a new game
{
    if (loaded) Stop();

    // game up & running
    last_time = 0;
    frame_count = 0;

    // initialize event manager, camera
    event_manager.Clear();
    timer_manager.Clear();
    camera.Clear();

    // load & build scenes
    cout << "Init game" << this << endl;

    const std::string &scene_file = GetConfiguration().GetSceneFile();
    if (!scene_file.empty()) {
        GameLoader gameloader(&context, scene_file);
        gameloader.Parse();
        gameloader.RunPrologScripts();
        gameloader.ParseScenes();
        gameloader.RunSceneScripts();
        gameloader.RunStartScripts();
    }
    cout << "done init game" << endl;

    loaded = true;
}

void Game_::Stop() // Kill current game
{
    if (!loaded) return;

    event_manager.Clear();
    timer_manager.Clear();
    camera.Clear();
    context.clear(); // clear script vm

    loaded = false;
    cout << "done stop game" << endl;
}

////////////////////////////////////////////////////////////
// FRAME COUNTER, FPS, TIMER                              //
////////////////////////////////////////////////////////////
long Game_::GetTime() // get time in milliseconds
{
    return SDL_GetTicks();
}

unsigned long Game_::GetFrameCount() // return number of frames since game start
{
    return frame_count;
}

////////////////////////////////////////////////////////////
// MAINLOOP                                               //
////////////////////////////////////////////////////////////

void Game_::Mainloop() // Update & Draw
{
    if (!loaded) return;

    unsigned long current_time;

    current_time = GetTime();
    unsigned long diff = current_time - last_time; // get time difference between calls
    if (last_time == 0) diff = 10; // first call? assume 100 fps
    this->last_time = current_time;

    // count frames
    ++frame_count;

    // update timer objects
    timer_manager.Update(diff);

    // update camera & current room
    camera.Update(diff);

    // draw camera
    camera.Draw();

    // keep scripts alive
    context.roundRobin();

    // fire user-draw event
    std::vector<CLValue> args;
    CLValue thr = event_manager.Signal(&context, "draw", args);
    if (!thr.isNull()) GET_THREAD(thr)->run();

    // update event manager
    event_manager.Update(diff);

    // collect garbage every 100 frames
    static int C = 0;
    ++C;
    if (C >= 100) {
        GarbageCollect();
        C = 0;
    }

    // Yield if >100fps
    if (diff < 10) {
        SDL_Delay(10 - diff);
    }
}

////////////////////////////////////////////////////////////
// UI-EVENTS HANDLING                                     //
////////////////////////////////////////////////////////////

void Game_::OnButtonDown(const std::string &key_name, int key_ascii) {
    if (!loaded) return;

    char key_ascii_str[2] = {0, 0};
    key_ascii_str[0] = (char) key_ascii;

    std::vector<CLValue> args;
    args.push_back(CLValue(new CLString(&context, key_name)));
    args.push_back(CLValue(new CLString(&context, key_ascii_str)));
    CLValue thread = event_manager.Signal(&context, "keydown", args);
    GET_THREAD(thread)->run();
}

void Game_::OnMouseDown(int x, int y, int btn) {
    if (!loaded) return;

    std::vector<CLValue> args;
    args.emplace_back(x);
    args.emplace_back(y);

    // check if mouse is over a room (camera)
    int room_x = x, room_y = y;
    if (camera.ScreenToRoomCoordinates(room_x, room_y)) {
        args.emplace_back(room_x); // .. if so, add room coordinates to arguments
        args.emplace_back(room_y);
    } else {
        args.emplace_back(CLValue(-1));
        args.emplace_back(CLValue(-1));
    }
    args.emplace_back(btn);

    CLValue thread = event_manager.Signal(&context, "mousedown", args);
    GET_THREAD(thread)->run();
}

void Game_::OnMouseMove(int x, int y) {
    if (!loaded) return;

    std::vector<CLValue> args;
    args.emplace_back(x);
    args.emplace_back(y);

    // check if mouse is over a room (camera) ..
    int room_x = x, room_y = y;
    if (camera.ScreenToRoomCoordinates(room_x, room_y)) {
        args.emplace_back(room_x); // .. if so, add room coordinates to arguments
        args.emplace_back(room_y);
    } else {
        args.emplace_back(CLValue(-1));
        args.emplace_back(CLValue(-1));
    }

    CLValue thread = event_manager.Signal(&context, "mousemove", args);
    GET_THREAD(thread)->run();
}

////////////////////////////////////////////////////////////
// SCRIPTING HELPER FUNCTIONS                             //
////////////////////////////////////////////////////////////

CLValue Game_::ExecuteScript(const string &file) // execute script by name, without threading
{
    CLValue thread(new CLThread(&context));
    GET_THREAD(thread)->init(CLCompiler::compile(&context, file));
    GET_THREAD(thread)->enableYield(false);
    GET_THREAD(thread)->run();

    return GET_THREAD(thread)->getResult();
}

void Game_::GarbageCollect() // perform garbage collection
{
    if (!loaded) return;

    //cout << "MARK----------" << endl;
    //cout << "Objects: " << ocount << endl;

    context.unmarkObjects();
    context.markObjects();

    // mark own CLValues here..
    event_manager.MarkObjects();
    timer_manager.MarkObjects();
    camera.markObjects();
    // marking done..

    //cout << "DONE--------" << endl;

    context.sweepObjects();
    context.finalizeObjects();
}

