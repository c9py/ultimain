/*
 * Ultima Audio Integration Layer
 * 
 * This header provides a unified interface for audio functionality
 * shared between ScummVM Ultima8 (Pagan) and Exult (Ultima VII).
 * 
 * Based on Pentagram audio code, originally developed for Ultima VIII
 * and later adopted by both projects.
 *
 * Copyright (C) 2005 The Pentagram Team
 * Copyright (C) 2000-2022 The Exult Team
 * Copyright (C) ScummVM Developers
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef ULTIMA_AUDIO_H
#define ULTIMA_AUDIO_H

// Core audio components (Pentagram heritage)
#include "AudioSample.h"
#include "AudioChannel.h"
#include "AudioMixer.h"
#include "RawAudioSample.h"

namespace Ultima {
namespace Audio {

/**
 * Audio configuration structure for both engines
 */
struct AudioConfig {
    int sampleRate = 22050;
    bool stereo = true;
    int bufferSize = 4096;
    int channels = 32;  // Max simultaneous sounds
};

/**
 * Initialize the shared audio system
 * @param config Audio configuration
 * @return true on success
 */
bool initAudio(const AudioConfig& config);

/**
 * Shutdown the audio system
 */
void shutdownAudio();

/**
 * Get the global audio mixer instance
 * @return Pointer to the mixer, or nullptr if not initialized
 */
Pentagram::AudioMixer* getMixer();

} // namespace Audio
} // namespace Ultima

#endif // ULTIMA_AUDIO_H
