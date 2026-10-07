#!/bin/sh
# Optional install boot. Not a CI requirement.
# Set PENTAGRAM_GAME_PATH to a real Pagan tree, then run this script.
set -eu

BIN=${PENTAGRAM_BIN:-engines/ultima8/build/pentagram}
if [ ! -x "$BIN" ]; then
    echo "pentagram binary not found at $BIN" >&2
    exit 1
fi

"$BIN" --version

if [ -z "${PENTAGRAM_GAME_PATH:-}" ]; then
    echo "PENTAGRAM_GAME_PATH unset; skipping install boot"
    exit 0
fi

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
"$SCRIPT_DIR/write_pentagram_ini.sh" "$PENTAGRAM_GAME_PATH"

# skipstart avoids the intro. A missing archive still exits from loadU8Data.
# This is not a play session: no walk, talk, combat, or save is claimed here.
set +e
OUT=$("$BIN" --game u8 2>&1)
STATUS=$?
set -e
printf '%s\n' "$OUT"
if printf '%s\n' "$OUT" | grep -E "Unable to load|std::exit" >/dev/null; then
    echo "Pagan boot failed: missing archive or savegame" >&2
    exit 1
fi
exit "$STATUS"
