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

#include <unistd.h>
#include <cstdlib>

#include <stdexcept>
#include <iostream>

#include "main.h"
#include "game.h"
#include "configuration.h"

#include "video.h"
#include "sound.h"

#include <GL/gl.h>
#include <physfs.h>
#include "dcdraw/dcdraw.h"

#include "resource/manager.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

static Configuration conf;

Configuration &GetConfiguration() {
    return conf;
}

Main_ &Main() // singleton instance getter
{
    static Main_ main;
    return main;
}

bool Main_::HandleEvent(SDL_Event &event) {
    switch (event.type) {
        /*case SDL_ACTIVEEVENT:
            printf( "app %s ", event->active.gain ? "gained" : "lost" );
            if ( event->active.state & SDL_APPACTIVE ) {
            printf( "active " );
            } else if ( event->active.state & SDL_APPMOUSEFOCUS ) {
            printf( "mouse " );
            } else if ( event->active.state & SDL_APPINPUTFOCUS ) {
            printf( "input " );
            }
            printf( "focus\n" );
            break;*/

        case SDL_MOUSEMOTION:
            Game().OnMouseMove(event.motion.x, event.motion.y);
            break;

        case SDL_MOUSEBUTTONDOWN: {
            int btn = -1;
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    btn = 0;
                    break;
                case SDL_BUTTON_MIDDLE:
                    btn = 1;
                    break;
                case SDL_BUTTON_RIGHT:
                    btn = 2;
                    break;
                default:
                    break;
            }
            if (btn != -1) Game().OnMouseDown(event.button.x, event.button.y, btn);
            break;
        }

        case SDL_KEYDOWN: {
            SDL_keysym &sym = event.key.keysym;
            char ascii = 0;
            if ((sym.unicode & 0xFF80) == 0) {
                ascii = sym.unicode & 0x7F;
            } else {
                //cout << "International character" << endl;
            }

            // Hotkeys..
            switch (sym.sym) {
                case SDLK_F10:
                    do_exit = true;
                    break;

                case SDLK_TAB:
                    break;

                case SDLK_F5:
                    Game().Save("quicksave");
                    break;

                case SDLK_F6:
                    Game().Load("quicksave");
                    break;

                default:
                    Game().OnButtonDown(SDL_GetKeyName(sym.sym), ascii);
                    break;
            }

            break;
        }

        case SDL_QUIT:
            do_exit = true;
            break;

        default:
            break;
    }

    return true;
}

int Main_::run(int argc, char **argv) {
    cout << "MindBender v0 - Copyright (C) 2006 by Gunnar Selke" << endl;
    cout << "This is free software, and you are welcome to redistribute it under certain conditions." << endl;
    cout << "MindBender comes with ABSOLUTELY NO WARRANTY. See GPL.txt (included in this distribution) for details."
         << endl;
    cout << endl;

    // Parse command line
    bool cmd_show_info = false;
    bool cmd_show_cmdline = false;
    bool cmd_disable_gc = false;
    bool cmd_disable_extensions = false;
    int cmd_max_tex_size = 0;
    std::string cmd_game_file = "game.xml";
    for (;;) {
        int cmd = getopt(argc, argv, "ihc:xt:y");
        if (cmd == -1) break;

        switch (cmd) {
            case 'c':
                cmd_game_file = optarg;
                break;
            case 'i':
                cmd_show_info = true;
                break;
            case 'x':
                cmd_disable_extensions = true;
                break;
            case 'y':
                cmd_disable_gc = true;
                break;
            case 't':
                cmd_max_tex_size = std::atoi(optarg);
                break;
            case ':':
            case 'h':
            case '?':
                cmd_show_cmdline = true;
                break;
        }
    }

    if (cmd_show_cmdline) {
        cout << "Command line:" << endl;
        cout << "  -c <file> Set config file" << endl;
        cout << "  -i        Show video information" << endl;
        cout << "  -h        Show this help" << endl;
        cout << endl;
        cout << "Advanced options:" << endl;
        cout << "  -t <size> Set maximum texture width/height (and don't autodetect)" << endl;
        cout << "  -x        Disable use of OpenGL extensions" << endl;
        cout << "  -y        Disable garbage collector" << endl;
        return 0;
    }

    if (cmd_max_tex_size != 0) {
        //...
    }

    if (cmd_disable_extensions) {
        //...
    }

    if (cmd_disable_gc) {
        //...
    }

    bool error = false;

    try {
        // Load game configuration file
        GetConfiguration().Parse(cmd_game_file);
        if (GetConfiguration().Error()) {
            cerr << "Configuration error: " << GetConfiguration().ErrorString() << endl;
#ifdef _WIN32
            MessageBox(0, "Configuration error (see stderr.txt), press ok to exit", "Exit", 0);
#endif
            return -2;
        }

        // Initialize video & periphals
        bool ok = InitVideo(GetConfiguration().GetVideoWidth(),
                            GetConfiguration().GetVideoHeight(),
                            GetConfiguration().GetFullscreen(), cmd_show_info);
        SDL_WM_SetCaption(GetConfiguration().GetTitle().c_str(), nullptr);

        // Initialize physfs
        PHYSFS_init(argv[0]);
        const std::vector<std::string> &sources = GetConfiguration().GetSources();
        for (size_t i = 0; i < sources.size(); ++i)
            Res::Manager.AddSource(sources[i]);

        // Init sound
        InitSound();

        // Start game
        Game().Start();

        // Run the main loop
        while (!do_exit) {
            BeginFrame();

            if (Game().IsLoaded()) {
                Game().Mainloop();
            } else {
            }

            // Check for error conditions
            GLenum gl_error = glGetError();
            if (gl_error != GL_NO_ERROR) {
                fprintf(stderr, "OpenGL error: %d\n", gl_error);
            }

            char *sdl_error = SDL_GetError();
            if (sdl_error[0] != '\0') {
                fprintf(stderr, "SDL error '%s'\n", sdl_error);
                SDL_ClearError();
            }

            EndFrame();

            // Check for event
            SDL_Event event{};
            while (SDL_PollEvent(&event)) {
                HandleEvent(event);
            }
        }

        // Stop & Delete game
        Game().Stop();
        Game().ShutDown();
        Res::Manager.ShutDown();

        // Shut down sound
        //	DoneSound();

        // Shut down physfs
        PHYSFS_deinit();

        // Shut down video
        DoneVideo();

    } catch (CLParserException &err) {
        std::cerr << err.what() << std::endl;
        error = true;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        error = true;
    }

#ifdef _WIN32
    if (error) MessageBox(0, "Errors occured, press ok to exit", "Exit", 0);
#endif

    return error ? -1 : 0;
}

int main(int argc, char **args) {
    return Main().run(argc, args);
}

