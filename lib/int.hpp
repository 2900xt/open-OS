#pragma once
#include <types.hpp>
void PIC_sendEOI(byte irq);
void PIC_remap(int offset1, int offset2);
void IRQ_set_mask(byte IRQline);
void IRQ_clear_mask(byte IRQline);

void x64IDT_INIT();


enum class keys
{
    escape = 127 + 0x01,
    backspace = 127 + 0x0E,
    enter = 127 + 0x1C,
    lctrl = 127 + 0x1D,
    lshift = 127 + 0x2A,
    rshift = 127 + 0x36,
    lalt = 127 + 0x38,
    caps = 127 + 0x3A,

    rshift_release = 127 + 0xB6,
    lshift_release = 127 + 0xAA
};


extern byte lastKey;
extern bool keyPressed;

extern bool irq6;
#define __STI __asm__("sti")

#define __CLI __asm__("cli")