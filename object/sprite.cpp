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

#include "object/sprite.h"

#include "game.h"

#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////
// CONSTRUCTION/DESTRUCTION                                     //
//////////////////////////////////////////////////////////////////

Sprite::Sprite(CLContext *context, const Res::Resource::ID &res)
	: TableObject(context),
	  method_draw(new CLExternalFunction(context, "adv_sprite_draw")),
	  method_rewind(new CLExternalFunction(context, "adv_sprite_rewind")),
	  method_is_finished(new CLExternalFunction(context, "adv_sprite_is_finished")),
	  method_translate(new CLExternalFunction(context, "adv_sprite_translate")),
	  last_tick((unsigned long)-100), last_time(-1000000),
	  x(0), y(0), scale(1.0f), trans_x(0), trans_y(0)
{
	sprite_res = Res::Manager.GetSprite(res);
	sprite_state = sprite_res->CreateSpriteState();
}

Sprite::~Sprite()
{
	delete sprite_state;
}

//////////////////////////////////////////////////////////////////
// DRAW SPRITE                                                  //
//////////////////////////////////////////////////////////////////

void Sprite::Draw()
{
	Update();

	Res::SpriteVisual vis;
	vis.scale = scale;

	sprite_res->Lock();
	sprite_res->Draw(x - int(trans_x*scale), y - int(trans_y*scale), sprite_state, &vis);
	sprite_res->Unlock();
}

void Sprite::Draw(int x, int y)
{
	this->x = x; this->y = y;
	Draw();
}

void Sprite::Rewind()
{
	sprite_state->Rewind();
	last_time = Game().GetTime();
}

bool Sprite::IsFinished()
{
	return sprite_state->IsFinished();
}

void Sprite::SetTrack(const std::string &track_id)
{
	sprite_state->SetTrack(track_id);
}

bool Sprite::HasTrack(const std::string &track_id)
{
	return sprite_res->HasTrack(track_id);
}

void Sprite::Update()
{
	// make sure this sprite is displayed in adjacent frames, else Rewind().
	unsigned long tick = Game().GetFrameCount();
	if (tick == last_tick) return; // already called Update() this frame?
	if (tick != last_tick + 1) Rewind();
	last_tick = tick;

	// calculate elapsed time
	long new_time = Game().GetTime();
	float delta_time = float(new_time - last_time);
	last_time = new_time;

	// update sprite
	sprite_state->Update(delta_time);
}

//////////////////////////////////////////////////////////////////
// CLObject                                                     //
//////////////////////////////////////////////////////////////////

bool Sprite::get(CLValue &key, CLValue &val)
{
	if (key.type == CL_STRING)
	{
		const std::string k = GET_STRING(key)->get();
		if (k == "Draw") {
			val = this->method_draw; return true;
		} else if (k == "Rewind") {
			val = this->method_rewind; return true;
		} else if (k == "IsFinished") {
			val = this->method_is_finished; return true;
		} else if (k == "Translate") {
			val = this->method_translate; return true;
		} else if (k == "track") {
			val = CLValue(new CLString(getContext(), sprite_state->GetTrack())); return true;
		} else if (k == "x") {
			val = CLValue(this->x); return true;
		} else if (k == "y") {
			val = CLValue(this->y); return true;
		} else if (k == "scale") {
			val = CLValue(static_cast<int>(scale * 100.0f)); return true;
		} 
	}

	return TableObject::get(key, val);
}

void Sprite::set(CLValue &key, CLValue &val)
{
	if (key.type == CL_STRING)
	{
		const std::string k = GET_STRING(key)->get();
		if (k == "x") {
			this->x = val.toInt();
			return;
		} else if (k == "y") {
			this->y = val.toInt();
			return;
		} else if (k == "scale") {
			scale = val.toFloat();
			return;
		} else if (k == "track") {
			const std::string &track_id = GET_STRING(val)->get();
			SetTrack(track_id);
			return;
		} 
	}

	TableObject::set(key, val);
}

void Sprite::markReferenced()
{
	TableObject::markReferenced();

	method_draw.markObject();
	method_rewind.markObject();
	method_is_finished.markObject();
	method_translate.markObject();
}

//////////////////////////////////////////////////////////////////
// SAVE & LOAD STATE                                            //
//////////////////////////////////////////////////////////////////
void Sprite::Save(CLSerialSaver &S, Sprite *sprite)
{
	// resource id
	std::string res_id = sprite->sprite_res->GetID();
	S.IO(res_id);

	// save relevant last_time information
	unsigned long now = Game().GetTime();
	int delta_time = static_cast<int>(now - sprite->last_time);
	S.IO(delta_time);
	
	// save relevant last_tick information
	int lt = 0;
	unsigned long tick = Game().GetFrameCount();
	if (tick == sprite->last_tick) lt = 1;
	if (tick == sprite->last_tick + 1) lt = 2;
	S.IO(lt);

	// x, y, scale, trans_x, trans_y
	S.IO(sprite->x);
	S.IO(sprite->y);
	S.IO(sprite->scale);
	S.IO(sprite->trans_x);
	S.IO(sprite->trans_y);

	// save sprite_state 
	sprite->sprite_state->Save(S);

	// save tableobject data
	TableObject::Save(S, sprite);
}

Sprite *Sprite::Load(CLSerialLoader &S)
{
	// resource id
	std::string res_id;
	S.IO(res_id);

	Sprite *sprite = new Sprite(S.getContext(), res_id);
	S.addPtr(sprite);

	// load relevant last_time information
	int delta_time; S.IO(delta_time);
	sprite->last_time = Game().GetTime() - delta_time;

	// load relevant last_tick information
	int lt; S.IO(lt);
	switch (lt)
	{
		case 0: sprite->last_tick = Game().GetFrameCount() - 100; break;
		case 1: sprite->last_tick = Game().GetFrameCount(); break;
		case 2: sprite->last_tick = Game().GetFrameCount() - 1; break;
		default: assert(0);
	}

	// x, y, scale, trans_x, trans_y
	S.IO(sprite->x);
	S.IO(sprite->y);
	S.IO(sprite->scale);
	S.IO(sprite->trans_x);
	S.IO(sprite->trans_y);

	// load sprite_state
	sprite->sprite_state->Load(S);

	// load tableobject data
	TableObject::Load(S, sprite);

	return sprite;
}

