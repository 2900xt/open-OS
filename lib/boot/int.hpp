#pragma once
#include <types.hpp>
void PIC_sendEOI(byte irq);
void PIC_remap(int offset1, int offset2);
void IRQ_set_mask(byte IRQline);
void IRQ_clear_mask(byte IRQline);
void x64IDT_INIT();

typedef struct{
    word offsetLow;
    word selector;
    byte IST;
    byte flags;
    word offsetMid;
    dword offsetHigh;
    dword zero;
}__attribute__((packed)) IDT64;

extern IDT64 _idt[256];
extern bool irq6;

#define __STI __asm__("sti")

#define __CLI __asm__("cli")