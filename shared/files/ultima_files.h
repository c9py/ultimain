/*
 * Ultima File Format Integration Layer
 * 
 * This header provides a unified interface for file format handling
 * shared between ScummVM Ultima8 (Pagan) and Exult (Ultima VII).
 * 
 * Supports:
 * - Flex archives (U7/U8 data files)
 * - Shape files (graphics)
 * - Palette files
 *
 * Copyright (C) 2000-2022 The Exult Team
 * Copyright (C) ScummVM Developers
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef ULTIMA_FILES_H
#define ULTIMA_FILES_H

#include "Flex.h"
#include "U7file.h"
#include "databuf.h"
#include "utils.h"

namespace Ultima {
namespace Files {

/**
 * Game type enumeration for file format variations
 */
enum class GameType {
    ULTIMA_VII_BG,      // Black Gate
    ULTIMA_VII_SI,      // Serpent Isle
    ULTIMA_VIII,        // Pagan
    CRUSADER_REMORSE,   // No Remorse
    CRUSADER_REGRET     // No Regret
};

/**
 * File format version information
 */
struct FormatInfo {
    GameType game;
    int flexVersion;
    bool hasExtendedShapes;
};

/**
 * Get format information for a game type
 */
FormatInfo getFormatInfo(GameType game);

/**
 * Detect game type from data files
 * @param dataPath Path to game data directory
 * @return Detected game type
 */
GameType detectGameType(const std::string& dataPath);

/**
 * Open a Flex archive with automatic version detection
 * @param filename Path to the Flex file
 * @return Unique pointer to Flex object, or nullptr on failure
 */
std::unique_ptr<Flex> openFlex(const std::string& filename);

} // namespace Files
} // namespace Ultima

#endif // ULTIMA_FILES_H
