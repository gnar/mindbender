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

#ifndef ACTOR_H
#define ACTOR_H

#include "object/roomobject.h"
#include "object/shape.h"

#include <queue>

#define GET_ACTOR(v)           ((Actor*)(v).value.object)

class Actor : public RoomObject {
public:
    // Constructor/Destructor //////////////////////////////////////////
    explicit Actor(CLContext *context);
    ~Actor() override;

    // Update/Draw actor ///////////////////////////////////////////////
    void Update(float dt) override;
    void Draw() override;
    void DebugDraw() override;
    void DrawText();
    void Stop(); // stop all actions

    // Talking interface ///////////////////////////////////////////////
    void Talk(const std::string &text);
    void TalkAppend(const std::string &text);
    bool IsTalking() { return state == TALKING; }

    // Walking interface ///////////////////////////////////////////////
    bool WalkTo(int x, int y);
    bool IsWalking() { return state == WALKING; }

    // Special animation interface /////////////////////////////////////
    void Perform(CLValue sprite);
    bool IsSpecialAction() { return state == SPECIAL; }

    // Get info ////////////////////////////////////////////////////////
    int GetWidth() { return width; }
    int GetHeight() { return height; }

    // SAVE & LOAD STATE //////////////////////////////////////////////
    static void Save(CLSerialSaver &S, Actor *bagitem);
    static Actor *Load(CLSerialLoader &S);

private:
    // ACTOR STATE HANDLING ////////////////////////////////////////////
    enum State {
        STILL,   // actor stands
        WALKING, // actor walks
        TALKING, // actor talks
        //	TURNING, // actor turns
        SPECIAL, // perform special animation, then stand
    };
    State state; // actor state (STILL)

    // main properties
    int dir;             // 0-359, 270=facing player, 90=into screen, 0=left, 180=right, clock-wise on ground
    int width, height;   // dimensions of actor, used for camera positioning and talk-text position
    int speed;           // walk speed in Pixels/second
    float scale;
    bool no_auto_scale;   // disable automatic actor scaling in rooms
    CLValue font;           // font to use for talk-text display
    CLValue path_override;  // override path

    // methods
    CLValue method_walkto;                  // ego.WalkTo(x, y)
    CLValue method_iswalking;        // ego.IsWalking()
    CLValue method_talk;            // ego.Talk(text)
    CLValue method_append_talk;             // ego.AppendTalk(text)
    CLValue method_istalking;        // ego.IsTalking()
    CLValue method_perform;            // ego.Perform(sprite)
    CLValue method_stop;            // ego.Stop()

    // events
    CLValue on_draw;                        // function()
    void CallEventDraw();

    // Talking /////////////////////////////////////////////////////////
    void Talk_Update(float dt);
    void Talk_Stop();
    void Talk_Clear(); // clear text queue
    void Talk_AppendText(const std::string &text); // append text to queue
    void Talk_AppendDelay(float ms); // append delay to queue

    // insert texts/delays using meta-tags
    void Talk_Parse(const std::string &text);

    struct TalkItem {
        TalkItem() = default;
        explicit TalkItem(float delay) : type(DELAY), delay(delay) {}
        explicit TalkItem(const std::string &text) : type(TEXT), text(text) {}
        ~TalkItem() = default;

        enum Type {
            TEXT, DELAY
        } type;
        std::string text;
        float delay{};
    };

    // talk parameters
    int Talk_x, Talk_y;
    std::string Talk_text; // current spoken text
    std::queue<TalkItem> Talk_queue;

    void Walk_Stop();
    void Walk_Update(float dt);

    void Rescale();

    // walk parameters
    int Walk_dest_x, Walk_dest_y;   // current walking destination
    std::list<Vertex> Walk_route;                 // current (remaining) walking route
    Vertex Walk_segm1, Walk_segm2;     // current walking route segment, popped from 'Walk_route' list
    int Walk_segm_len;              // length of current walking segment in pixels
    float Walk_progress;              // length of completed current route segment (in pixels)

    // Special animation ////////////////////////////////////////////////
    void Special_Stop();
    void Special_Update(float dt);

    // Graphics /////////////////////////////////////////////////////////
    CLValue sprite_still;
    CLValue sprite_walking;
    CLValue sprite_talking;
    CLValue sprite_special;

    // select most fitting track for the current direction
    void SelectTrack(CLValue sprite);

    // CLObject ////////////////////////////////////////////////////////
    void markReferenced() override;
    void set(CLValue &key, CLValue &val) override;
    bool get(CLValue &key, CLValue &val) override;

    std::string toString() override;
};

#endif


