#include <types.hpp>
#include <fonts.hpp>
VGA TTY1;

word Screen_x = 0;
word Screen_y = 0;
byte* ScreenBuffer = (byte*)0xA0000;