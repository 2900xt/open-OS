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

extern "C" qword common_ISR;
extern "C" qword ISR1;
extern "C" qword ISR6;
extern "C" qword ISR0;
extern "C" void enablePIT();


 extern "C" qword exc0;
 extern "C" qword exc1;
 extern "C" qword exc2;
 extern "C" qword exc3;
 extern "C" qword exc4;
 extern "C" qword exc5;
 extern "C" qword exc6;
 extern "C" qword exc7;
 extern "C" qword exc8;
 extern "C" qword exc9;
 extern "C" qword exc10;
 extern "C" qword exc11;
 extern "C" qword exc12;
 extern "C" qword exc13;
 extern "C" qword exc14;
 extern "C" qword exc16;
 extern "C" qword exc17;
 extern "C" qword exc18;
 extern "C" qword exc19;
 extern "C" qword exc20;
 extern "C" qword exc21;
 extern "C" qword exc28;
 extern "C" qword exc29;
 extern "C" qword exc30;

int seconds = 0;
int count = 0;
int reload = 650;

const char ScanCodeLookupTable[] ={
    0, 0, '1', '2',
    '3', '4', '5', '6',
    '7', '8', '9', '0',
    '-', '=', '\r', '\t',
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i',
    'o', 'p', '[', ']',
    '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h',
    'j', 'k', 'l', ';',
    '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',',
    '.', '/', 0, '*',
    0, ' '
};

const char CapsLookUpTable[] = {
    0, 0, '!', '@',
    '#', '$', '%', '^',
    '&', '*', '(', ')',
    '_', '+', '\r', '\t',
    'Q', 'W', 'E', 'R',
    'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}',
    '\n', 0, 'A', 'S',
    'D', 'F', 'G', 'H',
    'J', 'K', 'L', ':',
    '"', '~', 0, '|',
    'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<',
    '>', '?', 0, '*',
    0, ' '
};


byte lastKey = 0;
bool keyPressed = false;
bool shiftBit = false;


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

void IDT_ENABLEINT(int interrupt, uint64_t* isr, uint8_t ist = 0, uint8_t flags = 0x8e, uint16_t selector = 0x08){
        _idt[interrupt+32].zero = 0;
        _idt[interrupt+32].offsetLow = (uint16_t)(((uint64_t)isr & 0x000000000000ffff));
        _idt[interrupt+32].offsetMid = (uint16_t)(((uint64_t)isr & 0x00000000ffff0000) >> 16);
        _idt[interrupt+32].offsetHigh = (uint32_t)(((uint64_t)isr & 0xffffffff00000000) >> 32);
        _idt[interrupt+32].IST = ist;
        _idt[interrupt+32].selector = selector;
        _idt[interrupt+32].flags = flags;
}

void IDT_ENABLEEXC(int interrupt, uint64_t* isr, uint8_t ist = 0, uint8_t flags = 0x8f, uint16_t selector = 0x08){
        _idt[interrupt].zero = 0;
        _idt[interrupt].offsetLow = (uint16_t)(((uint64_t)isr & 0x000000000000ffff));
        _idt[interrupt].offsetMid = (uint16_t)(((uint64_t)isr & 0x00000000ffff0000) >> 16);
        _idt[interrupt].offsetHigh = (uint32_t)(((uint64_t)isr & 0xffffffff00000000) >> 32);
        _idt[interrupt].IST = ist;
        _idt[interrupt].selector = selector;
        _idt[interrupt].flags = flags;
}

extern "C" void keyboardHandler(){
    byte code = inb(0x60);
    if(code == 0x36 || code == 0x2A){
        shiftBit = true;
    }
    else if(code == 0xAA || code == 0xB6){
        shiftBit = false;
    }
    else if(code < 59){
        lastKey = (!shiftBit ? ScanCodeLookupTable[code] : CapsLookUpTable[code]);
        keyPressed = true;
    }
    
    PIC_sendEOI(1);
}
bool irq6 = false;

extern "C" void floppyHandler(){
    if(irq6)
        MagicBreak();
    irq6 = true;
    PIC_sendEOI(6);
}
bool cursorflashed = false;
extern "C" void PITHandler(){
    count++;
    if(count == reload && cursorflashed){
        count = 0;
        seconds++;
        TTY1.putCursor();
        cursorflashed = false;
    }
    else if(count == reload){
        count = 0;
        seconds++;
        TTY1.removeCursor();
        cursorflashed = true;
    }
    PIC_sendEOI(0);
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
        _idt[i].offsetLow = (word)(((qword)&common_ISR & 0x000000000000FFFF));
        _idt[i].offsetMid = (word)(((qword)&common_ISR & 0x00000000FFFF0000) >> 16);
        _idt[i].offsetHigh = (word)(((qword)&common_ISR & 0xFFFFFFFF00000000) >> 32);
        _idt[i].IST = 0;
        _idt[i].selector = 0x8;
        _idt[i].flags = 0x8e;
    }

    PIC_remap(32,40);

    maskAllInterrupts();

    IDT_ENABLEINT(1,&ISR1);
    IDT_ENABLEINT(6,&ISR6);
    IDT_ENABLEINT(0,&ISR0);

    IRQ_clear_mask(1);      //PS-2
    IRQ_clear_mask(6);      //FDC
    IRQ_clear_mask(0);

    IDT_ENABLEEXC(0,&exc0);
    IDT_ENABLEEXC(1,&exc1);
    IDT_ENABLEEXC(2,&exc2);
    IDT_ENABLEEXC(3,&exc3);
    IDT_ENABLEEXC(4,&exc4);
    IDT_ENABLEEXC(5,&exc5);
    IDT_ENABLEEXC(6,&exc6);
    IDT_ENABLEEXC(7,&exc7);
    IDT_ENABLEEXC(8,&exc8);
    IDT_ENABLEEXC(9,&exc9);
    IDT_ENABLEEXC(10,&exc10);
    IDT_ENABLEEXC(11,&exc11);
    IDT_ENABLEEXC(12,&exc12);
    IDT_ENABLEEXC(13,&exc13);
    IDT_ENABLEEXC(14,&exc14);
    IDT_ENABLEEXC(16,&exc16);
    IDT_ENABLEEXC(17,&exc17);
    IDT_ENABLEEXC(18,&exc18);
    IDT_ENABLEEXC(19,&exc19);
    IDT_ENABLEEXC(20,&exc20);
    IDT_ENABLEEXC(21,&exc21);
    IDT_ENABLEEXC(28,&exc28);
    IDT_ENABLEEXC(29,&exc29);
    IDT_ENABLEEXC(30,&exc30);

    lidt(_idt, 128*256 - 1);
    __STI;

    return;
}

