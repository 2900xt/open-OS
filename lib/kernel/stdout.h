#ifndef STDOUT_H
#define STDOUT_H

#include <stdarg.h>
#include <types.h>

extern uint64_t countPIT;

template<typename T> const char* IntegerToString(T);


void stdoutINIT(void);
void putc(char);
void puts(const char*);
void printf(const char*, ...);
void kpanic(const char* msg);
void sleep(uint64_t millis);
char* fprintf(const char* , ...);

static inline void outb(word port, byte val){
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline byte inb(word port){
    byte ret;
    asm volatile("inb %1, %0": "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(word port, word val){
    asm volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

enum class COLORS{
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    ORANGE,
    LGRAY,
    GRAY,
    PURPLE,
    LGREEN,
    LBLUE,
    LRED,
    PINK,
    YELLOW,
    WHITE
};


#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480


#endif