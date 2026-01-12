/*
RenderSurface.cpp : RenderSurface Interface source file

Copyright (C) 2002, 2003 The Pentagram Team
Copyright (C) 2025 SDL3 port

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
#include "RenderSurface.h"
#include "SoftRenderSurface.h"
#include <SDL3/SDL.h>
#include "misc/sdl2_compat.h"
#include <cmath>

#if defined(WIN32) && defined(I_AM_COLOURLESS_EXPERIMENTING_WITH_D3D)
#include "D3D9SoftRenderSurface.h"
#endif

RenderSurface::Format	RenderSurface::format = {
	0,	0,
	0,	0,	0,	0,
	0,	0,	0,	0,
	0,	0,	0,	0,
	0,	0,	0,	0
};

uint8 RenderSurface::Gamma10toGamma22[256];
uint8 RenderSurface::Gamma22toGamma10[256];

// SDL3 global window and surface (stored here for simplicity)
static SDL_Window *g_window = nullptr;
static SDL_Surface *g_window_surface = nullptr;

//
// RenderSurface::SetVideoMode()
//
// Desc: Create a standard RenderSurface
// Returns: Created RenderSurface or 0
//

RenderSurface *RenderSurface::SetVideoMode(uint32 width,		// Width of desired mode
									uint32 height,		// Height of desired mode
									uint32 bpp,			// Bits Per Pixel of desired mode
									bool fullscreen,	// Fullscreen if true, Windowed if false
									bool use_opengl)	// Use OpenGL if true, Software if false
{
	// TODO: Add in OpenGL
	if (use_opengl)
	{
		pout << "OpenGL Mode not enabled" << std::endl;
		// TODO: Set Error Code
		return 0;
	}

	// check to make sure a 16 bit or 32 bit Mode has been requested
	if (bpp != 16 && bpp != 32)
	{
		pout << "Only 16 bit and 32 bit video modes supported" << std::endl;
		// TODO: Set Error Code
		return 0;
	}

	// SDL3: Create window with appropriate flags
	Uint32 window_flags = 0;
	
	if (fullscreen) {
		window_flags |= SDL_WINDOW_FULLSCREEN;
	}

	// Create the window
	g_window = SDL_CreateWindow("Pentagram", width, height, window_flags);
	
	if (!g_window)
	{
		pout << "SDL_CreateWindow() failed: " << SDL_GetError() << std::endl;
		return 0;
	}

	// Get the window surface
	g_window_surface = SDL_GetWindowSurface(g_window);
	
	if (!g_window_surface)
	{
		pout << "SDL_GetWindowSurface() failed: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(g_window);
		g_window = nullptr;
		return 0;
	}

	// Now create the SoftRenderSurface
	RenderSurface *surf;

	// TODO: Change this
#if defined(WIN32) && defined(I_AM_COLOURLESS_EXPERIMENTING_WITH_D3D)
	if (bpp == 32) surf = new D3D9SoftRenderSurface<uint32>(width,height,fullscreen);
	else surf = new D3D9SoftRenderSurface<uint16>(width,height,fullscreen);
#else
	if (bpp == 32) surf = new SoftRenderSurface<uint32>(g_window_surface);
	else surf = new SoftRenderSurface<uint16>(g_window_surface);
#endif

	// Initialize gamma correction tables
	for (int i = 0; i < 256; i++)
	{
		Gamma22toGamma10[i] = static_cast<uint8>(0.5 + (std::pow (i/255.0, 2.2/1.0) * 255.0));
		Gamma10toGamma22[i] = static_cast<uint8>(0.5 + (std::pow (i/255.0, 1.0/2.2) * 255.0));
	}

	return surf;
}

// Create a SecondaryRenderSurface with an associated Texture object
RenderSurface *RenderSurface::CreateSecondaryRenderSurface(uint32 width, uint32 height)
{
	// Now create the SoftRenderSurface
	RenderSurface *surf;

	// TODO: Change this
	if (format.s_bpp == 32) surf = new SoftRenderSurface<uint32>(width,height);
	else surf = new SoftRenderSurface<uint16>(width,height);
	return surf;
}

RenderSurface::~RenderSurface()
{
}

// SDL3: Get the global window for updates
SDL_Window* RenderSurface::GetSDLWindow()
{
	return g_window;
}

// SDL3: Update the window surface
void RenderSurface::UpdateWindowSurface()
{
	if (g_window) {
		SDL_UpdateWindowSurface(g_window);
	}
}
