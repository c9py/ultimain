/*
 * SDL2 to SDL3 Compatibility Layer for Pentagram
 * This header provides compatibility macros and types to allow
 * SDL2-based code to compile with SDL3.
 */

#ifndef SDL2_COMPAT_H
#define SDL2_COMPAT_H

/* Undefine SDL3's renamed macros first to avoid conflicts */
#undef AUDIO_S16SYS
#undef AUDIO_S16LSB
#undef AUDIO_S16MSB
#undef AUDIO_U8
#undef AUDIO_S8
#undef AUDIO_S32SYS
#undef AUDIO_F32SYS

/* ============================================
 * Audio API Compatibility
 * SDL3 completely redesigned the audio API
 * ============================================ */

/* Audio format constants */
#define AUDIO_S16SYS SDL_AUDIO_S16
#define AUDIO_S16LSB SDL_AUDIO_S16LE
#define AUDIO_S16MSB SDL_AUDIO_S16BE
#define AUDIO_U8 SDL_AUDIO_U8
#define AUDIO_S8 SDL_AUDIO_S8
#define AUDIO_S32SYS SDL_AUDIO_S32
#define AUDIO_F32SYS SDL_AUDIO_F32

typedef void (*SDL2_AudioCallback)(void *userdata, Uint8 *stream, int len);

typedef struct SDL2_AudioSpec {
    int freq;
    SDL_AudioFormat format;
    Uint8 channels;
    Uint8 silence;
    Uint16 samples;
    Uint16 padding;
    Uint32 size;
    SDL2_AudioCallback callback;
    void *userdata;
} SDL2_AudioSpec;

static inline int SDL2_OpenAudio(SDL2_AudioSpec *desired, SDL2_AudioSpec *obtained) {
    (void)desired; (void)obtained;
    return -1;
}
static inline void SDL2_PauseAudio(int pause_on) { (void)pause_on; }
static inline void SDL2_CloseAudio(void) { }
static inline void SDL2_LockAudio(void) { }
static inline void SDL2_UnlockAudio(void) { }

#define SDL_OpenAudio SDL2_OpenAudio
#define SDL_PauseAudio SDL2_PauseAudio
#define SDL_CloseAudio SDL2_CloseAudio
#define SDL_LockAudio SDL2_LockAudio
#define SDL_UnlockAudio SDL2_UnlockAudio

/* ============================================
 * Event Type Compatibility
 * SDL3 renamed event types - undefine first
 * ============================================ */

#undef SDL_KEYDOWN
#undef SDL_KEYUP
#undef SDL_MOUSEBUTTONDOWN
#undef SDL_MOUSEBUTTONUP
#undef SDL_MOUSEMOTION
#undef SDL_MOUSEWHEEL
#undef SDL_QUIT
#undef SDL_WINDOWEVENT
#undef SDL_TEXTINPUT
#undef SDL_TEXTEDITING
#undef SDL_JOYAXISMOTION
#undef SDL_JOYBUTTONDOWN
#undef SDL_JOYBUTTONUP
#undef SDL_JOYHATMOTION

#define SDL_KEYDOWN SDL_EVENT_KEY_DOWN
#define SDL_KEYUP SDL_EVENT_KEY_UP
#define SDL_MOUSEBUTTONDOWN SDL_EVENT_MOUSE_BUTTON_DOWN
#define SDL_MOUSEBUTTONUP SDL_EVENT_MOUSE_BUTTON_UP
#define SDL_MOUSEMOTION SDL_EVENT_MOUSE_MOTION
#define SDL_MOUSEWHEEL SDL_EVENT_MOUSE_WHEEL
#define SDL_QUIT SDL_EVENT_QUIT
#define SDL_WINDOWEVENT SDL_EVENT_WINDOW_FIRST
#define SDL_TEXTINPUT SDL_EVENT_TEXT_INPUT
#define SDL_TEXTEDITING SDL_EVENT_TEXT_EDITING
#define SDL_JOYAXISMOTION SDL_EVENT_JOYSTICK_AXIS_MOTION
#define SDL_JOYBUTTONDOWN SDL_EVENT_JOYSTICK_BUTTON_DOWN
#define SDL_JOYBUTTONUP SDL_EVENT_JOYSTICK_BUTTON_UP
#define SDL_JOYHATMOTION SDL_EVENT_JOYSTICK_HAT_MOTION

/* Window event subtypes */
#undef SDL_WINDOWEVENT_RESIZED
#undef SDL_WINDOWEVENT_SIZE_CHANGED
#undef SDL_WINDOWEVENT_EXPOSED
#undef SDL_WINDOWEVENT_FOCUS_GAINED
#undef SDL_WINDOWEVENT_FOCUS_LOST

#define SDL_WINDOWEVENT_RESIZED SDL_EVENT_WINDOW_RESIZED
#define SDL_WINDOWEVENT_SIZE_CHANGED SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED
#define SDL_WINDOWEVENT_EXPOSED SDL_EVENT_WINDOW_EXPOSED
#define SDL_WINDOWEVENT_FOCUS_GAINED SDL_EVENT_WINDOW_FOCUS_GAINED
#define SDL_WINDOWEVENT_FOCUS_LOST SDL_EVENT_WINDOW_FOCUS_LOST

/* ============================================
 * Keyboard Compatibility
 * SDL3 changed key constants and modifiers
 * ============================================ */

typedef SDL_Keycode SDLKey;

/* Undefine renamed modifiers */
#undef KMOD_NONE
#undef KMOD_LSHIFT
#undef KMOD_RSHIFT
#undef KMOD_LCTRL
#undef KMOD_RCTRL
#undef KMOD_LALT
#undef KMOD_RALT
#undef KMOD_SHIFT
#undef KMOD_CTRL
#undef KMOD_ALT

#define KMOD_NONE SDL_KMOD_NONE
#define KMOD_LSHIFT SDL_KMOD_LSHIFT
#define KMOD_RSHIFT SDL_KMOD_RSHIFT
#define KMOD_LCTRL SDL_KMOD_LCTRL
#define KMOD_RCTRL SDL_KMOD_RCTRL
#define KMOD_LALT SDL_KMOD_LALT
#define KMOD_RALT SDL_KMOD_RALT
#define KMOD_SHIFT SDL_KMOD_SHIFT
#define KMOD_CTRL SDL_KMOD_CTRL
#define KMOD_ALT SDL_KMOD_ALT

/* Keypad key compatibility - SDL3 uses SDLK_KP_0 instead of SDLK_KP0 */
#undef SDLK_KP0
#undef SDLK_KP1
#undef SDLK_KP2
#undef SDLK_KP3
#undef SDLK_KP4
#undef SDLK_KP5
#undef SDLK_KP6
#undef SDLK_KP7
#undef SDLK_KP8
#undef SDLK_KP9

#define SDLK_KP0 SDLK_KP_0
#define SDLK_KP1 SDLK_KP_1
#define SDLK_KP2 SDLK_KP_2
#define SDLK_KP3 SDLK_KP_3
#define SDLK_KP4 SDLK_KP_4
#define SDLK_KP5 SDLK_KP_5
#define SDLK_KP6 SDLK_KP_6
#define SDLK_KP7 SDLK_KP_7
#define SDLK_KP8 SDLK_KP_8
#define SDLK_KP9 SDLK_KP_9

/* Letter keys - SDL3 uses uppercase SDLK_A instead of lowercase SDLK_a */
#undef SDLK_a
#undef SDLK_b
#undef SDLK_c
#undef SDLK_d
#undef SDLK_e
#undef SDLK_f
#undef SDLK_g
#undef SDLK_h
#undef SDLK_i
#undef SDLK_j
#undef SDLK_k
#undef SDLK_l
#undef SDLK_m
#undef SDLK_n
#undef SDLK_o
#undef SDLK_p
#undef SDLK_q
#undef SDLK_r
#undef SDLK_s
#undef SDLK_t
#undef SDLK_u
#undef SDLK_v
#undef SDLK_w
#undef SDLK_x
#undef SDLK_y
#undef SDLK_z

#define SDLK_a SDLK_A
#define SDLK_b SDLK_B
#define SDLK_c SDLK_C
#define SDLK_d SDLK_D
#define SDLK_e SDLK_E
#define SDLK_f SDLK_F
#define SDLK_g SDLK_G
#define SDLK_h SDLK_H
#define SDLK_i SDLK_I
#define SDLK_j SDLK_J
#define SDLK_k SDLK_K
#define SDLK_l SDLK_L
#define SDLK_m SDLK_M
#define SDLK_n SDLK_N
#define SDLK_o SDLK_O
#define SDLK_p SDLK_P
#define SDLK_q SDLK_Q
#define SDLK_r SDLK_R
#define SDLK_s SDLK_S
#define SDLK_t SDLK_T
#define SDLK_u SDLK_U
#define SDLK_v SDLK_V
#define SDLK_w SDLK_W
#define SDLK_x SDLK_X
#define SDLK_y SDLK_Y
#define SDLK_z SDLK_Z

/* Other renamed keys */
#undef SDLK_BACKQUOTE
#undef SDLK_PRINT
#undef SDLK_NUMLOCK
#undef SDLK_SCROLLOCK
#undef SDLK_QUOTEDBL
#undef SDLK_QUOTE

#define SDLK_BACKQUOTE SDLK_GRAVE
#define SDLK_PRINT SDLK_PRINTSCREEN
#define SDLK_NUMLOCK SDLK_NUMLOCKCLEAR
#define SDLK_SCROLLOCK SDLK_SCROLLLOCK
#define SDLK_QUOTEDBL SDLK_DBLAPOSTROPHE
#define SDLK_QUOTE SDLK_APOSTROPHE

/* ============================================
 * Removed Functions Compatibility
 * ============================================ */

/* SDL3 removed SDL_EnableUNICODE - text input is always enabled */
static inline int SDL2_EnableUNICODE(int enable) { (void)enable; return 0; }
#define SDL_EnableUNICODE SDL2_EnableUNICODE

/* SDL3 removed SDL_EnableKeyRepeat - key repeat is handled by the OS */
#define SDL_DEFAULT_REPEAT_DELAY 500
#define SDL_DEFAULT_REPEAT_INTERVAL 30
static inline int SDL2_EnableKeyRepeat(int delay, int interval) { (void)delay; (void)interval; return 0; }
#define SDL_EnableKeyRepeat SDL2_EnableKeyRepeat

/* SDL3 changed SDL_ShowCursor */
#undef SDL_DISABLE
#undef SDL_ENABLE
#define SDL_DISABLE false
#define SDL_ENABLE true

/* SDL3 changed SDL_GetMouseState signature */
static inline Uint32 SDL2_GetMouseState(int *x, int *y) {
    float fx, fy;
    Uint32 buttons = SDL_GetMouseState(&fx, &fy);
    if (x) *x = (int)fx;
    if (y) *y = (int)fy;
    return buttons;
}

/* ============================================
 * Keyboard Event Structure Compatibility
 * SDL3 removed keysym from SDL_KeyboardEvent
 * ============================================ */

/* Helper to get key from keyboard event */
static inline SDL_Keycode SDL2_GetKeyFromEvent(const SDL_KeyboardEvent *event) {
    return event->key;
}

static inline SDL_Keymod SDL2_GetModFromEvent(const SDL_KeyboardEvent *event) {
    return event->mod;
}

/* Macro to access keysym.sym - SDL3 uses event->key directly */
#define SDL2_KEYSYM_SYM(event) ((event)->key)
#define SDL2_KEYSYM_MOD(event) ((event)->mod)

/* ============================================
 * Video/Surface Compatibility
 * ============================================ */

#ifndef SDL_HWSURFACE
#define SDL_HWSURFACE 0
#endif
#ifndef SDL_SWSURFACE
#define SDL_SWSURFACE 0
#endif
#ifndef SDL_DOUBLEBUF
#define SDL_DOUBLEBUF 0
#endif
#ifndef SDL_FULLSCREEN
#define SDL_FULLSCREEN SDL_WINDOW_FULLSCREEN
#endif
#ifndef SDL_RESIZABLE
#define SDL_RESIZABLE SDL_WINDOW_RESIZABLE
#endif
#ifndef SDL_OPENGL
#define SDL_OPENGL SDL_WINDOW_OPENGL
#endif

/* ============================================
 * Timer Compatibility
 * ============================================ */

static inline Uint32 SDL2_GetTicks(void) {
    return (Uint32)SDL_GetTicks();
}

/* ============================================
 * Misc Compatibility
 * ============================================ */

#define SDL_WM_SetCaption(title, icon) /* Use SDL_SetWindowTitle instead */
#define SDL_WM_GrabInput(mode) /* Use SDL_SetWindowGrab instead */
#define SDL_WM_IconifyWindow() /* Use SDL_MinimizeWindow instead */

#ifndef SDL_INIT_EVERYTHING
#define SDL_INIT_EVERYTHING (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD)
#endif

/* ============================================
 * SDL_Surface format compatibility
 * ============================================ */

#define SDL2_SURFACE_FORMAT(surf) SDL_GetPixelFormatDetails((surf)->format)
#define SDL2_SURFACE_BPP(surf) SDL_BITSPERPIXEL((surf)->format)
#define SDL2_SURFACE_BYTESPERPIXEL(surf) SDL_BYTESPERPIXEL((surf)->format)

#define SDL3_FMT(surf) SDL_GetPixelFormatDetails((surf)->format)
#define SDL3_RLOSS(surf) (SDL3_FMT(surf) ? (8 - SDL3_FMT(surf)->Rbits) : 0)
#define SDL3_GLOSS(surf) (SDL3_FMT(surf) ? (8 - SDL3_FMT(surf)->Gbits) : 0)
#define SDL3_BLOSS(surf) (SDL3_FMT(surf) ? (8 - SDL3_FMT(surf)->Bbits) : 0)
#define SDL3_ALOSS(surf) (SDL3_FMT(surf) ? (8 - SDL3_FMT(surf)->Abits) : 0)
#define SDL3_RSHIFT(surf) (SDL3_FMT(surf) ? SDL3_FMT(surf)->Rshift : 0)
#define SDL3_GSHIFT(surf) (SDL3_FMT(surf) ? SDL3_FMT(surf)->Gshift : 0)
#define SDL3_BSHIFT(surf) (SDL3_FMT(surf) ? SDL3_FMT(surf)->Bshift : 0)
#define SDL3_ASHIFT(surf) (SDL3_FMT(surf) ? SDL3_FMT(surf)->Ashift : 0)
#define SDL3_RMASK(surf) (SDL3_FMT(surf) ? SDL3_FMT(surf)->Rmask : 0)
#define SDL3_GMASK(surf) (SDL3_FMT(surf) ? SDL3_FMT(surf)->Gmask : 0)
#define SDL3_BMASK(surf) (SDL3_FMT(surf) ? SDL3_FMT(surf)->Bmask : 0)
#define SDL3_AMASK(surf) (SDL3_FMT(surf) ? SDL3_FMT(surf)->Amask : 0)

static inline void SDL2_GetSurfaceFormatMasks(SDL_Surface *surf, 
    Uint32 *rmask, Uint32 *gmask, Uint32 *bmask, Uint32 *amask,
    Uint8 *rshift, Uint8 *gshift, Uint8 *bshift, Uint8 *ashift,
    Uint8 *rloss, Uint8 *gloss, Uint8 *bloss, Uint8 *aloss)
{
    const SDL_PixelFormatDetails *details = SDL_GetPixelFormatDetails(surf->format);
    if (details) {
        if (rmask) *rmask = details->Rmask;
        if (gmask) *gmask = details->Gmask;
        if (bmask) *bmask = details->Bmask;
        if (amask) *amask = details->Amask;
        if (rshift) *rshift = details->Rshift;
        if (gshift) *gshift = details->Gshift;
        if (bshift) *bshift = details->Bshift;
        if (ashift) *ashift = details->Ashift;
        if (rloss) *rloss = 8 - details->Rbits;
        if (gloss) *gloss = 8 - details->Gbits;
        if (bloss) *bloss = 8 - details->Bbits;
        if (aloss) *aloss = 8 - details->Abits;
    }
}

#define SDL_Flip(surface) /* No-op, use SDL_UpdateWindowSurface */

#endif /* SDL2_COMPAT_H */
