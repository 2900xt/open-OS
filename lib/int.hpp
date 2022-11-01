#pragma once
#include <types.hpp>
void PIC_sendEOI(byte irq);
void PIC_remap(int offset1, int offset2);
void IRQ_set_mask(byte IRQline);
void IRQ_clear_mask(byte IRQline);

void x64IDT_INIT();


extern char lastKey;
extern bool keyPressed;

extern bool irq6;
#define __STI __asm__("sti")

#define __CLI __asm__("cli")