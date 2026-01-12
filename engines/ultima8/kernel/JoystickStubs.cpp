/*
 * Joystick stubs for SDL3 port
 * TODO: Implement proper SDL3 joystick support
 */

#include "pent_include.h"
#include "IDataSource.h"
#include "ODataSource.h"
#include "Joystick.h"

// Stub joystick functions
void InitJoystick() {
    // SDL3 joystick support not yet implemented
}

void ShutdownJoystick() {
    // SDL3 joystick support not yet implemented
}

// JoystickCursorProcess implementation
DEFINE_RUNTIME_CLASSTYPE_CODE(JoystickCursorProcess,Process);

JoystickCursorProcess::JoystickCursorProcess() 
    : Process(), js(JOY1), x_axis(0), y_axis(0), ticks(0), accel(0) {
}

JoystickCursorProcess::JoystickCursorProcess(Joystick js_, int x_axis_, int y_axis_)
    : Process(), js(js_), x_axis(x_axis_), y_axis(y_axis_), ticks(0), accel(0) {
}

JoystickCursorProcess::~JoystickCursorProcess() {
}

void JoystickCursorProcess::run() {
    // Stub - joystick cursor movement not implemented
}

bool JoystickCursorProcess::loadData(IDataSource* ids, uint32 version) {
    if (!Process::loadData(ids, version)) return false;
    
    js = static_cast<Joystick>(ids->read1());
    x_axis = static_cast<int>(ids->read2());
    y_axis = static_cast<int>(ids->read2());
    
    return true;
}

void JoystickCursorProcess::saveData(ODataSource* ods) {
    Process::saveData(ods);
    
    ods->write1(static_cast<uint8>(js));
    ods->write2(static_cast<uint16>(x_axis));
    ods->write2(static_cast<uint16>(y_axis));
}
