#!/usr/bin/env python3
"""Required Pagan paths must stay aligned with the loaders and the install note."""

import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[3]
GAMEDATA = ROOT / "engines/ultima8/games/GameData.cpp"
U8GAME = ROOT / "engines/ultima8/games/U8Game.cpp"
DOC = ROOT / "docs/pentagram_install.md"

# Paths that abort a new game when missing. Speech flexes are on demand.
REQUIRED = [
    "static/u8pal.pal",
    "static/fixed.dat",
    "static/u8shapes.flx",
    "static/typeflag.dat",
    "static/anim.dat",
    "static/wpnovlay.dat",
    "static/glob.flx",
    "static/u8fonts.flx",
    "static/u8mouse.shp",
    "static/u8gumps.flx",
    "static/gumpage.dat",
    "usecode/eusecode.flx",
    "sound/music.flx",
    "sound/sound.flx",
    "savegame/u8save.000",
    "NONFIXED.DAT",
    "ITEMCACH.DAT",
    "NPCDATA.DAT",
]


def load_u8_data_body(text):
    start = text.find("void GameData::loadU8Data()")
    end = text.find("\nvoid GameData::", start + 1)
    if start < 0 or end < 0:
        raise SystemExit("loadU8Data not found")
    return text[start:end]


def start_game_body(text):
    start = text.find("bool U8Game::startGame()")
    end = text.find("\nvoid U8Game::", start + 1)
    if start < 0 or end < 0:
        raise SystemExit("startGame not found")
    return text[start:end]


def main():
    gamedata = GAMEDATA.read_text(encoding="utf-8", errors="replace")
    u8game = U8GAME.read_text(encoding="utf-8", errors="replace")
    doc = DOC.read_text(encoding="utf-8", errors="replace")
    load_body = load_u8_data_body(gamedata)
    palette_body = u8game[u8game.find("bool U8Game::loadFiles()"):u8game.find("bool U8Game::startGame()")]
    start_body = start_game_body(u8game)
    sources = load_body + palette_body + start_body

    missing_doc = [path for path in REQUIRED if path not in doc]
    missing_src = []
    for path in REQUIRED:
        if path.endswith(".DAT") and path != "static/fixed.dat":
            if path not in start_body:
                missing_src.append(path)
            continue
        if path == "usecode/eusecode.flx":
            if "usecode/" not in load_body or "usecode.flx" not in load_body:
                missing_src.append(path)
            continue
        if path not in sources and path.lower() not in sources.lower():
            missing_src.append(path)

    read_calls = re.findall(r'ReadFile\("(@game/[^"]+)"\)', load_body + palette_body + start_body)
    unnamed = []
    for call in read_calls:
        rel = call.replace("@game/", "")
        if rel.startswith("sound/") and rel.endswith(".flx") and rel not in ("sound/music.flx", "sound/sound.flx"):
            continue
        if rel.startswith("usecode/"):
            if "usecode/eusecode.flx" not in doc:
                unnamed.append(rel)
            continue
        if rel not in doc and rel.upper() not in doc:
            unnamed.append(rel)

    failed = False
    if missing_doc:
        print("install note missing:", ", ".join(missing_doc))
        failed = True
    if missing_src:
        print("loader missing:", ", ".join(missing_src))
        failed = True
    if unnamed:
        print("loader path not documented:", ", ".join(unnamed))
        failed = True
    if "reference/" not in doc or "not a playable install" not in doc:
        print("install note must say reference/ is not a playable install")
        failed = True
    if failed:
        return 1
    print("required Pagan paths match loadU8Data, startGame, and docs/pentagram_install.md")
    return 0


if __name__ == "__main__":
    sys.exit(main())
