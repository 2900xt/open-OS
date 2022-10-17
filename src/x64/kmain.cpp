#include <io.hpp>
#include <fonts.hpp>
#include <int.hpp>

extern "C" void kmain(){
    x64IDT_INIT();
    initTTY(&TTY1);
    TTY1.putString("HELLO WORLD!");

    while(true);

}