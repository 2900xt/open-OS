#ifndef STDLIB_H
#define STDLIB_H

#include <types.h>

void* kcalloc(qword count, qword size);
void* krealloc(void* address, qword newSize);
void kfree(void* address);
void* kmalloc(qword size);
void heapInit(qword heap_address, qword heap_length);
void memset(void* start, qword value, qword num);
void memcpy(void* destination, const void* source, qword num);
void fillIDT(void);

extern bool isrs[16];

#endif