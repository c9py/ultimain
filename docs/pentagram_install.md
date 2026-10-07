# Pentagram Pagan install layout

`reference/` is format evidence. It is not a playable install. `reference/u8_static` has static archives, including `FIXED.DAT`, `TYPEFLAG.DAT`, and `U8SHAPES.FLX`. `reference/u8_usecode` has `EUSECODE.FLX`, not `usecode/eusecode.flx`. There is no `sound/` tree and no `savegame/u8save.000` in this repository. Do not copy proprietary game files into the tree.

Put a real Pagan install in `engines/ultima8/gamedata` (gitignored) or point `PENTAGRAM_GAME_PATH` at it. The checked-in template is `engines/ultima8/system/pentagram.ini.template`. The engine reads `$HOME/.pentagram/pentagram.ini` (`HAVE_HOME`). Leave `type` and `language` unset so `GameDetector` identifies the tree. It asks `FileSystem` for lowercase virtual paths; `FileSystem` also tries the uppercase basename, so a CD layout of `STATIC/U8GUMPS.FLX` still matches `static/u8gumps.flx`.

## Required tree

A new game exits unless these paths exist under `[u8] path`:

```
static/u8pal.pal
static/fixed.dat
static/u8shapes.flx
static/typeflag.dat
static/anim.dat
static/wpnovlay.dat
static/glob.flx
static/u8fonts.flx
static/u8mouse.shp
static/u8gumps.flx
static/gumpage.dat
usecode/eusecode.flx
sound/music.flx
sound/sound.flx
savegame/u8save.000
```

`static/u8shapes.cmp` is accepted instead of `static/u8shapes.flx`. French, German, and Japanese installs use `usecode/fusecode.flx`, `usecode/gusecode.flx`, or `usecode/jusecode.flx` instead of `eusecode.flx`. `savegame/u8save.000` must contain `NONFIXED.DAT`, `ITEMCACH.DAT`, and `NPCDATA.DAT`.

Speech flexes (`sound/<letter><shape>.flx`) are loaded on demand and are not required to boot.

## First-run ini

```
scaler=bilinear
ttf=no
midi_driver=timidity
skipstart=yes
```

`midi_driver=disabled` is the explicit null driver. Timidity needs a `timiditycfg` patch set; if that file is missing, `MidiDriver::createInstance` falls through to FMOPL. `ttf=yes` is not the default. `PENTAGRAM_USE_SDL_TTF` is off, and bitmap fonts come from `u8fonts.flx`.

`skipstart=yes` skips the intro. Set `skipstart=no` only after `static/eintro.skf` loads. Failure before the menu is a data-path bug. Failure inside `GameData::loadU8Data` is a missing archive. Failure in `U8Game::startGame` is a missing or unreadable `u8save.000`.

## What this tree cannot claim

No local Pagan install is in the repository, so there is no recorded log of palette load, new game, map switch, walk, talk, combat, or save/load. `tools/osm2ultima` checks do not show that Pentagram loaded a generated map.
