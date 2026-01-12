/*
 * XMidi stubs for SDL3 port
 * TODO: Implement proper MIDI support
 */

#include "pent_include.h"
#include "XMidiFile.h"
#include "XMidiEventList.h"

XMidiFile::XMidiFile(IDataSource* source, int pconvert) {
    // Stub implementation
    (void)source;
    (void)pconvert;
}

XMidiFile::~XMidiFile() {
    // Stub implementation
}

XMidiEventList* XMidiFile::GetEventList(unsigned int track) {
    // Stub implementation - return nullptr
    (void)track;
    return nullptr;
}

// MidiDriver stub
#include "MidiDriver.h"

MidiDriver* MidiDriver::createInstance(std::string desired_driver, uint32 sample_rate, bool stereo) {
    // Stub - MIDI not implemented
    (void)desired_driver;
    (void)sample_rate;
    (void)stereo;
    return nullptr;
}
