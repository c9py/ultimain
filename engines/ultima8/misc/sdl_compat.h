/* SDL3 compatibility header for Pentagram */
#ifndef SDL_COMPAT_H
#define SDL_COMPAT_H

#include <SDL3/SDL.h>
#include "misc/sdl2_compat.h"

/* SDL3 renamed some types */
typedef SDL_Keycode SDLKey;
typedef SDL_Scancode SDLMod;

/* SDL3 changed some function names */
#define SDL_GetTicks() SDL_GetTicks()
#define SDL_Delay(ms) SDL_Delay(ms)

/* SDL3 removed SDL_rwops - use SDL_IOStream instead */
typedef SDL_IOStream SDL_RWops;
#define SDL_RWFromFile(file, mode) SDL_IOFromFile(file, mode)
#define SDL_RWFromMem(mem, size) SDL_IOFromMem(mem, size)
#define SDL_RWFromConstMem(mem, size) SDL_IOFromConstMem(mem, size)
#define SDL_RWclose(ctx) SDL_CloseIO(ctx)
#define SDL_RWread(ctx, ptr, size, n) SDL_ReadIO(ctx, ptr, (size)*(n))
#define SDL_RWwrite(ctx, ptr, size, n) SDL_WriteIO(ctx, ptr, (size)*(n))
#define SDL_RWseek(ctx, offset, whence) SDL_SeekIO(ctx, offset, whence)
#define SDL_RWtell(ctx) SDL_TellIO(ctx)

/* SDL3 changed event handling */
#define SDL_KEYDOWN SDL_EVENT_KEY_DOWN
#define SDL_KEYUP SDL_EVENT_KEY_UP
#define SDL_MOUSEBUTTONDOWN SDL_EVENT_MOUSE_BUTTON_DOWN
#define SDL_MOUSEBUTTONUP SDL_EVENT_MOUSE_BUTTON_UP
#define SDL_MOUSEMOTION SDL_EVENT_MOUSE_MOTION
#define SDL_QUIT SDL_EVENT_QUIT
#define SDL_VIDEORESIZE SDL_EVENT_WINDOW_RESIZED

/* SDL3 changed video functions */
#define SDL_SetVideoMode(w, h, bpp, flags) NULL /* Replaced by SDL_CreateWindow */
#define SDL_WM_SetCaption(title, icon) /* Replaced by SDL_SetWindowTitle */

#endif /* SDL_COMPAT_H */
