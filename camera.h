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

#ifndef CAMERA_H
#define CAMERA_H

#include "cl2/cl2.h"

class Camera {
public:
    // CONSTRUCTOR/DESCTRUCTOR //////////////////////////////////////////////
    Camera();
    ~Camera();

    void Clear();

    // SET CAMERA-MODE/-PARAMETERS //////////////////////////////////////////
    void SetScrollMode(); // enable scroll mode
    void SetSnapMode(); // enable snap mode
    void SetScrollSpeed(int spd, int accel = 1) {
        scroll_speed = spd;
        scroll_accel = accel;
    }

    // SET CAMERA TARGET ////////////////////////////////////////////////////
    void SetTarget(CLValue room, int x, int y); // set full position
    void SetTarget(int x, int y); // set position within same room
    void SetTarget(CLValue item); // seek item

    // UPDATE, DRAW CAMERA CONTENTS /////////////////////////////////////////
    void Update(float dt);

    void Draw();

    // GET CAMERA INFO //////////////////////////////////////////////////////
    CLValue GetRoom() { return cur_room; }

    int GetOffsetX() { return static_cast<int>(cur_x); }
    int GetOffsetY() { return static_cast<int>(cur_y); }

    void GetCameraWindow(int &x, int &y, int &width, int &height);
    void SetCameraWindow(int x, int y, int width, int height);

    bool ScreenToRoomCoordinates(int &X, int &Y); // returns true, iff result is currently visible (inside camera window)
    void RoomToScreenCoordinates(int &X, int &Y);

    // GARBAGE COLLECTING ///////////////////////////////////////////////////
    void markObjects();

    // SAVE & LOAD STATE ////////////////////////////////////////////////////
    void Save(CLSerialSaver &S);
    void Load(CLSerialLoader &S);

    // PRIVATE MEMBERS //////////////////////////////////////////////////////
private:
    enum CameraMode {
        MANUAL,
        SEEK, // seek an item/actor
    };

    CameraMode camera_mode;

    enum ScrollMode {
        SNAP,
        SCROLL,
    };
    ScrollMode scroll_mode;

    // current camera coordinates
    CLValue cur_room; // current room
    float cur_x, cur_y; // current camera position within room

    // current camera destination
    CLValue dest_room;
    int dest_x, dest_y; // camera movement target

    CLValue seeked; // item/actor to seek, if camera_mode == SEEK

    // scroll speed and acceleration, for scroll_mode == SCROLL
    int scroll_speed; // pixels/second
    int scroll_accel; // additional speed factor per 1000 pixel of dest_xy-->cur_xy difference

    // camera area on screen
    struct CameraWindow {
        int x, y, width, height;
    } window;
};

#endif

