#include <types.hpp>
#include <kernel/stdout.h>

extern "C" void _start(void) {
    stdoutINIT();
    putChar('h');
}