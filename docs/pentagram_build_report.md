# Pentagram (Ultima 8) Engine Build Report

## Build Status: ✅ SUCCESS

**Date:** January 12, 2026  
**Engine:** Pentagram 1.0  
**Binary:** `engines/ultima8/build/pentagram` (3.0 MB)

## Build Configuration

| Component | Status |
|-----------|--------|
| SDL3 Support | ✅ Ported |
| Audio Mixer | ✅ Working |
| Graphics Renderer | ✅ Working |
| Input Handling | ✅ Working |
| MIDI Support | ✅ Timidity and FMOPL linked into the SDL3 mixer |
| Joystick Support | ✅ SDL3 joystick API; keyboard and mouse still work with no device |
| TTF Fonts | ⚠️ Disabled (`PENTAGRAM_USE_SDL_TTF=OFF`; bitmap fonts from `u8fonts.flx`) |

## SDL3 Migration Summary

The original Pentagram codebase was designed for SDL 1.2/2.0. This port required extensive modifications to support SDL3:

### Key Changes

1. **Audio API Migration**
   - SDL3 completely redesigned the audio API
   - Created `AudioMixer.cpp` with SDL3 audio stream support
   - Timidity and FMOPL produce samples into `AudioMixer`; platform MIDI drivers stay unlinked

2. **Event System Migration**
   - Event types renamed: `SDL_KEYDOWN` → `SDL_EVENT_KEY_DOWN`
   - Keyboard event structure changed: `event.keysym.sym` → `event.key`
   - Key constants renamed: `SDLK_a` → `SDLK_A`

3. **Graphics API Migration**
   - `SDL_Surface->format` changed from embedded struct to pointer
   - Created compatibility macros for pixel format access
   - Updated `RenderSurface.cpp` for SDL3 window/renderer API

4. **Removed Functions**
   - `SDL_EnableUNICODE` - Text input now via `SDL_EVENT_TEXT_INPUT`
   - `SDL_EnableKeyRepeat` - Handled by OS
   - `SDL_ShowCursor(int)` - Now `SDL_ShowCursor()`/`SDL_HideCursor()`

### Files Modified

| File | Changes |
|------|---------|
| `misc/sdl2_compat.h` | Comprehensive SDL2→SDL3 compatibility layer |
| `misc/pent_include.h` | Added SDL3 includes |
| `audio/AudioMixer.cpp` | Rewritten for SDL3 audio API |
| `audio/AudioMixer.h` | Updated for SDL3 types |
| `graphics/RenderSurface.cpp` | SDL3 window/renderer support |
| `graphics/BaseSoftRenderSurface.cpp` | Pixel format compatibility |
| `kernel/GUIApp.cpp` | Event handling updates |
| `filesys/IDataSource.h` | Removed SDL_RWops dependency |
| `filesys/OutputLogger.cpp` | SDL3 thread API |

### Stub Files Created

| File | Purpose |
|------|---------|
| `kernel/JoystickStubs.cpp` | Removed. `kernel/Joystick.cpp` uses the SDL3 joystick API |
| `kernel/ToolStubs.cpp` | Disasm/Compile process stubs |
| `audio/XMidiStubs.cpp` | Removed. Software MIDI sources are compiled |

## Build Validation

```
$ ./pentagram --version
Pentagram version 1.0
Built: Jan 12 2026 04:53:04
Optional features: Timidity FMOPL 
Initialising SDL...
-- Initializing Pentagram -- 
Creating FileSystem...
Creating ConfigFileManager...
Creating SettingManager...
Creating Kernel...
```

That January log is historical. It continued into SDL after `--version`, and the feature banner listed Timidity and FMOPL while those drivers were stubbed. Current `--version` exits before SDL and prints only macros that CMake defined for linked sources.

## Dependencies

- SDL3 3.5.0
- zlib
- libpng
- freetype2

## Known Limitations

1. **TTF Fonts** - Still off. Bitmap fonts are the play path.
2. **Play session** - Not recorded. This tree has no `sound/` archives and no `savegame/u8save.000`. See `docs/pentagram_install.md`. Do not treat `reference/` as a substitute install.
3. **Headless boot** - `--version` exits before SDL. A menu or new-game boot still needs a display and a local Pagan tree.

## Next Steps

1. Point `[u8] path` at a real Pagan install (`engines/ultima8/gamedata` or `PENTAGRAM_GAME_PATH`).
2. Boot with `skipstart=yes`, then with the intro.
3. Record palette, GameData, new game, map switch, and a clean quit only after that log exists.
4. Enable `PENTAGRAM_USE_SDL_TTF` only after the job is on SDL `>= 3.2.6` and SDL_ttf `release-3.2.2` is installed.

## File Structure

```
engines/ultima8/
├── CMakeLists.txt          # Build configuration
├── config.h                # Build-time configuration
├── pentagram.cpp           # Main entry point
├── audio/                  # Audio subsystem
│   ├── AudioMixer.cpp      # SDL3 audio mixer
│   ├── midi/               # XMIDI, Timidity, FMOPL
├── graphics/               # Graphics subsystem
│   ├── RenderSurface.cpp   # SDL3 renderer
│   └── BaseSoftRenderSurface.cpp
├── kernel/                 # Core engine
│   ├── GUIApp.cpp          # Main application
│   ├── Joystick.cpp        # SDL3 joystick
│   └── ToolStubs.cpp       # Tool stubs
├── misc/
│   ├── pent_include.h      # Common includes
│   └── sdl2_compat.h       # SDL2→SDL3 compatibility
└── build/
    └── pentagram           # Built binary
```
