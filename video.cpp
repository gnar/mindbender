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

#include "video.h"

#include <SDL/SDL.h>
#include <dcdraw/dcdraw.h>

bool InitVideo(int width, int height, bool fullscreen, bool show_info) {
    int bpp = 0;
    int zbuffer = 0;
    float gamma = 0.0f;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // See if we should detect the display depth
    if (bpp == 0) {
        bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
    }

    // Set the flags we want to use for setting the video mode
    Uint32 video_flags = 0;
    video_flags |= SDL_OPENGL;
    if (fullscreen) video_flags |= SDL_FULLSCREEN;

    // Initialize the display
    int rgb_size[3] = {0};
    switch (bpp) {
        case 8:
            rgb_size[0] = 2;
            rgb_size[1] = 3;
            rgb_size[2] = 3;
            break;
        case 15:
        case 16:
            rgb_size[0] = 5;
            rgb_size[1] = 5;
            rgb_size[2] = 5;
            break;
        default:
            rgb_size[0] = 8;
            rgb_size[1] = 8;
            rgb_size[2] = 8;
            break;
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, rgb_size[0]);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, rgb_size[1]);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, rgb_size[2]);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, zbuffer); // We don't need a depth buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    if (SDL_SetVideoMode(width, height, bpp, video_flags) == nullptr) {
        fprintf(stderr, "Couldn't set GL mode: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    if (show_info) {
        int value;
        printf("OpenGL info:\n");
        printf("  Vendor ......... : %s\n", glGetString(GL_VENDOR));
        printf("  Renderer ....... : %s\n", glGetString(GL_RENDERER));
        printf("  Version ........ : %s\n", glGetString(GL_VERSION));
        printf("  Screen BPP ..... : %2d (requested %2d)\n", SDL_GetVideoSurface()->format->BitsPerPixel, bpp);
        SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
        printf("  Red bits ....... : %2d (requested %2d)\n", value, rgb_size[0]);
        SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &value);
        printf("  Green bits ..... : %2d (requested %2d)\n", value, rgb_size[1]);
        SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &value);
        printf("  Blue bits ...... : %2d (requested %2d)\n", value, rgb_size[2]);
        SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
        printf("  Depth bits ..... : %2d (requested %2d)\n", value, zbuffer);
        SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value);
        printf("  Doublebuffer ... : %s (requested %2s)\n", value ? "yes" : "no", "yes");
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
        printf("  Max texture size : %i\n", value);
        printf("  Extensions ..... : %s\n", glGetString(GL_EXTENSIONS));
    }

    /* Set the window manager title bar */
    SDL_WM_SetCaption("SDL GL test", "testgl");

    /* Set the gamma for the window */
    if (gamma != 0.0) SDL_SetGamma(gamma, gamma, gamma);

    SDL_EnableUNICODE(1);

    DCDraw::InitGraphics(width, height);

    return true;
}

void DoneVideo() {
    DCDraw::DoneGraphics();
    SDL_Quit();
}

void BeginFrame() {
    DCDraw::BeginFrame();
}

void EndFrame() {
    DCDraw::EndFrame();
    SDL_GL_SwapBuffers();
}
