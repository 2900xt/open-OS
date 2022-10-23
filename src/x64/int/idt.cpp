#include <int.hpp>
#include <types.hpp>
#include <io.hpp>
#include <fonts.hpp>

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

extern "C" void common_ISR();
extern "C" void ISR1();
byte isp = 127;
char inputStack[128];

const char ScanCodeLookupTable[] ={
    0, 0, '1', '2',
    '3', '4', '5', '6',
    '7', '8', '9', '0',
    '-', '=', 0, 0,
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i',
    'o', 'p', '[', ']',
    0, 0, 'a', 's',
    'd', 'f', 'g', 'h',
    'j', 'k', 'l', ';',
    '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',',
    '.', '/', 0, '*',
    0, ' '
  };

char popInputStack(){
    return inputStack[isp--];
}

void pushInputStack(char c){
    inputStack[isp++] = c;
}

void resetInputStack(){
    inputStack[isp] = 0;
    while(++isp != 127)
        inputStack[isp] = 0;
}

static inline void lidt(void* base, uint16_t size)
{
    struct {
        uint16_t length;
        void*    base;
    } __attribute__((packed)) IDTR = { size, base };
 
    asm ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

void maskAllInterrupts(){
    for(int i = 0; i < 16 ; i ++){
        IRQ_set_mask(i);
    }
    debugPrint("All Interrupts Masked\n\r");
}

void enableAllInterrupts(){
    for(int i = 0; i < 16 ; i ++){
        IRQ_clear_mask(i);
    }
    debugPrint("All Interrupts Enabled\n\r");
}


void IDTsetEntry(byte IRQ,qword* ISR, word selector, byte flags, byte IST){
    _idt[IRQ].zero = 0;
    _idt[IRQ].offsetLow = (word)(((qword)ISR & 0x000000000000FFFF));
    _idt[IRQ].offsetMid = (word)(((qword)ISR & 0x00000000FFFF0000) >> 16);
    _idt[IRQ].offsetHigh = (word)(((qword)ISR & 0xFFFFFFFF00000000) >> 32);
    _idt[IRQ].IST = IST;
    _idt[IRQ].selector = flags;
    _idt[IRQ].flags = flags;
}

extern "C" void keyboardHandler(){
    byte character = ScanCodeLookupTable[inb(0x60)];
    pushInputStack(character);
    PIC_sendEOI(1);
}

void x64IDT_INIT(){

    for(int i = 0 ; i < 32 ; i++){
        _idt[i].zero = 0;
        _idt[i].offsetLow = (word)(((qword)&common_ISR & 0x000000000000FFFF));
        _idt[i].offsetMid = (word)(((qword)&common_ISR & 0x00000000FFFF0000) >> 16);
        _idt[i].offsetHigh = (word)(((qword)&common_ISR & 0xFFFFFFFF00000000) >> 32);
        _idt[i].IST = 0;
        _idt[i].selector = 0x8;
        _idt[i].flags = 0x8f;
    }

    for(int i = 32 ; i < 255 ; i++){
        _idt[i].zero = 0;
        _idt[i].offsetLow = (word)(((qword)&ISR1 & 0x000000000000FFFF));
        _idt[i].offsetMid = (word)(((qword)&ISR1 & 0x00000000FFFF0000) >> 16);
        _idt[i].offsetHigh = (word)(((qword)&ISR1 & 0xFFFFFFFF00000000) >> 32);
        _idt[i].IST = 0;
        _idt[i].selector = 0x8;
        _idt[i].flags = 0x8e;
    }

    PIC_remap(32,40);

    maskAllInterrupts();

    IRQ_clear_mask(1);

    lidt(_idt, 128*256 - 1);

    __STI;

    return;
}

