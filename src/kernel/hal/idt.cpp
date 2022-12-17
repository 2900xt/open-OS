#include <stdout.h>

typedef struct{
    word offsetLow;
    word selector;
    byte IST;
    byte flags;
    word offsetMid;
    dword offsetHigh;
    dword zero;
}__attribute__((packed)) IDT64;

typedef struct{
    qword instructionPointer;
    qword codeSegment;
    qword rFlags;
    qword stackPointer;
    qword stackSegment;
}interruptFrame;

IDT64* _idt;
bool isrs[16];

const char* exceptionsList[] = {
    "Divide-by-zero Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Avalible",
    "Double Fault",
    "Coprocessor Segment Overrun", 
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    NULL,
    "x87 Floating Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point Exception",
    "Virtualization Exception",
    "Control Projection Exception",
    NULL, NULL, NULL, NULL, NULL, NULL,
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
};

#define INTERRUPT   0x8E
#define EXCEPTION   0x8F
#define PIC_EOI	    0x20
#define PIC2        0xA0
#define PIC1        0x20


void sendEOI(byte irq)
{
	if(irq >= 8){
		outb(PIC2, PIC_EOI);
    }
 
	outb(PIC1, PIC_EOI);
}

static inline void lidt(void* base, uint16_t size)
{
    struct {
        uint16_t length;
        void*    base;
    } __attribute__((packed)) IDTR = { size, base };
 
    asm ( "lidt %0" : : "m"(IDTR) );  
}

void setIDTEntry(uint8_t vector, void (*isr)(...), byte type){
    if(type == INTERRUPT){
        vector += 32;
    }
    _idt[vector].offsetLow = (uint16_t)(((uint64_t)isr & 0x000000000000ffff));
    _idt[vector].offsetMid = (uint16_t)(((uint64_t)isr & 0x00000000ffff0000) >> 16);
    _idt[vector].offsetHigh = (uint32_t)(((uint64_t)isr & 0xffffffff00000000) >> 32);
    _idt[vector].IST = 0;
    _idt[vector].selector = 0x8;
    _idt[vector].flags = type;
    _idt[vector].zero = 0;
}

//Interrupt Service Routines

__attribute__ ((interrupt)) void isr0(interruptFrame* frame){isrs[0] = true;sendEOI(0);}
__attribute__ ((interrupt)) void isr1(interruptFrame* frame){isrs[1] = true;sendEOI(1);}
__attribute__ ((interrupt)) void isr2(interruptFrame* frame){isrs[2] = true;sendEOI(2);}
__attribute__ ((interrupt)) void isr3(interruptFrame* frame){isrs[3] = true;sendEOI(3);}
__attribute__ ((interrupt)) void isr4(interruptFrame* frame){isrs[4] = true;sendEOI(4);}
__attribute__ ((interrupt)) void isr5(interruptFrame* frame){isrs[5] = true;sendEOI(5);}
__attribute__ ((interrupt)) void isr6(interruptFrame* frame){isrs[6] = true;sendEOI(6);}
__attribute__ ((interrupt)) void isr7(interruptFrame* frame){isrs[7] = true;sendEOI(7);}
__attribute__ ((interrupt)) void isr8(interruptFrame* frame){isrs[8] = true;sendEOI(8);}
__attribute__ ((interrupt)) void isr9(interruptFrame* frame){isrs[9] = true;sendEOI(9);}
__attribute__ ((interrupt)) void isr10(interruptFrame* frame){isrs[10] = true;sendEOI(10);}
__attribute__ ((interrupt)) void isr11(interruptFrame* frame){isrs[11] = true;sendEOI(11);}
__attribute__ ((interrupt)) void isr12(interruptFrame* frame){isrs[12] = true;sendEOI(12);}
__attribute__ ((interrupt)) void isr13(interruptFrame* frame){isrs[13] = true;sendEOI(13);}
__attribute__ ((interrupt)) void isr14(interruptFrame* frame){isrs[14] = true;sendEOI(14);}
__attribute__ ((interrupt)) void isr15(interruptFrame* frame){isrs[15] = true;sendEOI(15);}

//Exceptions without error codes

__attribute__ ((interrupt)) void exc0(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[0], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc1(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[1], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc2(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[2], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc3(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[3], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc4(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[4], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc5(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[5], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc6(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[6], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc7(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[7], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc9(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[9], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc16(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[16], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc18(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[18], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc19(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[19], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc20(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[20], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}
__attribute__ ((interrupt)) void exc28(interruptFrame* frame){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[28], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));}

//Exceptions with error codes

__attribute__ ((interrupt)) void exc8(interruptFrame* frame, qword error){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[8], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));};
__attribute__ ((interrupt)) void exc10(interruptFrame* frame, qword error){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[10], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));};
__attribute__ ((interrupt)) void exc11(interruptFrame* frame, qword error){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[11], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));};
__attribute__ ((interrupt)) void exc12(interruptFrame* frame, qword error){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[12], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));};
__attribute__ ((interrupt)) void exc13(interruptFrame* frame, qword error){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[13], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));};
__attribute__ ((interrupt)) void exc14(interruptFrame* frame, qword error){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[14], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));};
__attribute__ ((interrupt)) void exc17(interruptFrame* frame, qword error){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[17], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));};
__attribute__ ((interrupt)) void exc21(interruptFrame* frame, qword error){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[21], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));};
__attribute__ ((interrupt)) void exc29(interruptFrame* frame, qword error){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[29], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));};
__attribute__ ((interrupt)) void exc30(interruptFrame* frame, qword error){kpanic(fprintf("%s\nIP: %x\nCS: %x\nrFLAGS: %x\nSP: %x\nSS: %x", exceptionsList[30], frame->instructionPointer, frame->codeSegment, frame->rFlags, frame->stackPointer, frame->stackSegment));};


void fillIDT(void){
    asm("cli");
    setIDTEntry(0, isr0, INTERRUPT);
    setIDTEntry(1, isr1, INTERRUPT);
    setIDTEntry(2, isr2, INTERRUPT);
    setIDTEntry(3, isr3, INTERRUPT);
    setIDTEntry(4, isr4, INTERRUPT);
    setIDTEntry(5, isr5, INTERRUPT);
    setIDTEntry(6, isr6, INTERRUPT);
    setIDTEntry(7, isr7, INTERRUPT);
    setIDTEntry(8, isr8, INTERRUPT);
    setIDTEntry(9, isr9, INTERRUPT);
    setIDTEntry(10, isr10, INTERRUPT);
    setIDTEntry(11, isr11, INTERRUPT);
    setIDTEntry(12, isr12, INTERRUPT);
    setIDTEntry(13, isr13, INTERRUPT);
    setIDTEntry(14, isr14, INTERRUPT);
    setIDTEntry(15, isr15, INTERRUPT);


    setIDTEntry(0, exc0, EXCEPTION);
    setIDTEntry(1, exc1, EXCEPTION);
    setIDTEntry(2, exc2, EXCEPTION);
    setIDTEntry(3, exc3, EXCEPTION);
    setIDTEntry(4, exc4, EXCEPTION);
    setIDTEntry(5, exc5, EXCEPTION);
    setIDTEntry(6, exc6, EXCEPTION);
    setIDTEntry(7, exc7, EXCEPTION);
    setIDTEntry(8, exc8, EXCEPTION);
    setIDTEntry(9, exc9, EXCEPTION);
    setIDTEntry(10, exc10, EXCEPTION);
    setIDTEntry(11, exc11, EXCEPTION);
    setIDTEntry(12, exc12, EXCEPTION);
    setIDTEntry(13, exc13, EXCEPTION);
    setIDTEntry(14, exc14, EXCEPTION);
    setIDTEntry(16, exc16, EXCEPTION);
    setIDTEntry(17, exc17, EXCEPTION);
    setIDTEntry(18, exc18, EXCEPTION);
    setIDTEntry(19, exc19, EXCEPTION);
    setIDTEntry(20, exc20, EXCEPTION);
    setIDTEntry(21, exc21, EXCEPTION);
    setIDTEntry(28, exc28, EXCEPTION);
    setIDTEntry(29, exc29, EXCEPTION);
    setIDTEntry(30, exc30, EXCEPTION);
    lidt(_idt, 128*256 - 1);
    asm("sti");
}

