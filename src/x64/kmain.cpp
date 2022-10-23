#include <io.hpp>
#include <fonts.hpp>
#include <int.hpp>

extern "C" void kmain(){
    x64IDT_INIT();
    heap_init(0x100000,0x100000);       //Sets up heap from 0x100000 - 0x200000 :)
    initTTY(&TTY1);

    TTY1.putString("Welcome to OPEN-OS!");
    for(;;){
        getChar();
        debugPrint("Reached End of Execution!\n\r");
    }


}