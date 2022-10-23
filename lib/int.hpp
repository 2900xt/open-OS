#pragma once
#include <types.hpp>
void PIC_sendEOI(byte irq);
void PIC_remap(int offset1, int offset2);
void IRQ_set_mask(byte IRQline);
void IRQ_clear_mask(byte IRQline);

char popInputStack();

void pushInputStack(char c);

void resetInputStack();

void x64IDT_INIT();

extern byte isp;
extern char inputStack[128];

#define __STI __asm__("sti")

#define __CLI __asm__("cli")