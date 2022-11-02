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

bool enterPressed = false;
char lastKey = 0;
bool keyPressed = false;
double second = 0.0;
double reload;

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

void set_pit_count(unsigned count) {
 
	outb(0x40,count&0xFF);		// Low byte
	outb(0x40,(count&0xFF00)>>8);	// High byte

    reload = count / (3579545 / 3) * 100;
	return;
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

extern "C" void keyboardHandler(){
    byte code = inb(0x60);
    if(code < 59){
        lastKey = ScanCodeLookupTable[code];
        if(lastKey != 0)
            keyPressed = true;
    }
    if(code == 0x1C){
        enterPressed = true;
        keyPressed = true;
        }
    else
        enterPressed = false;
    PIC_sendEOI(1);
}

bool irq6 = false;

extern "C" void floppyHandler(){
    if(irq6)
        MagicBreak();
    irq6 = true;
    PIC_sendEOI(6);
}

extern "C" void PITHandler(){
    second += reload;
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

    IRQ_clear_mask(1);      //PS-2
    IRQ_clear_mask(6);      //FDC


    lidt(_idt, 128*256 - 1);

    __STI;

    return;
}

