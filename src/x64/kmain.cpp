#include <io.hpp>
#include <fonts.hpp>

extern "C" void kmain(){
    
    initTTY(&TTY1);
    TTY1.putString("HELLO, WORLD!");
    MagicBreak();
    for (;;);
}