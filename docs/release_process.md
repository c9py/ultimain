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

2. **build-web-launcher**: Packages the web launcher
   - Web interface files
   - CheerpX integration
   - Disk images

3. **create-release**: Creates the GitHub release
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
   - `ultimain-v*.*.* -linux-x86_64.tar.gz`
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
- SDL3 3.2.0 (statically linked)
- libvorbis, libogg
- zlib, libpng
- System libraries (dynamically linked)

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
- [ ] Add Windows builds
- [ ] Add AppImage or Flatpak packaging
- [ ] Automated changelog generation
- [ ] Draft release option for testing
- [ ] Pre-release tags (alpha, beta, rc)
