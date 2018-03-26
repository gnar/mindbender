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

#include "object/actor.h"
#include "object/room.h"
#include "object/font.h"
#include "object/sprite.h"

#include "game.h"

#include <cmath>

#include <sstream>

using namespace std;

namespace {
    inline int NormalizeDir(int dir) {
        if (dir >= 0) dir %= 360; else while (dir < 0) dir += 360;
        return dir - (dir % 5);
    }

    inline int GetDirFromMovement(float dx, float dy, float xy_ratio = 1.0f) {
        return NormalizeDir(int((180.0f * std::atan2(dy, dx)) / 3.1416));
    }

    inline int Distance(int x1, int y1, int x2, int y2) {
        return int(sqrt(float((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))));
    }

    static const unsigned AUTOPAUSE_PER_CHAR = 80;
    static const unsigned AUTOPAUSE_MIN = 1000;

    static unsigned GetTalkTime(const std::string &str) {
        unsigned n = 0;
        for (size_t i = 0; i < str.size(); ++i) {
            if (!isspace(str[i])) ++n;
        }
        if (n == 0) return 0;

        unsigned time = n * AUTOPAUSE_PER_CHAR;

        if (time > AUTOPAUSE_MIN)
            return time;
        else
            return AUTOPAUSE_MIN;
    }
}

////////////////////////////////////////////////////////////////////
// Constructor/Destructor                                         //
////////////////////////////////////////////////////////////////////
Actor::Actor(CLContext *context)
        : RoomObject(context),
          state(STILL),
          dir(0), width(80), height(150), speed(100), scale(1.0f), no_auto_scale(false),

          method_walkto(new CLExternalFunction(context, "adv_actor_walkto")),
          method_iswalking(new CLExternalFunction(context, "adv_actor_iswalking")),
          method_talk(new CLExternalFunction(context, "adv_actor_talk")),
          method_append_talk(new CLExternalFunction(context, "adv_actor_appendtalk")),
          method_istalking(new CLExternalFunction(context, "adv_actor_istalking")),
          method_perform(new CLExternalFunction(context, "adv_actor_perform")),
          method_stop(new CLExternalFunction(context, "adv_actor_stop")),

        //  font(), //TODO: Initialize with stdfont
          Walk_dest_x(0), Walk_dest_y(0) {
    SetName("<unnamed actor>");
}

Actor::~Actor() {
}

////////////////////////////////////////////////////////////////////
// Update/Draw actor                                              //
////////////////////////////////////////////////////////////////////
void Actor::Update(float dt) {
    switch (state) {
        case STILL:
            break;

        case WALKING:
            Walk_Update(dt);
            break;

        case TALKING:
            Talk_Update(dt);
            break;

        case SPECIAL:
            Special_Update(dt);
            break;
    }
}

void Actor::Draw() {
    if (IsHidden()) return;

    if (!on_draw.isNull()) {
        // handled by user!
        CallEventDraw();
    } else {
        // Draw Actor

        // select sprite
        CLValue sprite;
        switch (state) {
            case STILL:
                sprite = sprite_still;
                break;

            case WALKING:
                sprite = sprite_walking;
                if (sprite.isNull()) sprite = sprite_still; // fall back?
                break;

            case TALKING:
                sprite = sprite_talking;
                if (sprite.isNull()) sprite = sprite_still; // fall back?
                break;

            case SPECIAL:
                sprite = sprite_special;
                break;
        }

        // draw sprite
        if (!sprite.isNull()) {
            SelectTrack(sprite); // select animation
            Sprite *spr = GET_SPRITE(sprite);
            spr->SetScaling(scale);
            spr->Draw(GetPositionX(), GetPositionY());
        }
    }
}

void Actor::DebugDraw() {
    /**if (state == WALKING)
    {
        CL_Display::draw_line(Walk_segm1.x, Walk_segm1.y, Walk_segm2.x, Walk_segm2.y, CL_Color::yellow);
        std::list<Vertex>::iterator it = Walk_route.begin(), end = Walk_route.end();
        if (it != end)
            CL_Display::draw_line(Walk_segm2.x, Walk_segm2.y, it->x, it->y, CL_Color::yellow);
        for (;it!=end;) {
            Vertex &V1 = *it;
            ++it; if (it == end) break;
            Vertex &V2 = *it;
            CL_Display::draw_line(V1.x, V1.y, V2.x, V2.y, CL_Color::yellow);
        }
    }**/
}

#ifdef _WIN32
#undef DrawText
#endif

void Actor::DrawText() {
    // TODO: Implement user-OnDrawText

    // Draw actor talk text, if any
    if (state == TALKING && !font.isNull()) {
        int x = Talk_x, y = Talk_y;

        // Map room position to screen position
        //int scr_x = x, scr_y = y;
        //Game().GetCamera().RoomToScreenCoordinates(x, y);

        // Fit into camera window
        //const int SPACE_HORI = 150; // TODO: Make them configurable somehow
        //const int SPACE_VERT = 20;
        const int TEXT_WIDTH = 300;

        //int win_x, win_y, win_w, win_h;
        //Game().GetCamera().GetCameraWindow(win_x, win_y, win_w, win_h);

        GET_FONT(font)->PrintBoxed(x, y, TEXT_WIDTH, Talk_text);
    }
}

void Actor::Stop() // stop all actions
{
    switch (state) {
        case STILL:
            break; //ok
        case WALKING:
            Walk_Stop();
            break;
        case TALKING:
            Talk_Stop();
            break;
        case SPECIAL:
            Special_Stop();
            break;
    }

    state = STILL;
}

// select most fitting track of 'sprite' for the current direction
void Actor::SelectTrack(CLValue sprite) {
    // TODO: Handle missing tracks
    // 270=facing player, 90=into screen, 0=left, 180=right, clock-wise on ground
    if ((dir < 45) || (dir >= 315)) GET_SPRITE(sprite)->SetTrack("w");
    if ((dir >= 45) && (dir < 135)) GET_SPRITE(sprite)->SetTrack("n");
    if ((dir >= 135) && (dir < 225)) GET_SPRITE(sprite)->SetTrack("e");
    if ((dir >= 225) && (dir < 315)) GET_SPRITE(sprite)->SetTrack("s");
}

////////////////////////////////////////////////////////////////////
// Talking                                                        //
////////////////////////////////////////////////////////////////////

// public interface
void Actor::Talk(const std::string &text) {
    if (state != TALKING) {
        Stop();
        state = TALKING;
    }

    Talk_Clear();
    Talk_Parse(text);
}

// public interface
void Actor::TalkAppend(const std::string &text) {
    if (state != TALKING) {
        Talk(text); // need to start talking?
    } else {
        Talk_Parse(text);
    }
}

void Actor::Talk_Update(float dt) {
    while (dt > 0.0f && (!Talk_queue.empty())) {
        TalkItem &T = Talk_queue.front();
        switch (T.type) {
            case TalkItem::TEXT:
                // determine position where text should be displayed
                Talk_x = GetPositionX();
                Talk_y = GetPositionY() - int(height * scale);
                Talk_text = T.text;
                Talk_queue.pop();
                break;

            case TalkItem::DELAY:
                if (dt < T.delay) {
                    T.delay -= dt;
                    dt = 0.0f;
                } else { // delay done?
                    dt = dt - T.delay; // subtract rest time
                    Talk_queue.pop();
                }
                break;
        }
    }

    if (Talk_queue.empty()) {
        Talk_Stop();
    }
}

void Actor::Talk_Stop() {
    Talk_Clear();
    Talk_text = "";
    state = STILL;
}

void Actor::Talk_Parse(const std::string &text) {
    // <p> delay based on character count
    // <p nnn> delay for n ms

    const char *cstr = text.c_str();
    std::string line;

    for (size_t i = 0; i < text.size(); ++i) {
        char ch = cstr[i];
        if (ch != '<') {
            line += ch;
        } else {
            // read token within '<' and '>'
            std::string token;
            ++i;
            if (i >= text.size()) goto done;
            ch = cstr[i];
            while (ch != '>') {
                token += ch;
                ++i;
                if (i >= text.size()) goto done;
                ch = cstr[i];
            }

            // parse token
            if (token == "p") {
                if (!line.empty()) {
                    Talk_AppendText(line);
                    Talk_AppendDelay(GetTalkTime(line));
                    line = "";
                }
            }
        }
    }

    done:
    if (!line.empty()) {
        Talk_AppendText(line);
        Talk_AppendDelay(GetTalkTime(line));
    }

}

void Actor::Talk_Clear() // clear text queue
{
    Talk_queue = std::queue<TalkItem>();
}

void Actor::Talk_AppendText(const std::string &text) // append text to queue
{
    Talk_queue.push(TalkItem(text));
}

void Actor::Talk_AppendDelay(float ms) // append delay to queue
{
    Talk_queue.push(TalkItem(ms));
}

////////////////////////////////////////////////////////////////////
// Walking                                                        //
////////////////////////////////////////////////////////////////////
void Actor::Rescale() {
    if (!no_auto_scale) {
        CLValue room = GetRoom();
        if (!room.isNull()) {
            this->scale = GET_ROOM(room)->GetScalingAt(GetPositionX(), GetPositionY(), scale);
        }
    }
}

bool Actor::WalkTo(int to_x, int to_y) {
    Stop();

    // error handling
    if (GetRoom().isNull()) {
        clog << "<actor>.WalkTo: No room present." << endl;
        return false;
    }

    Shape *path = GET_SHAPE(GET_ROOM(GetRoom())->GetPath());

    {    // Override room path?
        Shape *path2 = !path_override.isNull() ? GET_SHAPE(path_override) : nullptr;
        if (path2) path = path2;
    }

    if (!path) {
        clog << "<actor>.WalkTo: No path set to use!" << endl;
        return false;
    }

    { // HACKS
        int x = GetPositionX(), y = GetPositionY();

        if (Distance(to_x, to_y, x, y) <= 2) {
            //SetPositionX(to_x); SetPositionY(to_y);
            return true;
        }

        if (!path->Hit(x, y)) {
            if (path->Hit(x + 1, y)) x += 1;
            else if (path->Hit(x - 1, y)) x -= 1;
            else if (path->Hit(x, y + 1)) y += 1;
            else if (path->Hit(x, y - 1)) y -= 1;
            else if (path->Hit(x - 1, y - 1)) {
                x -= 1;
                y -= 1;
            }
            else if (path->Hit(x + 1, y - 1)) {
                x += 1;
                y -= 1;
            }
            else if (path->Hit(x - 1, y + 1)) {
                x -= 1;
                y += 1;
            }
            else if (path->Hit(x + 1, y + 1)) {
                x += 1;
                y += 1;
            }
            SetPositionX(x);
            SetPositionY(y);
        }
    } // HACKS END

    // already there?
    if (to_x == GetPositionX() && to_y == GetPositionY()) return true;

    Walk_route = path->FindPath(Vertex(GetPositionX(), GetPositionY()), Vertex(to_x, to_y));
    if (Walk_route.empty()) {
        clog << "Kein Weg gefunden: (" << GetPositionX() << ","
             << GetPositionY() << ")-->(" << to_x << "," << to_y << ")" << endl;
        return false;
    }

    state = WALKING;
    Walk_dest_x = to_x;
    Walk_dest_y = to_y;
    Walk_segm1 = *(Walk_route.begin());
    Walk_route.pop_front();
    Walk_segm2 = *(Walk_route.begin());
    Walk_route.pop_front();
    Walk_segm_len = Distance(Walk_segm1.x, Walk_segm1.y, Walk_segm2.x, Walk_segm2.y);
    Walk_progress = 0.0f;

    // new direction
    dir = GetDirFromMovement(Walk_segm1.x - Walk_segm2.x, Walk_segm1.y - Walk_segm2.y);

    return true;
}

void Actor::Walk_Stop() {
    if (state != WALKING) return;

    Walk_route.clear();
    state = STILL;
}

void Actor::Walk_Update(float dt) {
    if (state != WALKING) return;

    Room *room = GET_ROOM(GetRoom());

    // set new position; linear interpolation
    const float f = Walk_progress / Walk_segm_len;
    SetPositionX(static_cast<int>((1.0f - f) * Walk_segm1.x + f * Walk_segm2.x));
    SetPositionY(static_cast<int>((1.0f - f) * Walk_segm1.y + f * Walk_segm2.y));

    // adjust scaling for new position; get yx_ratio from room
    Rescale();
    float yx_ratio = static_cast<float>(room->GetYXRatio()) / 100.0f;

    // calculate walking progress in pixels
    float progress = 0;
    float raw_progress = (dt * speed) / 1000.0f; // raw speed in pixels for this step (with scaling = 1.0)
    if (Walk_segm1.x == Walk_segm2.x) // vertical walk?
    {
        progress = raw_progress * yx_ratio;
    } else {
        float dx = Walk_segm1.x - Walk_segm2.x;
        float dy = Walk_segm1.y - Walk_segm2.y;
        float yx_speed_2 = dy / dx; // y/x speed ratio (= y/x direction ratio)
        yx_speed_2 *= yx_speed_2; // (y/x speed ratio)^2
        float yx_ratio_2 = yx_ratio * yx_ratio; // yx_ratio^2
        progress = raw_progress *
                   std::sqrt((1 + yx_speed_2 * yx_ratio_2) / (1 + yx_speed_2)); // scaled down due to room->yx_ratio
    }

    // advance actor position by 'progress' pixels
    Walk_progress += progress * this->scale;
    if (Walk_progress >= Walk_segm_len) // walk along segment done?
    {
        if (Walk_route.empty()) // destination reached?
        {
            Walk_Stop();
            SetPositionX(Walk_dest_x);
            SetPositionY(Walk_dest_y);
        } else { // else, enter next segment
            Walk_progress -= Walk_segm_len;
            Walk_segm1 = Walk_segm2;
            Walk_segm2 = *(Walk_route.begin());
            Walk_route.pop_front();
            Walk_segm_len = Distance(Walk_segm1.x, Walk_segm1.y, Walk_segm2.x, Walk_segm2.y);

            // update: new direction
            this->dir = GetDirFromMovement(Walk_segm1.x - Walk_segm2.x, Walk_segm1.y - Walk_segm2.y);
        }
    }
}


////////////////////////////////////////////////////////////////////
// Special animations                                             //
////////////////////////////////////////////////////////////////////
void Actor::Perform(CLValue sprite) {
    Stop();

    sprite_special = sprite;
    state = SPECIAL;
    GET_SPRITE(sprite_special)->Rewind();
}

void Actor::Special_Stop() {
    state = STILL;
    sprite_special.setNull();
}

void Actor::Special_Update(float dt) {
    if (GET_SPRITE(sprite_special)->IsFinished()) Special_Stop();
}

////////////////////////////////////////////////////////////////////
// CLObject                                                       //
////////////////////////////////////////////////////////////////////
void Actor::set(CLValue &key, CLValue &val) {
    if (key.type == CL_STRING) {
        const std::string &k = GET_STRING(key)->get();

        // methods (all read-only)
        if (k == "WalkTo") {
            clog << "<actor>.WalkTo is read-only!" << endl;
            return;
        } else if (k == "IsWalking") {
            clog << "<actor>.IsWalking is read-only!" << endl;
            return;
        } else if (k == "IsTalking") {
            clog << "<actor>.IsTalking is read-only!" << endl;
            return;
        } else if (k == "Talk") {
            clog << "<actor>.Talk is read-only!" << endl;
            return;
        } else if (k == "TalkAppend") {
            clog << "<actor>.TalkAppend is read-only!" << endl;
            return;
        } else if (k == "Stop") {
            clog << "<actor>.Stop is read-only!" << endl;
            return;
        } else if (k == "Perform") {
            clog << "<actor>.Perform is read-only!" << endl;
            return;

            // properties
        } else if (k == "x") {
            SetPositionX(val.toInt());
            Rescale();
            return;
        } else if (k == "y") {
            SetPositionY(val.toInt());
            Rescale();
            return;
        } else if (k == "width") {
            this->width = val.toInt();
            return;
        } else if (k == "height") {
            this->height = val.toInt();
            return;
        } else if (k == "scale") {
            this->scale = float(val.toInt()) / 100;
            return;
        } else if (k == "font") {
            this->font = val;
            return;
        } else if (k == "speed") {
            this->speed = val.toInt();
            return;
        } else if (k == "dir") {
            this->dir = val.toInt();
            return;
        } else if (k == "walk_x") { // ro
            clog << "<actor>.walk_x is read-only!" << endl;
            return;
        } else if (k == "walk_y") { // ro
            clog << "<actor>.walk_y is read-only!" << endl;
            return;

        } else if (k == "still_sprite") {
            this->sprite_still = val;
            return;
        } else if (k == "walk_sprite") {
            this->sprite_walking = val;
            return;
        } else if (k == "talk_sprite") {
            this->sprite_talking = val;
            return;

        } else if (k == "path_override") {
            this->path_override = val;
            return;
        } else if (k == "no_auto_scale") {
            this->no_auto_scale = val.toBool();
            return;

            // events
        } else if (k == "OnDraw") {
            this->on_draw = val;
            return;
        }
    }

    RoomObject::set(key, val);
}

bool Actor::get(CLValue &key, CLValue &val) // returns true if key existed
{
    if (key.type == CL_STRING) {
        const std::string &k = GET_STRING(key)->get();

        // methods
        if (k == "WalkTo") {
            val = method_walkto;
            return true;
        } else if (k == "IsWalking") {
            val = method_iswalking;
            return true;
        } else if (k == "IsTalking") {
            val = method_istalking;
            return true;
        } else if (k == "Talk") {
            val = method_talk;
            return true;
        } else if (k == "TalkAppend") {
            val = method_append_talk;
            return true;
        } else if (k == "Stop") {
            val = method_stop;
            return true;
        } else if (k == "Perform") {
            val = method_perform;
            return true;

            // properties
        } else if (k == "x") {
            val = CLValue(GetPositionX());
            return true;
        } else if (k == "y") {
            val = CLValue(GetPositionY());
            return true;
        } else if (k == "width") {
            val = CLValue(width);
            return true;
        } else if (k == "height") {
            val = CLValue(height);
            return true;
        } else if (k == "scale") {
            val = CLValue(int(scale * 100.0f));
            return true;
        } else if (k == "font") {
            val = font;
            return true;
        } else if (k == "speed") {
            val = CLValue(speed);
            return true;
        } else if (k == "dir") {
            val = CLValue(this->dir);
            return true;

        } else if (k == "walk_x") { // ro
            val = CLValue(Walk_dest_x);
            return true;
        } else if (k == "walk_y") { // ro
            val = CLValue(Walk_dest_y);
            return true;

        } else if (k == "still_sprite") {
            val = this->sprite_still;
            return true;
        } else if (k == "walk_sprite") {
            val = this->sprite_walking;
            return true;
        } else if (k == "talk_sprite") {
            val = this->sprite_talking;
            return true;

        } else if (k == "path_override") {
            val = this->path_override;
            return true;
        } else if (k == "no_auto_scale") {
            val = this->no_auto_scale ? CLValue::True() : CLValue::False();
            return true;

            // events
        } else if (k == "OnDraw") {
            val = this->on_draw;
            return true;
        }
    }

    return RoomObject::get(key, val);
}

void Actor::markReferenced() {
    RoomObject::markReferenced();

    // properties
    font.markObject();
    sprite_still.markObject();
    sprite_walking.markObject();
    sprite_talking.markObject();
    sprite_special.markObject();

    // methods
    method_walkto.markObject();
    method_iswalking.markObject();
    method_talk.markObject();
    method_append_talk.markObject();
    method_istalking.markObject();
    method_perform.markObject();
    method_stop.markObject();

    // events
    on_draw.markObject();
}

////////////////////////////////////////////////////////////////////
// Events                                                         //
////////////////////////////////////////////////////////////////////

void Actor::CallEventDraw() {
    auto *T = new CLThread(getContext());
    T->init(on_draw, std::vector<CLValue>(), CLValue(this));
    T->enableYield(false);
    T->run();
}

////////////////////////////////////////////////////////////////////
// SAVE & LOAD STATE                                              //
////////////////////////////////////////////////////////////////////
void Actor::Save(CLSerialSaver &S, Actor *actor) {
    int tmp;

    S.IO(tmp = static_cast<int>(actor->state));

    S.IO(actor->dir);
    S.IO(actor->width);
    S.IO(actor->height);
    S.IO(actor->speed);
    S.IO(actor->scale);
    S.IO(actor->no_auto_scale);
    CLValue::save(S, actor->font);

    {
        S.IO(actor->Talk_x);
        S.IO(actor->Talk_y);
        S.IO(actor->Talk_text);

        S.IO(tmp = static_cast<size_t>(actor->Talk_queue.size()));

        std::queue<TalkItem> tmp_talkqueue;
        while (!actor->Talk_queue.empty()) {
            TalkItem &TI = actor->Talk_queue.front();

            S.IO(tmp = static_cast<int>(TI.type));
            S.IO(TI.text);
            S.IO(TI.delay);

            tmp_talkqueue.push(TI);

            actor->Talk_queue.pop();
        }
        actor->Talk_queue = tmp_talkqueue; // restore detroyed queue..
    }

    {
        S.IO(actor->Walk_dest_x);
        S.IO(actor->Walk_dest_y);

        //std::list<Vertex>  Walk_route
        S.IO(tmp = static_cast<int>(actor->Walk_route.size()));
        auto it = actor->Walk_route.begin(), end = actor->Walk_route.end();
        for (; it != end; ++it) {
            Vertex &V = *it;
            S.IO(V.x);
            S.IO(V.y);
        }

        //Vertex Walk_segm1, Walk_segm2
        S.IO(actor->Walk_segm1.x);
        S.IO(actor->Walk_segm1.y);
        S.IO(actor->Walk_segm2.x);
        S.IO(actor->Walk_segm2.y);

        S.IO(actor->Walk_segm_len);
        S.IO(actor->Walk_progress);
    }

    CLValue::save(S, actor->sprite_still);
    CLValue::save(S, actor->sprite_walking);
    CLValue::save(S, actor->sprite_talking);
    CLValue::save(S, actor->sprite_special);

    CLValue::save(S, actor->path_override);

    RoomObject::Save(S, actor); // save roomobject
    TableObject::Save(S, actor); // save tableobject
}

Actor *Actor::Load(CLSerialLoader &S) {
    int tmp;

    auto *actor = new Actor(S.getContext());
    S.addPtr(actor);

    S.IO(tmp);
    actor->state = static_cast<State>(tmp);

    S.IO(actor->dir);
    S.IO(actor->width);
    S.IO(actor->height);
    S.IO(actor->speed);
    S.IO(actor->scale);
    S.IO(actor->no_auto_scale);
    actor->font = CLValue::load(S);

    {
        S.IO(actor->Talk_x);
        S.IO(actor->Talk_y);
        S.IO(actor->Talk_text);

        //std::queue<TalkItem> Talk_queue
        S.IO(tmp);
        size_t Talk_queue_size = static_cast<int>(tmp);
        for (size_t i = 0; i < Talk_queue_size; ++i) {
            actor->Talk_queue.push(TalkItem());
            TalkItem &TI = actor->Talk_queue.front();

            S.IO(tmp);
            TI.type = static_cast<TalkItem::Type>(tmp);
            S.IO(TI.text);
            S.IO(TI.delay);
        }
    }

    {
        S.IO(actor->Walk_dest_x);
        S.IO(actor->Walk_dest_y);

        //std::list<Vertex>  Walk_route
        S.IO(tmp);
        auto Walk_route_size = static_cast<size_t>(tmp);
        for (size_t i = 0; i < Walk_route_size; ++i) {
            Vertex V;
            S.IO(V.x);
            S.IO(V.y);
            actor->Walk_route.push_back(V);
        }

        //Vertex Walk_segm1, Walk_segm2
        S.IO(actor->Walk_segm1.x);
        S.IO(actor->Walk_segm1.y);
        S.IO(actor->Walk_segm2.x);
        S.IO(actor->Walk_segm2.y);

        S.IO(actor->Walk_segm_len);
        S.IO(actor->Walk_progress);
    }

    actor->sprite_still = CLValue::load(S);
    actor->sprite_walking = CLValue::load(S);
    actor->sprite_talking = CLValue::load(S);
    actor->sprite_special = CLValue::load(S);

    actor->path_override = CLValue::load(S);

    RoomObject::Load(S, actor); // load roomobject part
    TableObject::Load(S, actor); // load tableobject part

    return actor;
}

std::string Actor::toString() {
    std::stringstream res;

    res << "[Actor:";
    res << " x,y=" << GetPositionX() << "," << GetPositionY() << endl;

    return res.str();
}


