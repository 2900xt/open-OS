#include <io.hpp>


void MagicBreak(){
    asm volatile("xchg %bx,%bx");
}

void debugPrint(string str){
    while(*str)
        outb(0xE9, *str++);
}
