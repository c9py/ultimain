/*
Copyright (C) 2002-2004 The Pentagram team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "pent_include.h"
#include "Joystick.h"

#include <SDL3/SDL.h>
#include "misc/sdl2_compat.h"
#include <SDL3/SDL.h>
#include "misc/sdl2_compat.h"
#include "GUIApp.h"

static SDL_Joystick * joy[JOY_LAST] = {0};

void InitJoystick()
{
	int i, buttons, axes, balls, hats;
	int count = 0;

	if (!SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
		perr << "SDL joystick subsystem unavailable: " << SDL_GetError() << std::endl;
		return;
	}

	SDL_JoystickID *ids = SDL_GetJoysticks(&count);
	if (!ids || count <= 0) {
		SDL_free(ids);
		return;
	}

	for (i = 0; i < count; ++i)
	{
		if (i >= JOY_LAST)
		{
			perr << "Additional joysticks detected. Cannot initialize more than "
				<< JOY_LAST << "." << std::endl;
			break;
		}

		joy[i] = SDL_OpenJoystick(ids[i]);
		if (joy[i])
		{
			buttons = SDL_GetNumJoystickButtons(joy[i]);
			axes = SDL_GetNumJoystickAxes(joy[i]);
			balls = SDL_GetNumJoystickBalls(joy[i]);
			hats = SDL_GetNumJoystickHats(joy[i]);

			pout << "Initialized joystick " << i + 1 << "." << std::endl;
			pout << "\tButtons: " << buttons << std::endl;
			pout << "\tAxes: " << axes << std::endl;
			pout << "\tBalls: " << balls << std::endl;
			pout << "\tHats: " << hats << std::endl;
		}
		else
		{
			perr << "Error while initializing joystick " << i + 1 << ": "
				<< SDL_GetError() << std::endl;
		}
	}
	SDL_free(ids);
}

void ShutdownJoystick()
{
	int i;
	for (i = 0; i < JOY_LAST; ++i)
	{
		if (joy[i])
		{
			SDL_CloseJoystick(joy[i]);
		}
		joy[i] = 0;
	}
}

DEFINE_RUNTIME_CLASSTYPE_CODE(JoystickCursorProcess,Process);

JoystickCursorProcess::JoystickCursorProcess()
	: Process(), js(JOY1), x_axis(0), y_axis(1), ticks(0), accel(0)
{
}

JoystickCursorProcess::JoystickCursorProcess(Joystick js_, int x_axis_, int y_axis_)
	: Process(), js(js_), x_axis(x_axis_), y_axis(y_axis_), ticks(0), accel(0)
{
	flags |= PROC_RUNPAUSED;
	type = 1;

	if(joy[js] && js < JOY_LAST)
	{
		int axes = SDL_GetNumJoystickAxes(joy[js]);
		if (x_axis >= axes && y_axis >= axes)
		{
			perr << "Failed to start JoystickCursorProcess: illegal axis for x (" << x_axis << ") or y (" << y_axis << ")" << std::endl;
			terminate();
		}
	}
	else
	{
		terminate();
	}

}

JoystickCursorProcess::~JoystickCursorProcess()
{
}

#define AXIS_TOLERANCE 1000

//! CONSTANTS - and a lot of guess work
void JoystickCursorProcess::run()
{
	int dx = 0, dy = 0;
	int now = (int)SDL_GetTicks();

	if(joy[js] && ticks)
	{
		SDL_UpdateJoysticks();
		int tx = now - ticks;
		int r = 350 - accel * 30;
		sint16 jx = SDL_GetJoystickAxis(joy[js], x_axis);
		sint16 jy = SDL_GetJoystickAxis(joy[js], y_axis);
		if (jx > AXIS_TOLERANCE || jx < -AXIS_TOLERANCE)
			dx = ((jx / 1000) * tx) / r;
		if (jy > AXIS_TOLERANCE || jy < -AXIS_TOLERANCE)
			dy = ((jy / 1000) * tx) / r;
	}

	ticks = now;

	if (dx || dy)
	{
		int mx, my;
		GUIApp * app = GUIApp::get_instance();
		app->getMouseCoords(mx, my);
		mx += dx;
		my += dy;
		app->setMouseCoords(mx, my);
		++accel;
		if (accel > 10)
			accel = 10;
	}
	else
	{
		accel = 0;
	}
}

bool JoystickCursorProcess::loadData(IDataSource* ids, uint32 version)
{
	if (!Process::loadData(ids, version)) return false;

	terminateDeferred(); // Don't allow this process to continue
	return true;
}

void JoystickCursorProcess::saveData(ODataSource* ods)
{
	Process::saveData(ods);
	// saves no status
}
