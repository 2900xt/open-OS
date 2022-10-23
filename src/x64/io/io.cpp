#include <io.hpp>
#include <int.hpp>


void MagicBreak(){
    asm volatile("xchg %bx,%bx");
}

void debugPrint(string str){
    while(*str)
        outb(0xE9, *str++);
}

char getChar(){
    resetInputStack();
    while(isp == 127);
    return popInputStack();
}
