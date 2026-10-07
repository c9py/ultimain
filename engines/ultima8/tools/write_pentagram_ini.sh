#!/bin/sh
# Write $HOME/.pentagram/pentagram.ini for a Pagan data root.
# Usage: write_pentagram_ini.sh /path/to/pagan
set -eu

if [ "$#" -ne 1 ] || [ -z "$1" ]; then
    echo "usage: write_pentagram_ini.sh PAGAN_DATA_ROOT" >&2
    exit 2
fi

ROOT=$1
HOME_DIR=${HOME:-.}
INI_DIR="$HOME_DIR/.pentagram"
INI="$INI_DIR/pentagram.ini"
mkdir -p "$INI_DIR"

if [ ! -f "$INI" ]; then
    cat > "$INI" <<EOF
[pentagram]
defaultgame=u8
fullscreen=no
scaler=bilinear
width=640
height=480
bpp=32
ttf=no
midi_driver=timidity
skipstart=yes

[u8]
path=$ROOT
EOF
    exit 0
fi

# Always refresh [u8] path. Add first-run keys only when absent.
tmp=$(mktemp)
awk -v root="$ROOT" '
BEGIN { section=""; saw_u8=0; wrote_path=0; saw_pent=0; saw_midi=0; saw_scaler=0; saw_ttf=0; saw_skip=0; saw_default=0 }
function flush_pent() {
    if (section == "pentagram") {
        if (!saw_default) print "defaultgame=u8"
        if (!saw_scaler) print "scaler=bilinear"
        if (!saw_ttf) print "ttf=no"
        if (!saw_midi) print "midi_driver=timidity"
        if (!saw_skip) print "skipstart=yes"
    }
    if (section == "u8" && !wrote_path) {
        print "path=" root
        wrote_path=1
    }
}
/^\[/ {
    flush_pent()
    section=tolower(substr($0, 2, index($0, "]")-2))
    if (section == "u8") saw_u8=1
    if (section == "pentagram") saw_pent=1
    print
    next
}
section == "u8" && $0 ~ /^path=/ { print "path=" root; wrote_path=1; next }
section == "pentagram" && $0 ~ /^midi_driver=/ { saw_midi=1 }
section == "pentagram" && $0 ~ /^scaler=/ { saw_scaler=1 }
section == "pentagram" && $0 ~ /^ttf=/ { saw_ttf=1 }
section == "pentagram" && $0 ~ /^skipstart=/ { saw_skip=1 }
section == "pentagram" && $0 ~ /^defaultgame=/ { saw_default=1 }
{ print }
END {
    flush_pent()
    if (!saw_pent) {
        print ""
        print "[pentagram]"
        print "defaultgame=u8"
        print "scaler=bilinear"
        print "ttf=no"
        print "midi_driver=timidity"
        print "skipstart=yes"
    }
    if (!saw_u8) {
        print ""
        print "[u8]"
        print "path=" root
    }
}
' "$INI" > "$tmp"
mv "$tmp" "$INI"
