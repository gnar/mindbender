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

#ifndef ROOM_H
#define ROOM_H

#include "cl2/cl2.h"

#include <string>
#include <list>

#include "object/tableobject.h"

#define GET_ROOM(v)           ((Room*)(v).value.object)

const size_t NUM_LAYERS = 8;

class RoomObject;

class Room : public TableObject {
public:
    enum PlaneID {
        BACKGROUND_PLANE = 0,
        MAIN_PLANE = 1,
        FOREGROUND_PLANE = 2,
        NUM_PLANES = 3
    };

    // CONSTRUCTOR/DESTRUCTOR //////////////////////////////////////
    Room(CLContext *context, int width, int height);
    ~Room() override;

    // DRAW, UPDATE ROOM ///////////////////////////////////////////
    void Draw(PlaneID p = MAIN_PLANE);
    void DrawActorTexts(int p);
    void DebugDraw();
    void Update(float dt);

    // called by roomobjects in this room when their position has changed
    void ChangedPosition();

    // GET/SET WIDTH/HEIGHT OF ROOM ////////////////////////////////
    int GetWidth(int p = 1) { return planes[p].width; }
    int GetHeight(int p = 1) { return planes[p].height; }

    void SetSize(int width, int height, int p = 1) {
        planes[p].width = width;
        planes[p].height = height;
    }

    // GET/SET BACKGROUND SPRITE OF ROOM ///////////////////////////
    CLValue GetBackground(int p = 1) { return planes[p].background; }
    void SetBackground(CLValue &sprite, int p = 1) { planes[p].background = sprite; }

    // GET/SET WALKING PATH OF ROOM ////////////////////////////////
    void SetPath(CLValue &shape) { path = shape; }
    CLValue GetPath() { return path; }

    // ADD/REMOVE/FIND OBJECTS INTO ROOM ///////////////////////////
    void AddObject(RoomObject *obj, int p = 1);
    void RemoveObject(RoomObject *obj);
    RoomObject *GetObjectAt(int x, int y);

    // ACTOR SCALING /////////////////////////////////////////////////
    void EnableScaling() { scale_enabled = true; }
    void DisableScaling() { scale_enabled = false; }

    void SetConstantScaling(float s) {
        scale_mode = SCALE_CONSTANT;
        scale_constant = s;
    }

    void SetPerspectiveScaling(int horizon, int base) {
        scale_mode = SCALE_PERSPECTIVE;
        scale_y_horizon = horizon;
        scale_y_base = base;
    }

    float GetScalingAt(int x, int y, float prev_scale);

    int GetYXRatio() { return yx_ratio; }

    void SetYXRatio(int tilt) { yx_ratio = tilt; }

    // SAVE & LOAD STATE /////////////////////////////////////////////
    static void Save(CLSerialSaver &S, Room *room);

    static Room *Load(CLSerialLoader &S);

    // CLObject //////////////////////////////////////////////////////
    void markReferenced() override; // GC
    void set(CLValue &key, CLValue &val) override;
    bool get(CLValue &key, CLValue &val) override; // returns true if key existed

    // PRIVATE MEMBER VARIALBES //////////////////////////////////////
private:
    bool need_resort; // true if objects in this room should be resorted
    int width, height; // width/height of room
    int yx_ratio; // y-speed/x-speed ratio of actors in this room in percent

    struct Plane // parallaxed background (0), main plane (1) and foreground (2) layers
    {
        CLValue background; // the background image
        int width, height; // plane size
        std::list<CLValue> objects[NUM_LAYERS];
    } planes[NUM_PLANES];

    CLValue path; // path of room (in main plane)

    // Scaling for actors
    enum ScaleMode {
        SCALE_CONSTANT,    // Keep scaling constant
        SCALE_PERSPECTIVE, // Perspective scaling
    } scale_mode;

    bool scale_enabled;
    int scale_y_horizon, scale_y_base; // parameters for SCALE_PERSPECTIVE mode
    float scale_constant; // parameter for SCALE_CONSTANT mode

    // methods
    CLValue method_objectat;
    CLValue method_addobject;
    CLValue method_remobject;

    CLValue method_set_constant_scaling;      // SetConstantScaling(100)
    CLValue method_set_perspective_scaling;   // SetPerspectiveScaling(y_horizon, y_base)
    CLValue method_enable_scaling;            // EnableScaling()
    CLValue method_disable_scaling;           // DisableScaling()
};

#endif

