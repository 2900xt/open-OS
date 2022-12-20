#include <types.h>
#include <stdout.h>
#include <stdlib.h>

extern "C" void _start(void) {
    stdoutINIT();
    heapInit(0x100000, 0x100000);
    fillIDT();

    byte* data = (byte*)readSectors(0, 0, 1);

    for(int i = 0; i < 512*2; i++){
        printf("%x ", *data);
        data++;
    }

    for(;;);
}