/*
 * Tool process stubs for Pentagram
 * These are normally standalone tools but referenced by ConApp
 */

#include "pent_include.h"
#include "DisasmProcess.h"
#include "CompileProcess.h"

// DisasmProcess stub implementation
DEFINE_RUNTIME_CLASSTYPE_CODE(DisasmProcess,Process);

DisasmProcess::DisasmProcess() : Process(), termCounter(0) {
}

DisasmProcess::~DisasmProcess() {
}

void DisasmProcess::run() {
    // Stub - disassembler not implemented in main engine
    terminate();
}

// CompileProcess stub implementation
DEFINE_RUNTIME_CLASSTYPE_CODE(CompileProcess,Process);

CompileProcess::CompileProcess(FileSystem* fs) : Process(), termCounter(0), cu(nullptr) {
    (void)fs;
}

CompileProcess::~CompileProcess() {
}

void CompileProcess::run() {
    // Stub - compiler not implemented in main engine
    terminate();
}
