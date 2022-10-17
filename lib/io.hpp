#pragma once
#include <types.hpp>

static inline void outb(word port, byte val){
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline byte inb(word port){
    byte ret;
    asm volatile("inb %1, %0": "=a"(ret) : "Nd"(port));
    return ret;
}

static inline dword CPUID(dword code, dword where[4]) {
  asm volatile("cpuid":"=a"(*where),"=b"(*(where+1)),
               "=c"(*(where+2)),"=d"(*(where+3)):"a"(code));
  return (dword)where[0];
}

void MagicBreak();

void debugPrint(string str);