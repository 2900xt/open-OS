#include <int.hpp>
#include <types.hpp>
#include <io.hpp>

typedef struct{
    word offsetLow;
    word selector;
    byte IST;
    byte flags;
    word offsetMid;
    dword offsetHigh;
    dword zero;
}__attribute__((packed)) IDT64;

IDT64 _idt[256];

extern "C" qword common_ISR;

void maskAllInterrupts(){
    for(int i = 32; i < 48 ; i ++){
        IRQ_set_mask(i);
    }
    debugPrint("All Interrupts Masked\n\r");
}

void enableAllInterrupts(){
    for(int i = 32; i < 48 ; i ++){
        IRQ_clear_mask(i);
    }
    debugPrint("All Interrupts Enabled\n\r");
}

void maskExceptions(){
    for(int i = 0; i  < 32 ; i++){
        IRQ_set_mask(i);
    }
    debugPrint("All Exceptions Disabled\n\r");
}

void enableExceptions(){
    for(int i = 0; i  < 32 ; i++){
        IRQ_clear_mask(i);
    }
    debugPrint("All Exceptions Enabled\n\r");
}

void x64IDT_INIT(){

    for(int i = 0 ; i < 32 ; i++){
        _idt[i].zero = 0;
        _idt[i].offsetLow = (word)(((qword)&common_ISR & 0x000000000000FFFF));
        _idt[i].offsetMid = (word)(((qword)&common_ISR & 0x00000000FFFF0000) >> 16);
        _idt[i].offsetHigh = (word)(((qword)&common_ISR & 0xFFFFFFFF00000000) >> 32);
        _idt[i].IST = 0;
        _idt[i].selector = 0x08;
        _idt[i].flags = 0x8f;
    }

    for(int i = 32 ; i < 255 ; i++){
        _idt[i].zero = 0;
        _idt[i].offsetLow = (word)(((qword)&common_ISR & 0x000000000000FFFF));
        _idt[i].offsetMid = (word)(((qword)&common_ISR & 0x00000000FFFF0000) >> 16);
        _idt[i].offsetHigh = (word)(((qword)&common_ISR & 0xFFFFFFFF00000000) >> 32);
        _idt[i].IST = 0;
        _idt[i].selector = 0x08;
        _idt[i].flags = 0x8e;
    }

    debugPrint("IDT Loaded Sucessfully\n\r");

    maskAllInterrupts();

    maskExceptions();

    __STI;

    return;
}

