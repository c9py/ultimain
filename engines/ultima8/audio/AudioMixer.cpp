/*
Copyright (C) 2005 The Pentagram team
Copyright (C) 2025 SDL3 port

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "pent_include.h"
#include "AudioMixer.h"
#include "SettingManager.h"
#include "Kernel.h"

#include "AudioProcess.h"
#include "MusicProcess.h"
#include "AudioChannel.h"

#include "MidiDriver.h"

#include <SDL3/SDL.h>

namespace Pentagram {

AudioMixer *AudioMixer::the_audio_mixer = 0;

// SDL3 audio stream callback
static void SDLCALL sdl3AudioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
	AudioMixer *mixer = static_cast<AudioMixer*>(userdata);
	if (mixer && additional_amount > 0) {
		// Allocate temporary buffer
		Uint8 *buffer = new Uint8[additional_amount];
		memset(buffer, 0, additional_amount);
		
		// Mix audio into buffer
		mixer->MixAudio(reinterpret_cast<sint16*>(buffer), additional_amount / 2);
		
		// Put data into stream
		SDL_PutAudioStreamData(stream, buffer, additional_amount);
		delete[] buffer;
	}
	(void)total_amount;
}

AudioMixer::AudioMixer(int sample_rate_, bool stereo_, int num_channels_) : 
		audio_ok(false), 
		sample_rate(sample_rate_), stereo(stereo_),
		midi_driver(0), midi_volume(255),
		num_channels(num_channels_), channels(0),
		audio_device(0), audio_stream(nullptr)
{
	the_audio_mixer = this;

	con.Print(MM_INFO, "Creating AudioMixer...\n");

	// Initialize SDL Audio subsystem
	if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
		con.Printf(MM_MAJOR_ERR, "Failed to initialize SDL audio: %s\n", SDL_GetError());
		return;
	}

	// SDL3 audio spec
	SDL_AudioSpec spec;
	spec.format = SDL_AUDIO_S16;
	spec.freq = sample_rate_;
	spec.channels = stereo_ ? 2 : 1;

#ifdef UNDER_CE
	spec.freq = 11025;
	spec.channels = 1;
#endif

	// Create audio stream with callback
	audio_stream = SDL_OpenAudioDeviceStream(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		&spec,
		sdl3AudioCallback,
		this
	);

	if (audio_stream) {
		audio_ok = true;
		
		// Get the actual device ID
		audio_device = SDL_GetAudioStreamDevice(audio_stream);
		
		pout << "Audio opened using format: " << spec.freq << " Hz " 
		     << (int)spec.channels << " Channels" << std::endl;

		// Lock the audio
		Lock();

		sample_rate = spec.freq;
		stereo = spec.channels == 2;

		channels = new AudioChannel*[num_channels];
		for (int i = 0; i < num_channels; i++)
			channels[i] = new AudioChannel(sample_rate, stereo);

		// Unlock it
		Unlock();

		// Start playback
		SDL_ResumeAudioDevice(audio_device);
	} else {
		con.Printf(MM_MAJOR_ERR, "Failed to open audio device: %s\n", SDL_GetError());
	}
}

void AudioMixer::createProcesses()
{
	Kernel *kernel = Kernel::get_instance();

	// Create the Audio Process
	kernel->addProcess(new AudioProcess());

	// Create the Music Process
	kernel->addProcess(new MusicProcess(midi_driver));
}

AudioMixer::~AudioMixer(void)
{
	con.Print(MM_INFO, "Destroying AudioMixer...\n");

	closeMidiOutput();

	if (audio_stream) {
		SDL_DestroyAudioStream(audio_stream);
		audio_stream = nullptr;
	}

	if (channels) {
		for (int i = 0; i < num_channels; i++)
			delete channels[i];
		delete[] channels;
		channels = 0;
	}

	the_audio_mixer = 0;
}

void AudioMixer::Lock()
{
	if (audio_stream) {
		SDL_LockAudioStream(audio_stream);
	}
}

void AudioMixer::Unlock()
{
	if (audio_stream) {
		SDL_UnlockAudioStream(audio_stream);
	}
}

void AudioMixer::reset()
{
	Lock();
	if (channels) {
		for (int i = 0; i < num_channels; i++)
			channels[i]->stop();
	}
	Unlock();
}

void AudioMixer::MixAudio(sint16 *stream, uint32 bytes)
{
	if (!channels) return;

	memset(stream, 0, bytes * 2);

	for (int i = 0; i < num_channels; i++) {
		if (channels[i]->isPlaying()) {
			channels[i]->resampleAndMix(stream, bytes);
		}
	}
}

int AudioMixer::playSample(AudioSample *sample, int loop, int priority, bool paused, uint32 pitch_shift, int lvol, int rvol)
{
	if (!audio_ok) return -1;

	int lowest = -1;
	int lowprior = 65536;

	Lock();

	int i;
	for (i = 0; i < num_channels; i++) {
		if (!channels[i]->isPlaying()) {
			lowest = i;
			break;
		}
		else if (channels[i]->getPriority() < lowprior) {
			lowprior = channels[i]->getPriority();
			lowest = i;
		}
	}

	if (i != num_channels || lowprior < priority) {
		if (lowest != -1) {
			channels[lowest]->playSample(sample, loop, priority, paused, pitch_shift, lvol, rvol);
		}
	}
	else {
		lowest = -1;
	}

	Unlock();

	return lowest;
}

bool AudioMixer::isPlaying(int chan)
{
	if (chan < 0 || chan >= num_channels) return false;

	Lock();
	bool playing = channels[chan]->isPlaying();
	Unlock();

	return playing;
}

void AudioMixer::stopSample(int chan)
{
	if (chan < 0 || chan >= num_channels) return;

	Lock();
	channels[chan]->stop();
	Unlock();
}

void AudioMixer::setPaused(int chan, bool paused)
{
	if (chan < 0 || chan >= num_channels) return;

	Lock();
	channels[chan]->setPaused(paused);
	Unlock();
}

bool AudioMixer::isPaused(int chan)
{
	if (chan < 0 || chan >= num_channels) return false;

	Lock();
	bool ret = channels[chan]->isPaused();
	Unlock();

	return ret;
}

void AudioMixer::setVolume(int chan, int lvol, int rvol)
{
	if (chan < 0 || chan >= num_channels) return;

	Lock();
	channels[chan]->setVolume(lvol, rvol);
	Unlock();
}

void AudioMixer::getVolume(int chan, int &lvol, int &rvol)
{
	if (chan < 0 || chan >= num_channels) return;

	Lock();
	channels[chan]->getVolume(lvol, rvol);
	Unlock();
}

void AudioMixer::openMidiOutput()
{
	SettingManager *settingman = SettingManager::get_instance();
	std::string driver;
	settingman->get("midi_driver", driver);
	
	midi_driver = MidiDriver::createInstance(driver, sample_rate, stereo);
}

void AudioMixer::closeMidiOutput()
{
	delete midi_driver;
	midi_driver = 0;
}

void AudioMixer::setMidiVolume(int vol)
{
	midi_volume = vol;
	if (midi_driver)
		midi_driver->setGlobalVolume(vol);
}

} // namespace Pentagram
