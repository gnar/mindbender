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

#include "sprite_anim.h"
#include "manager.h"

#include <sstream>

#include "tinyxml/tinyxml.h"

#include "loadxml.h"

using namespace std;

namespace Res {

    Sprite_Anim::Sprite_Anim(const Resource::ID &id) : Sprite(id) {
        LoadXML();
    }

    Sprite_Anim::~Sprite_Anim() {
        Unload();
    }

    void Sprite_Anim::Load() {
        loaded = true;
    }

    void Sprite_Anim::Unload() {
        loaded = false;
    }

    bool Sprite_Anim::HasTrack(const std::string &id) {
        for (auto &track : tracks)
            if (track.id == id) return true;
        return false;
    }

    SpriteState *Sprite_Anim::CreateSpriteState() {
        return new SpriteState_Anim(this);
    }

    void Sprite_Anim::Draw(int x, int y, SpriteState *state, SpriteVisual *visual) {
        auto *as = (SpriteState_Anim *) state;
        if (as->display_pos < 0) return;

        TrackItem &item = tracks[as->track_idx].items[as->display_pos];
        Image *img = item.image;

        DCDraw::Transform t;
        t.SetTrans(x, y);
        if (visual) t.SetScale(visual->scale, visual->scale);
        t.SetOrigin(item.origin_x, item.origin_y);

        img->Lock();
        if (item.rect.full) {
            img->GetTexture()->Draw(t);
        } else {
            static DCDraw::Color color;
            DCDraw::Clip clip(item.rect.x, item.rect.y, item.rect.w, item.rect.h);
            img->GetTexture()->Draw(t, color, clip);
        }
        img->Unlock();
    }

    void Sprite_Anim::LoadXML() {
        // Load xml document
        std::unique_ptr<TiXmlDocument> doc(
                LoadXMLDocument(Manager.OpenFile(GetID()))
        );

        TiXmlElement *sprite_elem = doc->FirstChildElement("sprite");
        TiXmlElement *track_elem = sprite_elem ? sprite_elem->FirstChildElement("track") : nullptr;
        while (track_elem) {
            LoadTrack(track_elem);
            track_elem = track_elem->NextSiblingElement("track");
        }
    }

    void Sprite_Anim::LoadTrack(TiXmlElement *track_elem) {
        // create track
        tracks.emplace_back();
        Track &track = *(tracks.rbegin());

        // track id
        const char *track_id = track_elem->Attribute("id");
        //cout << "Track id: " << track_id << endl;
        track.id = track_id;

        // default delay
        if (TIXML_SUCCESS != track_elem->QueryIntAttribute("default-delay", &track.default_delay))
            track.default_delay = 100;

        // default origin
        const char *default_origin_attr = track_elem->Attribute("default-origin");
        if (!default_origin_attr
            || (2 != std::sscanf(default_origin_attr, "%i,%i", &track.default_origin_x, &track.default_origin_y))) {
            track.default_origin_x = 0;
            track.default_origin_y = 0;
        }

        // loop mode
        const char *loop_mode = track_elem->Attribute("loop-mode");
        if (loop_mode == nullptr) {
            track.loop_mode = Track::REWIND;
        } else if (0 == strcmp(loop_mode, "halt")) {
            track.loop_mode = Track::HALT;
        } else if (0 == strcmp(loop_mode, "pingpong")) {
            track.loop_mode = Track::PINGPONG;
        } else {
            track.loop_mode = Track::REWIND;
        }

        // track items..
        TiXmlElement *item_elem = track_elem->FirstChildElement();
        while (item_elem) {
            LoadTrackItem(item_elem, *(tracks.rbegin()));
            item_elem = item_elem->NextSiblingElement();
        }
    }

    void Sprite_Anim::LoadTrackItem(TiXmlElement *item_elem, Track &track) {
        TrackItem item;

        const char *item_type = item_elem->Value();
        if (strcmp(item_type, "display") == 0) {
            item.type = TrackItem::DISPLAY;
            item.image = Manager.GetImage(item_elem->Attribute("image")); // TODO: Check
            item.rect = ParseRectAttribute(item_elem);

            const char *origin_attr = item_elem->Attribute("origin");
            if (!origin_attr || (2 != std::sscanf(origin_attr, "%i,%i", &item.origin_x, &item.origin_y))) {
                item.origin_x = track.default_origin_x;
                item.origin_y = track.default_origin_y;
            }
        } else if (strcmp(item_type, "delay") == 0) {
            item.type = TrackItem::DELAY;
            if (TIXML_SUCCESS != item_elem->QueryIntAttribute("ms", &item.delay)) item.delay = track.default_delay;
        } else {
            // TODO: Check
        }

        track.items.push_back(item);
    }

    Sprite_Anim::Rect Sprite_Anim::ParseRectAttribute(TiXmlElement *elem) {
        int x, y, w, h;
        const char *rect_attr = elem->Attribute("rect");

        if ((!rect_attr) || (std::sscanf(rect_attr, "%i,%i,%i,%i", &x, &y, &w, &h) != 4))
            return Rect();
        else
            return Rect(x, y, w, h);
    }

    void Sprite_Anim::UpdateState(SpriteState_Anim &state, float time_passed) {
        // do nothing if last track item has been progressed
        if (state.finished) {
            // (the _animation_ is finished if last track item is done!!)
            if (state.time < 0.0f) state.time += time_passed;
            return;
        }

        Track &track = tracks[state.track_idx]; // played track
        state.time += time_passed;
        while (state.time >= 0.0f) {
            TrackItem &item = track.items[state.track_pos];
            switch (item.type) {
                case TrackItem::DELAY:
                    state.time -= item.delay;
                    break;

                case TrackItem::DISPLAY:
                    state.display_pos = state.track_pos;
                    break;
            }

            state.track_pos += (state.forward ? 1 : -1);

            if (state.track_pos >= (int) track.items.size()) // overrun?
            {
                switch (track.loop_mode) {
                    case Track::REWIND:
                        state.track_pos = 0;
                        break;
                    case Track::HALT:
                        state.track_pos = track.items.size() - 1;
                        state.finished = true;
                        return;
                    case Track::PINGPONG:
                        state.track_pos = track.items.size() - 1;
                        state.forward = false;
                        break;
                }
            }

            if (state.track_pos < 0) // underrun?
            {
                switch (track.loop_mode) {
                    case Track::REWIND:
                        assert(0);
                    case Track::HALT:
                        assert(0);
                    case Track::PINGPONG: // can only happen in pingpong mode
                        state.track_pos = 0;
                        state.forward = true;
                        break;
                }
            }
        }
    }


    SpriteState_Anim::SpriteState_Anim(Sprite_Anim *sprite)
            : sprite(sprite), track_idx(0), track_pos(0), display_pos(-1), time(0), forward(true), finished(false) {
        Rewind();
    }

    SpriteState_Anim::~SpriteState_Anim() = default;

    void SpriteState_Anim::Update(float time_passed) {
        sprite->UpdateState(*this, time_passed);
    }

    void SpriteState_Anim::Rewind() {
        time = 0.0f;
        track_pos = 0;
        finished = false;
        Update(0.0f);
    }

    void SpriteState_Anim::SetTrack(const std::string &track_id) {
        if (track_id == sprite->tracks[track_idx].id) return;

        for (size_t i = 0; i < sprite->tracks.size(); ++i) {
            if (sprite->tracks[i].id == track_id) {
                track_idx = i;
                display_pos = -1;
                Rewind();
                return;
            }
        }

        // No track change..
    }

    const std::string &SpriteState_Anim::GetTrack() {
        return sprite->tracks[track_idx].id;
    }

    // Save & Load state
    void SpriteState_Anim::Save(CLSerializer &S) {
        int tmp;

        S.IO(track_idx); // current track
        S.IO(track_pos); // current position in track
        S.IO(display_pos); // track display-item that
        S.IO(time); // for delay-statements

        tmp = forward ? 1 : 0; // bool: forward or backward playing
        S.IO(tmp);

        tmp = finished ? 1 : 0; // bool: is animation finished?
        S.IO(tmp);
    }

    void SpriteState_Anim::Load(CLSerializer &S) {
        int tmp;

        S.IO(track_idx); // current track
        S.IO(track_pos); // current position in track
        S.IO(display_pos); // track display-item that
        S.IO(time); // for delay-statements

        S.IO(tmp);
        forward = tmp != 0; // bool: forward or backward playing

        S.IO(tmp);
        finished = tmp != 0; // bool: is animation finished?
    }

} //ns
