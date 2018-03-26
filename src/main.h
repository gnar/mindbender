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

#ifndef MAIN_H
#define MAIN_H

#include <SDL/SDL.h>

class Configuration &GetConfiguration();

class Main_ &Main(); // Singleton getter

class Main_ {
    bool do_exit;

	Main_() : do_exit(false) {}
	friend Main_ &Main();
	
	bool HandleEvent(SDL_Event &event); // returns true if event handled

public:
    int run(int argc, char **argv);

    void Exit() { do_exit = true; }
};

#endif

