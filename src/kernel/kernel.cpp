#include <types.h>
#include <stdout.h>
#include <stdlib.h>

extern "C" void _start(void) {
    stdoutINIT();
    heapInit(0x100000, 0x100000);
    fillIDT();

    for(;;);
}