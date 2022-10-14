#include <io.hpp>
#include <fonts.hpp>

extern "C" void kmain(){
    TTY1.addChar('A',
    0b00000000,
    0b00011000,
    0b00100100,
    0b01100110,
    0b01111110,
    0b01000010,
    0b01000010,
    0b00000000);
    TTY1.putChar('A');
    TTY1.putChar('A');
    //MagicBreak();
    for (;;);
}