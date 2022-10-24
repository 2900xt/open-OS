#include <io.hpp>
#include <fonts.hpp>
#include <int.hpp>
#include <gsl/string.hpp>
#include <fs/tfs.hpp>


void * __gxx_personality_v0=0;
void * _Unwind_Resume =0;

extern "C" void kmain(){
    x64IDT_INIT();
    heap_init(0x100000,0x100000);       //Sets up heap from 0x100000 - 0x200000 :)
    initTTY(&TTY1);
    
    FileObjectDescriptor* OPENOS_ROOT = NEW(FileObjectDescriptor);

    ROOT_INIT(OPENOS_ROOT);

    TTY1.putString("Loading...");

    for(;;){
        getChar();
    }


}