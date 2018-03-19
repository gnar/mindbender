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

#include "sushimodule.h"

#include "game.h"
#include "camera.h"

#include "object/sprite.h"
#include "object/font.h"
#include "object/room.h"
#include "object/roomobject.h"
#include "object/item.h"
#include "object/bagitem.h"
#include "object/shape.h"
#include "object/actor.h"
#include "object/sound.h"
#include "object/timer.h"

#define DECL_FUNC(name) CLValue name(CLThread &thread, std::vector<CLValue> &args, CLValue self)

#include <iostream>
using namespace std;

// global functions
static DECL_FUNC(execute);
static DECL_FUNC(exit_thread);

static DECL_FUNC(add_event);
static DECL_FUNC(signal_event);
static DECL_FUNC(suspend_events);
static DECL_FUNC(resume_events);
static DECL_FUNC(is_suspended);

static DECL_FUNC(get_ticks);
static DECL_FUNC(random);

static DECL_FUNC(set_cursor);

// member functions
static DECL_FUNC(sprite_new);
static DECL_FUNC(sprite_draw);
static DECL_FUNC(sprite_rewind);
static DECL_FUNC(sprite_is_finished);
static DECL_FUNC(sprite_translate);

static DECL_FUNC(font_new);
static DECL_FUNC(font_print);
static DECL_FUNC(font_print_center);
static DECL_FUNC(font_get_length);
static DECL_FUNC(font_get_height);

static DECL_FUNC(room_new);
static DECL_FUNC(room_add_object);
static DECL_FUNC(room_rem_object);
static DECL_FUNC(room_object_at);
static DECL_FUNC(room_set_constant_scaling);
static DECL_FUNC(room_set_perspective_scaling);
static DECL_FUNC(room_enable_scaling);
static DECL_FUNC(room_disable_scaling);

static DECL_FUNC(shape_new);
static DECL_FUNC(shape_hit);
static DECL_FUNC(shape_find_vertical);

static DECL_FUNC(item_new);

static DECL_FUNC(bagitem_new);

static DECL_FUNC(roomobject_add_bagitem);
static DECL_FUNC(roomobject_rem_bagitem);
static DECL_FUNC(roomobject_has_bagitem);

static DECL_FUNC(actor_new);
static DECL_FUNC(actor_walkto);
static DECL_FUNC(actor_iswalking);
static DECL_FUNC(actor_talk);
static DECL_FUNC(actor_append_talk);
static DECL_FUNC(actor_istalking);
static DECL_FUNC(actor_perform);
static DECL_FUNC(actor_stop);

static DECL_FUNC(sound_new);
static DECL_FUNC(sound_play);
static DECL_FUNC(sound_stop);

static DECL_FUNC(timer_new);

static DECL_FUNC(camera_settarget);
static DECL_FUNC(camera_setscrollmode);
static DECL_FUNC(camera_setsnapmode);
static DECL_FUNC(camera_setscrollspeed);
static DECL_FUNC(camera_getroom);
static DECL_FUNC(camera_getoffset);
static DECL_FUNC(camera_setwindow);

SushiModule::SushiModule() : CLModule("adv")
{
	// thread functions
	registerFunction("Execute", "sushi_execute", &execute);
	registerFunction("ExitThread", "adv_exit_thread", &exit_thread);

	// event functions
	registerFunction("AddEvent", "adv_add_event", &add_event);
	registerFunction("SignalEvent", "adv_signal_event", &signal_event);
	registerFunction("ResumeEvents", "adv_resume_events", &suspend_events);
	registerFunction("SuspendEvents", "adv_suspend_events", &resume_events);
	registerFunction("IsSuspended", "adv_is_suspended", &is_suspended);

	// misc functions
	registerFunction("GetTime",   "adv_get_ticks", &get_ticks);
	registerFunction("Random",    "adv_random", &random);
	registerFunction("SetCursor", "adv_set_cursor", &set_cursor);

	// sprite constructor/member functions
	registerFunction("Sprite", "adv_sprite_new", &sprite_new);
	registerFunction("adv_sprite_new", &sprite_new);
	registerFunction("adv_sprite_draw", &sprite_draw);
	registerFunction("adv_sprite_rewind", &sprite_rewind);
	registerFunction("adv_sprite_is_finished", &sprite_is_finished);
	registerFunction("adv_sprite_translate", &sprite_translate);

	// font constructor/member functions
	registerFunction("Font", "adv_font_new", &font_new);
	registerFunction("adv_font_print", &font_print);
	registerFunction("adv_font_print_center", &font_print_center);
	registerFunction("adv_font_get_length", &font_get_length);
	registerFunction("adv_font_get_height", &font_get_height);

	// room constructor/member functions
	registerFunction("Room", "adv_room_new", &room_new);
	registerFunction("adv_room_add_object", &room_add_object);
	registerFunction("adv_room_rem_object", &room_rem_object);
	registerFunction("adv_room_object_at", &room_object_at);
	registerFunction("adv_room_set_constant_scaling", &room_set_constant_scaling);
	registerFunction("adv_room_set_perspective_scaling", &room_set_perspective_scaling);
	registerFunction("adv_room_enable_scaling", &room_enable_scaling);
	registerFunction("adv_room_disable_scaling", &room_disable_scaling);

	// shape constructor/member functions
	registerFunction("Shape", "adv_shape_new", &shape_new);
	registerFunction("adv_shape_hit", &shape_hit);
	registerFunction("adv_shape_find_vertical", &shape_find_vertical);

	// bagitem constructor/member functions
	registerFunction("BagItem", "adv_bagitem_new", &bagitem_new);

	// item constructor/member functions
	registerFunction("Item", "adv_item_new", &item_new);
	
	// actor constructor/member functions
	registerFunction("Actor", "adv_actor_new", &actor_new);
	registerFunction("adv_actor_walkto", &actor_walkto);
	registerFunction("adv_actor_iswalking", &actor_iswalking);
	registerFunction("adv_actor_talk", &actor_talk);
	registerFunction("adv_actor_appendtalk", &actor_append_talk);
	registerFunction("adv_actor_istalking", &actor_istalking);
	registerFunction("adv_actor_stop", &actor_stop);
	registerFunction("adv_actor_perform", &actor_perform);

	// roomobject member functions (common to items and actors)
	registerFunction("adv_roomobject_add_bagitem", &roomobject_add_bagitem);
	registerFunction("adv_roomobject_rem_bagitem", &roomobject_rem_bagitem);
	registerFunction("adv_roomobject_has_bagitem", &roomobject_has_bagitem);
	
	// sound constructor/member functions
	registerFunction("Sound", "adv_sound_new", &sound_new);
	registerFunction("adv_sound_play", &sound_play);
	registerFunction("adv_sound_stop", &sound_stop);

	// timer constructor/member functions
	registerFunction("Timer", "adv_timer_new", &timer_new);
	
	// camera namespace
	registerFunction("CameraSetTarget"      , "adv_camera_settarget"     , &camera_settarget);
	registerFunction("CameraSetScrollMode"  , "adv_camera_setscrollmode" , &camera_setscrollmode);
	registerFunction("CameraSetSnapMode"    , "adv_camera_setsnapmode"   , &camera_setsnapmode);
	registerFunction("CameraGetRoom"        , "adv_camera_getroom"       , &camera_getroom);
	registerFunction("CameraSetScrollSpeed" , "adv_camera_setscrollspeed", &camera_setscrollspeed);
	registerFunction("CameraGetOffset"      , "adv_camera_getoffset"     , &camera_getoffset);
	registerFunction("CameraSetWindow"      , "adv_camera_setwindow"     , &camera_setwindow);
}

SushiModule::~SushiModule()
{
}

////////////////////////////////////////////////////////////////////////
// Functions to check arguments types //////////////////////////////////
////////////////////////////////////////////////////////////////////////

static bool IsArgStr(CLValue &value)
{
	return value.type == CL_STRING;
}

static bool IsArgNum(CLValue &value)
{
	return value.type & CL_RAW_ISNUMERIC;
}

static bool IsArgInt(CLValue &value)
{
	return value.type == CL_INTEGER;
}

#define ARG_NUM(i) ((args.size() > i) && IsArgNum(args[i]))
#define ARG_STR(i) ((args.size() > i) && IsArgStr(args[i]))
#define ARG_INT(i) ((args.size() > i) && IsArgInt(args[i]))

#define ARGC(i) (args.size() == i)

static bool IsUsrDat(CLValue &value)
{
	return value.type == CL_USERDATA;
}

template <class T>
static bool IsObj(CLValue &value)
{
	return IsUsrDat(value) && dynamic_cast<T*>(GET_OBJECT(value));
}

////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS                                                   //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(execute) // Execute("file")
{
	const std::string &file = GET_STRING(args[0])->get();

	return Game().ExecuteScript(file);
}

static DECL_FUNC(exit_thread)
{
	thread.kill();
	return CLValue();
}

static DECL_FUNC(add_event) // AddEvent("scumm", "use", arg0, arg1, ..., func)
{
	const std::string &event_class = GET_STRING(args[0])->get();
	const std::string &event_name = GET_STRING(args[1])->get();
	CLValue event_func = args[args.size()-1];

	EventHandler handler(event_class, event_name);
	handler.SetFunction(event_func);
	handler.SetArgumentCount(args.size() - 3);
	
	for (size_t i=0; i<handler.GetArgumentCount(); ++i)
	{
		CLValue arg = args[i+2];
		if ((arg.type == CL_STRING) && (GET_STRING(arg)->get() == "[any]"))
		{
			handler.SetArgument(i);
		} else {
			handler.SetArgument(i, arg);
		}
	}

	Game().GetEventManager().AddEventHandler(handler);
	return CLValue();
}

static DECL_FUNC(signal_event)
{
	const std::string &event_name = GET_STRING(args[0])->get();

	args.erase(args.begin());
	Game().GetEventManager().Signal(thread.getContext(), event_name, args);

	return CLValue();
}

static DECL_FUNC(suspend_events)
{
	const std::string &event_class = GET_STRING(args[0])->get();
	Game().GetEventManager().SuspendEvents(event_class);
	return CLValue();
}

static DECL_FUNC(resume_events)
{
	const std::string &event_class = GET_STRING(args[0])->get();
	Game().GetEventManager().ResumeEvents(event_class);
	return CLValue();
}

static DECL_FUNC(is_suspended)
{
	const std::string &event_class = GET_STRING(args[0])->get();

	if (Game().GetEventManager().IsSuspended(event_class))
		return CLValue::True();
	else
		return CLValue::False();
}

static DECL_FUNC(get_ticks)
{
	int ticks = static_cast<int>(Game().GetTime());
	return CLValue(ticks);
}

static DECL_FUNC(random) // adv.Random(max), adv.Random(min, max), adv.Random()
{
	switch (args.size())
	{
		case 0: return CLValue(std::rand());

		case 1:
			if (!ARG_INT(0)) goto error;
			return CLValue(std::rand() % args[0].toInt());

		case 2:
		{
			if (!ARG_INT(0) || !ARG_INT(1)) goto error;
			int min = args[0].toInt(), max = args[1].toInt();
			return CLValue((std::rand() % (max - min)) + min);
		}
	}

error:
	thread.runtimeError("Error calling Random()", false);
	return CLValue::Null();
}

static DECL_FUNC(set_cursor) // SetCursor(sprite)
{
	//Game().SetCursor(args[0]); TODO
	return CLValue::Null();
}

////////////////////////////////////////////////////////////////////////
// SPRITE METHODS                                                     //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(sprite_new)
{
	if (ARGC(1) && ARG_STR(0))
	{
		const std::string &str_id = GET_STRING(args[0])->get();
		return CLValue(new Sprite(thread.getContext(), str_id));
	} else goto error;
	return CLValue();

error:
	thread.runtimeError(
		"Invalid arguments: Sprite(..) function\n"
		"\tExpected: Sprite(id); with id being a string", false
	);
	return CLValue();
}

static DECL_FUNC(sprite_draw) // sprite.Draw() / sprite.Draw(x, y)
{
	Sprite *sprite;
	if (!IsObj<Sprite>(self)) goto error;
	sprite = GET_SPRITE(self);

	if (ARGC(0)) {
		sprite->Draw();
	} else if (ARGC(2) && ARG_NUM(0) && ARG_NUM(1)) {
		sprite->Draw(args[0].toInt(), args[1].toInt());
	} else goto error;

	return CLValue();

error:
	clog << "Invalid arguments: <Sprite>.Draw(..) method" << endl;
	clog << " Expected: <Sprite>.Draw();" << endl;
	clog << "           <Sprite>.Draw(x, y); with x, y being numeric" << endl;
	return CLValue();
}

static DECL_FUNC(sprite_rewind)
{
	Sprite *sprite;
	if (!IsObj<Sprite>(self)) goto error;
	sprite = GET_SPRITE(self);

	if (ARGC(0)) {
		sprite->Rewind();
	} else goto error;
	return CLValue();

error:
	clog << "Invalid arguments: <Sprite>.Rewind() method" << endl;
	clog << " Expected: <Sprite>.Rewind();" << endl;
	return CLValue();
}

static DECL_FUNC(sprite_is_finished)
{
	Sprite *sprite;
	if (!IsObj<Sprite>(self)) goto error;
	sprite = GET_SPRITE(self);

	if (ARGC(0)) {
		if (sprite->IsFinished())
			return CLValue::True();
		else
			return CLValue::False();
	} else goto error;
	return CLValue();

error:
	clog << "Invalid arguments: <Sprite>.IsFinished() method" << endl;
	clog << " Expected: <Sprite>.IsFinished();" << endl;
	return CLValue();
}

static DECL_FUNC(sprite_translate)
{
	Sprite *sprite;
	if (!IsObj<Sprite>(self)) goto error;
	sprite = GET_SPRITE(self);

	if (ARG_NUM(0) && ARG_NUM(1))
		sprite->Translate(args[0].toInt(), args[1].toInt());
	else 
		goto error;

	return CLValue();

error:
	clog << "Invalid arguments: <sprite>.Translate() method" << endl;
	clog << " Expected: <Sprite>.Translate(x, y) with x, y being numeric" << endl;
	return CLValue();
}

////////////////////////////////////////////////////////////////////////
// FONT METHODS                                                       //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(font_new)
{
	const std::string &str_id = GET_STRING(args[0])->get();
	return CLValue(new FontSet(thread.getContext(), str_id));
}

static DECL_FUNC(font_print) // font.Print(3, 4, "Hello") or font.Print("Hello")
{
	FontSet *font = GET_FONT(self);
	if (args.size() == 3)
	{
		int x = args[0].toInt();
		int y = args[1].toInt();
		const std::string &text = GET_STRING(args[2])->get();
		font->Print(x, y, text);
	} else {
		const std::string &text = GET_STRING(args[0])->get();
		font->Print(text);
	}
	return CLValue();
}

static DECL_FUNC(font_print_center) // font.PrintCenter(3, 2, "Hello")
{
	FontSet *font = GET_FONT(self);
	int x = args[0].toInt();
	int y = args[1].toInt();

	const std::string &text = GET_STRING(args[2])->get();
	font->PrintCenter(x, y, text);
	return CLValue();
}

static DECL_FUNC(font_get_length) // font.GetWidth("text")
{
	FontSet *font = GET_FONT(self);
	const std::string &text = GET_STRING(args[0])->get();
	int result = font->GetWidth(text);
	return CLValue(result);
}

static DECL_FUNC(font_get_height) // font.GetHeight()
{
	FontSet *font = GET_FONT(self);
	int result = font->GetHeight();
	return CLValue(result);
}

////////////////////////////////////////////////////////////////////////
// ROOM METHODS                                                       //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(room_new) // Room(width, height)
{
	if (args.size() == 2)
	{
		int width  = args[0].toInt();
		int height = args[1].toInt();
		return CLValue(new Room(thread.getContext(), width, height));
	}
	return CLValue();
}

static DECL_FUNC(room_object_at)
{
	Room *room = GET_ROOM(self);
	int x = args[0].toInt();
	int y = args[1].toInt();

	RoomObject *obj = room->GetObjectAt(x, y);
	if (obj) {
		return CLValue(obj);
	} else {
		return CLValue::Null();
	}
}

static DECL_FUNC(room_add_object) // room.AddObject(obj)
{
	Room *room = GET_ROOM(self);
	RoomObject *obj = GET_ROOMOBJECT(args[0]);
	room->AddObject(obj);
	return CLValue();
}

static DECL_FUNC(room_rem_object) // room.RemObject(obj)
{
	Room *room = GET_ROOM(self);
	RoomObject *obj = GET_ROOMOBJECT(args[0]);
	room->RemoveObject(obj);
	return CLValue();
}

static DECL_FUNC(room_set_constant_scaling) // room.SetConstantScaling(50)
{
	Room *room = GET_ROOM(self);
	float scl = args[0].toFloat() / 100.0f;
	room->SetConstantScaling(scl);
	return CLValue();
}

static DECL_FUNC(room_set_perspective_scaling)
{
	Room *room = GET_ROOM(self);
	int horizon = args[0].toInt();
	int base    = args[1].toInt();
	room->SetPerspectiveScaling(horizon, base);
	return CLValue();
}

static DECL_FUNC(room_enable_scaling)
{
	Room *room = GET_ROOM(self);
	room->EnableScaling();
	return CLValue::Null();
}

static DECL_FUNC(room_disable_scaling)
{
	Room *room = GET_ROOM(self);
	room->DisableScaling();
	return CLValue::Null();
}

////////////////////////////////////////////////////////////////////////
// ITEM METHODS                                                       //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(item_new)
{
	if (!ARGC(0)) goto error;
	return CLValue(new Item(thread.getContext()));

error:
	clog << "Invalid arguments: Item() function" << endl;
	clog << " Expected: Item()" << endl;
	return CLValue();
}

////////////////////////////////////////////////////////////////////////
// BAGITEM METHODS                                                    //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(bagitem_new)
{
	if (!ARGC(0)) goto error;
	return CLValue(new BagItem(thread.getContext()));

error:
	clog << "Invalid arguments: BagItem() function" << endl;
	clog << " Expected: BagItem()" << endl;
	return CLValue();
}

////////////////////////////////////////////////////////////////////////
// ROOMOBJECT METHODS                                                 //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(roomobject_add_bagitem)
{
	if (!ARGC(1)) goto error;
	if (!IsObj<BagItem>(args[0])) goto error;
	if (!IsObj<RoomObject>(self)) goto error;

	{
		RoomObject *roomobject = GET_ROOMOBJECT(self);
		roomobject->AddToInventory(args[0]);
		return CLValue::Null();
	}

error:
	clog << "Invalid arguments: <actor/item>.AddBagItem(...) method" << endl;
	clog << " Expected: <actor/item>.AddBagItem(bagitem)" << endl;
	return CLValue();
}

static DECL_FUNC(roomobject_rem_bagitem)
{
	if (!ARGC(1)) goto error;
	if (!IsObj<BagItem>(args[0])) goto error;
	if (!IsObj<RoomObject>(self)) goto error;

	{
		RoomObject *roomobject = GET_ROOMOBJECT(self);
		roomobject->RemoveFromInventory(args[0]);
		return CLValue::Null();
	}

error:
	clog << "Invalid arguments: <actor/item>.RemBagItem(...) method" << endl;
	clog << " Expected: <actor/item>.RemBagItem(bagitem)" << endl;
	return CLValue();
}

static DECL_FUNC(roomobject_has_bagitem)
{
	if (!ARGC(1)) goto error;
	if (!IsObj<BagItem>(args[0])) goto error;
	if (!IsObj<RoomObject>(self)) goto error;

	{
	RoomObject *roomobject = GET_ROOMOBJECT(self);
	return (roomobject->HasInInventory(args[0]) ? CLValue::True() : CLValue::False());
	}

error:
	clog << "Invalid arguments: <actor/item>.HasBagItem(...) method" << endl;
	clog << " Expected: <actor/item>.HasBagItem(bagitem)" << endl;
	return CLValue();
}

////////////////////////////////////////////////////////////////////////
// CAMERA FUNCTIONS                                                   //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(camera_settarget)
{
	switch (args.size())
	{
		case 1: // Camera.SetTarget(actor)
			if (!IsObj<Actor>(args[0])) goto error;
			Game().GetCamera().SetTarget(args[0]);
			break;

		case 2: // Camera.SetTarget(x, y)
			if (!ARG_NUM(0) || !ARG_NUM(1)) goto error;
			Game().GetCamera().SetTarget(args[0].toInt(), args[1].toInt());
			                             
			break;

		case 3: // Camera.SetTarget(room, x, y)
			if (!IsObj<Room>(args[0])) goto error;
			if (!ARG_NUM(1) || !ARG_NUM(2)) goto error;
			Game().GetCamera().SetTarget(args[0], args[1].toInt(), args[2].toInt());
			break;

		default:
			goto error;

	}
	return CLValue();

error:
	clog << "Invalid arguments: Camera.SetTarget() function" << endl;
	clog << " Expected: Camera.SetTarget(actor)" << endl;
	clog << "           Camera.SetTarget(x, y)" << endl;
	clog << "           Camera.SetTarget(room, x, y)" << endl;
	return CLValue();
}

static DECL_FUNC(camera_setwindow)
{
	int x, y, w, h;
	
	if (args.size() != 4) goto error;
	if (!ARG_NUM(0) || !ARG_NUM(1)) goto error;
	if (!ARG_NUM(2) || !ARG_NUM(3)) goto error;

	x = args[0].toInt();
	y = args[1].toInt();
	w = args[2].toInt();
	h = args[3].toInt();

	Game().GetCamera().SetCameraWindow(x, y, w, h);
	
	return CLValue();

error:
	clog << "Invalid arguments: Camera.SetWindow() function" << endl;
	clog << " Expected: Camera.SetWindow(x, y, width, height)" << endl;
	return CLValue();
}

static DECL_FUNC(camera_setscrollmode)
{
	Game().GetCamera().SetScrollMode();
	return CLValue();
}

static DECL_FUNC(camera_setsnapmode)
{
	Game().GetCamera().SetSnapMode();
	return CLValue();
}

static DECL_FUNC(camera_getroom)
{
	return Game().GetCamera().GetRoom();
}

static DECL_FUNC(camera_setscrollspeed) // Camera.SetScrollSpeed(speed, accel)
{
	Game().GetCamera().SetScrollSpeed(args[0].toInt(), args[1].toInt());
	return CLValue();
}

static DECL_FUNC(camera_getoffset)
{
	CLValue key, val, retv = CLValue(new CLTable(thread.getContext()));
	GET_TABLE(retv)->set(key = CLValue(new CLString(thread.getContext(), "x")), val = CLValue(Game().GetCamera().GetOffsetX()));
	GET_TABLE(retv)->set(key = CLValue(new CLString(thread.getContext(), "y")), val = CLValue(Game().GetCamera().GetOffsetY()));
	return retv;
}

////////////////////////////////////////////////////////////////////////
// SHAPE METHODS                                                      //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(shape_new) // Shape("polygon_resource")  /  Shape(x, y, radius)  /  Shape(x, y, width, height)
{
	switch (args.size())
	{
		case 1: {
			// create shape from polygon resource
			const std::string &str_id = GET_STRING(args[0])->get();
			return CLValue(new PolygonShape(thread.getContext(), str_id));
		}
		case 3: {
			// create circular shape
			int x = args[0].toInt();
			int y = args[1].toInt();
			int radius = args[2].toInt();
			return CLValue(new CircularShape(thread.getContext(), x, y, radius));
		}
		case 4: {
			// create rectangular shape
			int x      = args[0].toInt();
			int y      = args[1].toInt();
			int width  = args[2].toInt();
			int height = args[3].toInt();
			return CLValue(new RectangularShape(thread.getContext(), x, y, width, height));
		}
	}

	return CLValue::Null();
}

static DECL_FUNC(shape_hit) // shape.Hit(x, y)
{
	Shape *shape = GET_SHAPE(self);
	int x = args[0].toInt();
	int y = args[1].toInt();

	return shape->Hit(x, y) ? CLValue::True() : CLValue::False();
}

static DECL_FUNC(shape_find_vertical) // shape.FindVertical(x, y, range)
{
	Shape *shape = GET_SHAPE(self);
	int x     = args[0].toInt();
	int y     = args[1].toInt();
	int range = args[2].toInt();
	int result;

	if (shape->FindVertical(x, y, range, result))
		return CLValue(result);
	else
		return CLValue::Null();
}

////////////////////////////////////////////////////////////////////////
// ACTOR METHODS                                                      //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(actor_new)
{
	return CLValue(new Actor(thread.getContext()));
}

static DECL_FUNC(actor_stop)
{
	Actor *actor = GET_ACTOR(self);
	actor->Stop();
	return CLValue();
}

static DECL_FUNC(actor_walkto)
{
	//if (!ARG_NUM(0) || !ARG_NUM(1)) goto error;

	Actor *actor = GET_ACTOR(self);
	int x = args[0].toInt();
	int y = args[1].toInt();

	bool result = actor->WalkTo(x, y);
	return result ? CLValue::True() : CLValue::False();

error:
	clog << "Invalid arguments: <actor>.WalkTo() function" << endl;
	clog << " Expected: <actor>.WalkTo(x, y)" << endl;
	return CLValue();
}

static DECL_FUNC(actor_iswalking)
{
	if (!ARGC(0) || !IsObj<Actor>(self)) goto error;

	{
		Actor *actor = GET_ACTOR(self);
		return actor->IsWalking() ? CLValue::True() : CLValue::False();
	}

error:
	clog << "Invalid arguments: <actor>.IsWalking() function" << endl;
	clog << " Expected: <actor>.IsWalking()" << endl;
	return CLValue();
}

static DECL_FUNC(actor_istalking)
{
	if (!ARGC(0) || !IsObj<Actor>(self)) goto error;

	{
		Actor *actor = GET_ACTOR(self);
		return actor->IsTalking() ? CLValue::True() : CLValue::False();
	}

error:
	clog << "Invalid arguments: <actor>.IsTalking() function" << endl;
	clog << " Expected: <actor>.IsTalking()" << endl;
	return CLValue();
}

static DECL_FUNC(actor_talk)
{
	if (!ARGC(1) || !IsObj<Actor>(self)) goto error;
	if (!ARG_STR(0)) goto error;

	{
		Actor *actor = GET_ACTOR(self);
		const std::string &text = GET_STRING(args[0])->get();	
		actor->Talk(text);
	}
	return CLValue();

error:
	clog << "Invalid arguments: <actor>.Talk(..) function" << endl;
	clog << " Expected: <actor>.Talk(text), with 'text' being a string" << endl;
	return CLValue();
}

static DECL_FUNC(actor_append_talk)
{
	if (!ARGC(1) || !IsObj<Actor>(self)) goto error;
	if (!ARG_STR(0)) goto error;

	{
		Actor *actor = GET_ACTOR(self);
		const std::string &text = GET_STRING(args[0])->get();	
		actor->TalkAppend(text);
	}
	return CLValue();

error:
	clog << "Invalid arguments: <actor>.TalkAppend(..) function" << endl;
	clog << " Expected: <actor>.TalkAppend(text), with 'text' being a string" << endl;
	return CLValue();
}

static DECL_FUNC(actor_perform)
{
	if (!ARGC(1) || !IsObj<Actor>(self)) goto error;
	if (!IsObj<Sprite>(args[0])) goto error;

	{
		Actor *actor = GET_ACTOR(self);
		actor->Perform(args[0]);
	}
	return CLValue();

error:
	clog << "Invalid arguments: <actor>.Perform(..) function" << endl;
	clog << " Expected: <actor>.Perform(spr), with 'spr' being a sprite" << endl;
	return CLValue();
}

////////////////////////////////////////////////////////////////////////
// SOUND METHODS                                                      //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(sound_new)
{
	const std::string &res_id = GET_STRING(args[0])->get();
	return CLValue(new Sound(thread.getContext(), res_id));
}

static DECL_FUNC(sound_play)
{
	Sound *sound = GET_SOUND(self);
	sound->Play();
	return CLValue();
}

static DECL_FUNC(sound_stop)
{
	Sound *sound = GET_SOUND(self);
	sound->Stop();
	return CLValue();
}

////////////////////////////////////////////////////////////////////////
// TIMER METHODS                                                      //
////////////////////////////////////////////////////////////////////////

static DECL_FUNC(timer_new)
{
	return CLValue(new Timer(thread.getContext()));
}

