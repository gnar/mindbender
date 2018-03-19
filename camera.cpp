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

#include "camera.h"

#include "object/room.h"
#include "object/roomobject.h"

#include <cmath>

#include <iostream>
using namespace std;

#include <GL/gl.h>
#include "dcdraw/dcdraw.h"

Camera::Camera()
{
	Clear();
}

Camera::~Camera()
{
}

void Camera::Clear()
{
	cur_room.setNull();
	dest_room.setNull();
	seeked.setNull();

	dest_x = dest_y = 0;
	cur_x = cur_y = 0;

	camera_mode = MANUAL;
	scroll_mode = SNAP;

	window.x = 0;
	window.y = 0;
	window.width = 640;
	window.height = 360;

	scroll_speed = 300;
	scroll_accel = 0;
}

void Camera::SetScrollMode()
{
	scroll_mode = SCROLL;
}

void Camera::SetSnapMode()
{
	scroll_mode = SNAP;
}


void Camera::SetTarget(CLValue room, int x, int y)
{
	dest_room = room;
	SetTarget(x, y);

	//Update(0.0f);
}

void Camera::SetTarget(int x, int y)
{
	camera_mode = MANUAL;

	dest_x = x;
	dest_y = y;
	seeked.setNull();

	//Update(0.0f);
}

void Camera::SetTarget(CLValue item)
{
	camera_mode = SEEK;
	seeked = item;

	//Update(0.0f);
}


void Camera::Update(float dt)
{
	// update camera destination
	switch (camera_mode)
	{
		case SEEK: 
			if (!seeked.isNull())
			{
				RoomObject *seekee = GET_ROOMOBJECT(seeked);
				dest_x = seekee->GetPositionX() - window.width/2;
				dest_y = seekee->GetPositionY() - window.height/2;
				dest_room = seekee->GetRoom();
			} else {
				dest_x = 0;
				dest_y = 0;
			}
			break; 

		case MANUAL:
			break;
	}

	// keep camera in room
	if (dest_x < 0) dest_x = 0;
	if (dest_y < 0) dest_y = 0;

	if (!dest_room.isNull())
	{
		if (dest_x >= GET_ROOM(dest_room)->GetWidth() - window.width ) dest_x = GET_ROOM(dest_room)->GetWidth()  - window.width;
		if (dest_y >= GET_ROOM(dest_room)->GetHeight()- window.height) dest_y = GET_ROOM(dest_room)->GetHeight() - window.height;
	} else {
		dest_x = dest_y = 0;
	}

	// move camera towards destination
	switch (scroll_mode)
	{
		case SNAP:
			cur_x = static_cast<float>(dest_x);
			cur_y = static_cast<float>(dest_y);
			cur_room = dest_room;
			break;

		case SCROLL:
			if (cur_room.op_eq(dest_room).getBoolUnsave()) // on room change, snap anyway
			{
				cur_x = static_cast<float>(dest_x);
				cur_y = static_cast<float>(dest_y);
				cur_room = dest_room;
			} else { // Smooth scroll
				float steplen = (dt * float(scroll_speed)) / 1000.0f; // maximum movement in this frame in pixels without acceleration
				if (scroll_accel != 0)
				{
					float dist = std::sqrt((cur_x - dest_x) * (cur_x - dest_x) + (cur_y - dest_y) * (cur_y - dest_y)); // src->dest distance / 1000
					float speed_factor = float(scroll_accel) * dist / 1000.0f;
					steplen *= speed_factor;
				}

				if (cur_x < dest_x) cur_x += steplen;
				if (cur_x > dest_x) cur_x -= steplen;

				//cur_x = dest_x;
				cur_y = static_cast<float>(dest_y);
				cur_room = dest_room;
			}
			break;
	}
	cur_room = dest_room;

	// update room
	if (!cur_room.isNull()) GET_ROOM(cur_room)->Update(dt);
}

void Camera::Draw()
{
	// draw camera window
	if (!cur_room.isNull())
	{
		Room *room = GET_ROOM(cur_room);
		DCDraw::OpenGLClipScreen(true, window.x, window.y, window.width, window.height);

		// Calculate scroll offsets for all planes
		int plane_offset_x[Room::NUM_PLANES];
		int plane_offset_y[Room::NUM_PLANES];
		
		for (int p=0; p<Room::NUM_PLANES; ++p)
		{
			Room::PlaneID pid = (Room::PlaneID)p;
			switch (pid)
			{
				case Room::MAIN_PLANE:
					plane_offset_x[p] = cur_x;
					plane_offset_y[p] = cur_y;
					break;
					
				case Room::FOREGROUND_PLANE:
				case Room::BACKGROUND_PLANE: 
				{
					int p_width  = room->GetWidth(pid);
					int p_height = room->GetHeight(pid);
					int r_width  = room->GetWidth();
					int r_height = room->GetHeight();
					
					if (p_width <= window.width)
						plane_offset_x[p] = 0;
					else
						plane_offset_x[p] = ((p_width - window.width) * cur_x) / (r_width - window.width);

					if (p_height <= window.height)
						plane_offset_y[p] = 0;
					else
						plane_offset_y[p] = ((p_height - window.height) * cur_y) / (r_height - window.height);
					
					break;
				}

				default:
					break;
			}
		}

		// Draw background, main, foreground layer
		for (int p=0; p<Room::NUM_PLANES; ++p)
		{
			Room::PlaneID pid = (Room::PlaneID)p;

			// This is a hack. This global transformation should really be in the dcdraw lib
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(window.x, window.y, 0);
			glTranslatef(-plane_offset_x[p], -plane_offset_y[p], 0);

			room->Draw(pid);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}

		// draw actor chat texts
		for (int p=0; p<Room::NUM_PLANES; ++p)
		{
			Room::PlaneID pid = (Room::PlaneID)p;

			// This is a hack. This global transformation should really be in the dcdraw lib
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(window.x, window.y, 0);
			glTranslatef(-plane_offset_x[p], -plane_offset_y[p], 0);

			room->DrawActorTexts(pid);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}

		DCDraw::OpenGLClipScreen(false);
	}

}

bool Camera::ScreenToRoomCoordinates(int &X, int &Y)
{
	if (cur_room.isNull()) {
		X = Y = -1;
		return false;
	}

	int room_x = X + int(cur_x) - window.x; // translate to room coordinates
	int room_y = Y + int(cur_y) - window.y;

	bool res = (   X >= window.x && X < window.x + window.width
	            && Y >= window.y && Y < window.y + window.height
	            && room_x >= 0 && room_x < GET_ROOM(cur_room)->GetWidth() 
	            && room_y >= 0 && room_y < GET_ROOM(cur_room)->GetHeight());

	X = room_x;
	Y = room_y;
	return res;
}
	
void Camera::RoomToScreenCoordinates(int &X, int &Y)
{
	if (cur_room.isNull()) {
		X = Y = -1;
		return;
	}

	X = X - int(cur_x) + window.x;
	Y = Y - int(cur_y) + window.y;
}

void Camera::GetCameraWindow(int &x, int &y, int &width, int &height)
{
	x = window.x;
	y = window.y;
	width = window.width;
	height = window.height;
}

void Camera::SetCameraWindow(int x, int y, int width, int height)
{
	window.x = x;
	window.y = y;
	window.width = width;
	window.height = height;
}

/////////////////////////////////////////////////////////////////////////
// GARBAGE COLLECTING                                                  //
/////////////////////////////////////////////////////////////////////////

void Camera::markObjects()
{
	cur_room.markObject();
	dest_room.markObject();
	seeked.markObject();
}

/////////////////////////////////////////////////////////////////////////
// SAVE & LOAD STATE                                                   //
/////////////////////////////////////////////////////////////////////////

void Camera::Save(CLSerialSaver &S)
{
	int tmp;

	S.magic("[CAM]");
	
	// camera modes
	S.IO(tmp = (int)camera_mode);
	S.IO(tmp = (int)scroll_mode);

	// current camera coordinates
	CLValue::save(S, cur_room); // current room 
	S.IO(cur_x);
	S.IO(cur_y);

	// current camera destination
	CLValue::save(S, dest_room);

	// camera movement target
	S.IO(dest_x);
	S.IO(dest_y);

	// seeked object
	CLValue::save(S, seeked);

	// scroll speed and acceleration
	S.IO(scroll_speed); 
	S.IO(scroll_accel);

	// camera area on screen
	S.IO(window.x);
	S.IO(window.y);	
	S.IO(window.width);
	S.IO(window.height);

	S.magic("[END-CAM]");
}

void Camera::Load(CLSerialLoader &S)
{
	int tmp;

	S.magic("[CAM]");
	
	// camera modes
	S.IO(tmp); camera_mode = static_cast<CameraMode>(tmp);
	S.IO(tmp); scroll_mode = static_cast<ScrollMode>(tmp);

	// current camera coordinates
	cur_room = CLValue::load(S);
	S.IO(cur_x);
	S.IO(cur_y);

	// current camera destination
	dest_room = CLValue::load(S);

	// camera movement target
	S.IO(dest_x);
	S.IO(dest_y);

	// seeked object
	seeked = CLValue::load(S);

	// scroll speed and acceleration
	S.IO(scroll_speed); 
	S.IO(scroll_accel);

	// camera area on screen
	S.IO(window.x);
	S.IO(window.y);	
	S.IO(window.width);
	S.IO(window.height);

	S.magic("[END-CAM]");
}

