# Web Launcher - CheerpX Integration

This directory contains the web-based launcher for running Ultima engines (Exult and Pentagram) directly in the browser using CheerpX x86-to-WebAssembly virtualization.

## Files

- `index.html` - Main launcher interface
- `js/cheerpx-engine.js` - CheerpX integration module
- `js/data-manager.js` - IndexedDB game data manager
- `assets/ultima-engines.ext2.gz` - Compressed Linux filesystem containing engine binaries (11MB)
- `assets/build-diskimage.sh` - Script to rebuild the disk image
- `assets/rootfs/` - Template filesystem structure for disk image

## Deployment

### Prerequisites

The web launcher requires:
1. **SharedArrayBuffer support** - The server must send these headers:
   ```
   Cross-Origin-Opener-Policy: same-origin
   Cross-Origin-Embedder-Policy: require-corp
   ```
2. **Web server** - To serve the files (Python HTTP server, Apache, Nginx, etc.)
3. **Modern browser** - With WebAssembly and SharedArrayBuffer support

### Disk Image Setup

The disk image is stored in compressed format (`ultima-engines.ext2.gz`) to reduce repository size. Before deploying, you need to decompress it:

```bash
cd web/assets
gunzip -k ultima-engines.ext2.gz
```

This creates the uncompressed `ultima-engines.ext2` (64MB) which is referenced by the JavaScript.

Alternatively, you can rebuild the disk image from scratch:

```bash
cd web/assets
./build-diskimage.sh
```

This will:
1. Copy Exult and Pentagram binaries from `engines/*/build/`
2. Copy SDL3 and system libraries
3. Create a 64MB ext2 filesystem
4. Generate both uncompressed and compressed versions

### Local Testing

Using Python 3:

```bash
cd web
python3 -m http.server 8000 --bind 127.0.0.1
```

Then open `http://127.0.0.1:8000` in your browser.

**Note:** The SharedArrayBuffer headers are set via meta tags in the HTML, but some browsers may require server-side headers for full compatibility.

### Production Deployment

For production deployment with proper headers:

**Nginx:**
```nginx
location /ultimain/ {
    add_header Cross-Origin-Opener-Policy "same-origin";
    add_header Cross-Origin-Embedder-Policy "require-corp";
    # ... other config
}
```

**Apache (.htaccess):**
```apache
Header set Cross-Origin-Opener-Policy "same-origin"
Header set Cross-Origin-Embedder-Policy "require-corp"
```

## Architecture

### CheerpX Integration

The `cheerpx-engine.js` module provides:
- Linux environment initialization
- Disk image mounting (`ultima-engines.ext2`)
- Engine launching (`/usr/bin/run-exult`, `/usr/bin/run-pentagram`)
- Display and input handling

### Game Data Management

The `data-manager.js` module provides:
- IndexedDB storage for game files
- File upload/download
- Game data persistence

### Disk Image Contents

The `ultima-engines.ext2` filesystem contains:

```
/
├── usr/bin/
│   ├── exult            # Exult engine binary (15MB)
│   ├── pentagram        # Pentagram engine binary (3MB)
│   ├── run-exult        # Exult launcher script
│   └── run-pentagram    # Pentagram launcher script
├── lib/
│   ├── libSDL3.so.0.1.6
│   ├── libc.so.6
│   ├── libstdc++.so.6
│   ├── libvorbis.so.0
│   └── ... (other system libraries)
├── lib64/
│   └── ld-linux-x86-64.so.2
└── game/                # Mount point for game data
```

## Browser Compatibility

- ✅ Chrome 92+
- ✅ Edge 92+
- ✅ Firefox 79+ (with security settings)
- ❌ Safari (no SharedArrayBuffer support in cross-origin contexts)

## Troubleshooting

### "SharedArrayBuffer is not defined"
- Check that COOP/COEP headers are set correctly
- Try a different browser
- Check browser console for specific errors

### "CheerpX not loading"
- Check network tab for 404 errors
- Verify CDN is accessible: `https://cxrtnc.leaningtech.com/1.0.6/cx.js`
- Check browser console for script errors

### "Disk image not found"
- Ensure `ultima-engines.ext2` exists in `assets/` directory
- Run `gunzip -k assets/ultima-engines.ext2.gz` to decompress
- Check file permissions

### "Engine fails to start"
- Check browser console for errors
- Verify engine binaries exist in disk image: `e2ls assets/ultima-engines.ext2:/usr/bin`
- Check that game data files are uploaded correctly
