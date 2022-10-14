#include <io.hpp>

static inline void outb(word port, byte val){
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline byte inb(word port){
    byte ret;
    asm volatile("inb %1, %0": "=a"(ret) : "Nd"(port));
    return ret;
}

void MagicBreak(){
    asm volatile("xchg %bx,%bx");
}

void debugPrint(string str){
    while(*str)
        outb(0xE9, *str++);
}
