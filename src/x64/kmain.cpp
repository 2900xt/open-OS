#include <io.hpp>
#include <fonts.hpp>
#include <int.hpp>
#include <gsl/string.hpp>
#include <fs/tfs.hpp>
#include <proc.hpp>

void * __gxx_personality_v0=0;
void * _Unwind_Resume =0;

int init(PROCESS_T* procPtr){
    
    x64IDT_INIT();
    heap_init(0x100000,0x100000);       //Sets up heap from 0x100000 - 0x1F0000
    initTTY(&TTY1);
    
    FileObjectDescriptor* OPENOS_ROOT = NEW(FileObjectDescriptor);

    initializeRoot(OPENOS_ROOT);

    TTY1.putString("INIT : Booting from 0x7c00...\n");

    FDCInitialize();

    for(;;){
        TTY1.putChar(getChar());
    }
}

extern "C" void setup64(){

    PROCESS_T INIT = {(qword)init,nullptr , nullptr , PROCESS_PERMISSIONS::ROOT};
    runProc(&INIT);
}