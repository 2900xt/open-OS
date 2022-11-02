#ifndef IO_HPP
#define IO_HPP

#include <types.hpp>
#define NEW(a) (a*)calloc(sizeof(a));

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

struct MemorySegmentHeader_T{
    uint64_t MemoryLength;
    MemorySegmentHeader_T*  NextSegment;
    MemorySegmentHeader_T* PreviousSegment;
    MemorySegmentHeader_T*  NextFreeSegment;
    MemorySegmentHeader_T* PreviousFreeSegment;
    bool Free;
};

struct MEMMAPENTRY{
    qword BaseAddress;
    qword RegionLength;
    dword RegionType;
    dword ExtendedAttributes;
};

extern "C" byte memoryRegionCount;

void MagicBreak();
char getChar();
void printMemoryInformation(const char* proc);
void debugPrint(string str);
void memset(void* start, qword value, qword num);
void memcpy(void* destination, const void* source, qword num);
void heap_init(qword heap_address, qword heap_length);
void* malloc(qword size);
void CombineFreeSegments(MemorySegmentHeader_T* a, MemorySegmentHeader_T* b);
void free(void* address);
void* calloc(qword size);
void* realloc(void* address, qword newSize);
template<typename T>
const char* IntegerToString(T value);
void debugPrintInteger(qword i);
void strcat(char* destination, char* source);
int strlen(const char* str);
char* getLine();


#endif