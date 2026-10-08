# Release Process

This document describes the automated release process for the Ultima Engines Integration project.

## Overview

The project uses GitHub Actions to automatically build and publish releases when a new version tag is pushed to the repository.

## Release Workflow

The release workflow (`.github/workflows/release.yml`) is triggered when a tag matching `v*` is pushed to the repository.

### Build Jobs

The workflow includes the following build jobs:

1. **build-linux**: Builds all Linux components
   - Shared library (`libultima_shared.a`)
   - Exult engine (Ultima VII)
   - Pentagram engine (Ultima VIII)
   - Unified launcher
   - NPC AI module

2. **build-windows**: Builds the same components for Windows x64
   - Runner: `windows-latest` with MSYS2 MinGW-w64 UCRT64
   - Shared library, Exult, Pentagram, unified launcher, and NPC AI module
   - SDL3 and SDL3_ttf built from the same release tags as the Linux job
   - Runtime DLLs bundled next to the executables

3. **build-web-launcher**: Packages the web launcher
   - Web interface files
   - CheerpX integration
   - Disk images

4. **create-release**: Creates the GitHub release
   - Generates release notes
   - Uploads all build artifacts
   - Publishes the release

## Creating a New Release

### 1. Update Version Numbers

Before creating a release, update version numbers in:
- `CMakeLists.txt` (root)
- `engines/exult/CMakeLists.txt`
- `launcher/CMakeLists.txt`
- `README.md` (if applicable)

### 2. Create and Push Tag

```bash
# Create annotated tag
git tag -a v1.2.3 -m "Release version 1.2.3"

# Push tag to GitHub
git push origin v1.2.3
```

### 3. Monitor Workflow

1. Go to the GitHub Actions tab in the repository
2. Watch the "Build and Release" workflow
3. Verify all jobs complete successfully

### 4. Verify Release

1. Go to the Releases page on GitHub
2. Verify the new release is published
3. Check that all artifacts are attached:
   - `ultimain-v*.*.*-linux-x86_64.tar.gz`
   - `ultimain-v*.*.*-windows-x86_64.zip`
   - `ultimain-v*.*.*-web-launcher.zip`

## Release Artifacts

### Linux Binaries (linux-x86_64)

The Linux release includes:

```
linux-x86_64/
├── bin/
│   ├── exult              # Exult engine (Ultima VII)
│   ├── pentagram          # Pentagram engine (Ultima VIII)
│   └── ultima-launcher    # Unified launcher
├── lib/
│   ├── libultima_shared.a # Shared library
│   └── libnpc_ai.a        # NPC AI module
├── tools/
│   └── osm2ultima/        # Map conversion tool
└── docs/
    └── ...                # Documentation
```

### Windows Binaries (windows-x86_64)

The Windows release is a MinGW-w64 UCRT64 build. Executables are in `bin/` with the DLLs they need (SDL3, SDL3_ttf, and the MinGW runtime).

```
windows-x86_64/
├── bin/
│   ├── exult.exe            # Exult engine (Ultima VII)
│   ├── pentagram.exe        # Pentagram engine (Ultima VIII)
│   ├── ultima-launcher.exe  # Unified launcher
│   └── *.dll                # Bundled runtime libraries
├── lib/
│   ├── libultima_shared.a   # Shared library
│   └── libultima_npc_ai.a   # NPC AI module
├── tools/
│   └── osm2ultima/          # Map conversion tool
└── docs/
    └── ...                  # Documentation
```

Extract `ultimain-v*.*.*-windows-x86_64.zip` and run the executables from `windows-x86_64/bin`. No separate SDL install is required.

### Web Launcher

The web launcher package includes:
```
web-launcher/
├── index.html
├── js/
│   ├── cheerpx-engine.js
│   └── data-manager.js
└── assets/
    └── ultima-engines.ext2.gz
```

## Dependencies

The Linux binaries are built with:
- SDL3 `release-3.2.6` and SDL3_ttf `release-3.2.2`
- libvorbis, libogg
- zlib, libpng
- System libraries (dynamically linked)

The Windows binaries are built with MSYS2 MinGW-w64 UCRT64 and:
- SDL3 `release-3.2.6` and SDL3_ttf `release-3.2.2` (DLLs bundled in the zip)
- libvorbis, libogg, zlib, libpng, and freetype from the UCRT64 packages
- The MinGW runtime DLLs required by those executables

Users will need:
- Original Ultima VII/VIII game data files
- Python 3.x (for OSM2Ultima tool)
- Modern web browser with WebAssembly support (for web launcher)

## Versioning Scheme

The project uses [Semantic Versioning](https://semver.org/):

- **MAJOR** version: Incompatible API changes
- **MINOR** version: New functionality (backward-compatible)
- **PATCH** version: Bug fixes (backward-compatible)

Example: `v1.2.3`
- 1 = Major version
- 2 = Minor version
- 3 = Patch version

## Release Checklist

Before creating a release, ensure:

- [ ] All CI tests pass on main branch
- [ ] Version numbers updated in CMakeLists.txt files
- [ ] CHANGELOG updated (if exists)
- [ ] Documentation is up-to-date
- [ ] No known critical bugs
- [ ] Tag follows versioning scheme (v*.*.*)

## Troubleshooting

### Release workflow failed

1. Check the GitHub Actions logs for error messages
2. Verify all dependencies are available
3. Ensure tag format is correct (`v*`)
4. Check for any build system changes that need workflow updates

### Artifact missing from release

1. Check if the corresponding build job succeeded
2. Verify artifact upload step completed
3. Check artifact paths in workflow file

### Release not created

1. Verify the `create-release` job has proper permissions
2. Check `GITHUB_TOKEN` permissions in workflow
3. Ensure tag was pushed correctly

## Manual Release Process (Fallback)

If automated release fails, you can create a manual release:

1. Build artifacts locally using the same steps as CI
2. Create release manually on GitHub
3. Upload artifacts through GitHub UI
4. Copy release notes from workflow file

## Future Improvements

Potential enhancements to the release process:

- [ ] Add macOS builds
- [x] Add Windows builds
- [ ] Add AppImage or Flatpak packaging
- [ ] Automated changelog generation
- [ ] Draft release option for testing
- [ ] Pre-release tags (alpha, beta, rc)
