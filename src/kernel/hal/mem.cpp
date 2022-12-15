#include <stdlib.h>
#include <stdout.h>
#include <types.h>

struct MemorySegmentHeader_T{
    qword MemoryLength;
    MemorySegmentHeader_T*  NextSegment;
    MemorySegmentHeader_T* PreviousSegment;
    MemorySegmentHeader_T*  NextFreeSegment;
    MemorySegmentHeader_T* PreviousFreeSegment;
    bool Free;
};

MemorySegmentHeader_T* firstFreeMemorySegment;

void memset(void* start, qword value, qword num) {

	if (num <= 8) {
		byte* valPtr = (byte*)&value;
		for (byte* ptr = (byte*)start; ptr < (byte*)((qword)start + num); ptr++) {
			*ptr = *valPtr;
			valPtr++;
		}

		return;
	}

	qword proceedingBytes = num % 8;
	qword newnum = num - proceedingBytes;

	for (qword* ptr = (qword*)start; ptr < (qword*)((qword)start + newnum); ptr++) {
		*ptr = value;
	}

	byte* valPtr = (byte*)&value;
	for (byte* ptr = (byte*)((qword)start + newnum); ptr < (byte*)((qword)start + num); ptr++)
	{
		*ptr = *valPtr;
		valPtr++;
	}
}

void memcpy(void* destination, const void* source, qword num) {
	if (num <= 8) {
		byte* valPtr = (byte*)source;
		for (byte* ptr = (byte*)destination; ptr < (byte*)((qword)destination + num); ptr++) {
			*ptr = *valPtr;
			valPtr++;
		}

		return;
	}

	qword proceedingBytes = num % 8;
	qword newnum = num - proceedingBytes;
	qword* srcptr = (qword*)source;

	for (qword* destptr = (qword*)destination; destptr < (qword*)((qword)destination + newnum); destptr++) {
		*destptr = *srcptr;
		srcptr++;
	}

	byte* srcptr8 = (byte*)((qword)source + newnum);
	for (byte* destptr8 = (byte*)((qword)destination + newnum); destptr8 < (byte*)((qword)destination + num); destptr8++)
	{
		*destptr8 = *srcptr8;
		srcptr8++;
	}

}


void heapInit(qword heap_address, qword heap_length){
    firstFreeMemorySegment = (MemorySegmentHeader_T*)heap_address;
    firstFreeMemorySegment->MemoryLength = heap_length - sizeof(MemorySegmentHeader_T);
    firstFreeMemorySegment->NextSegment=0;
    firstFreeMemorySegment->PreviousSegment=0;
    firstFreeMemorySegment->NextFreeSegment=0;
    firstFreeMemorySegment->PreviousFreeSegment=0;
    firstFreeMemorySegment->Free = true;
}


void* kmalloc(qword size){
    qword remainder = size % 8;
    size -= remainder;
    if (remainder != 0)
        size += 8;
    
    MemorySegmentHeader_T* currentMemorySegment = firstFreeMemorySegment;

    while (true){
        if(currentMemorySegment->MemoryLength > size + sizeof(MemorySegmentHeader_T)){

            if (currentMemorySegment->MemoryLength != size){
                MemorySegmentHeader_T* newSegmentHeader = (MemorySegmentHeader_T*)((qword)currentMemorySegment + sizeof(MemorySegmentHeader_T) + size);

                newSegmentHeader->Free = true;
                newSegmentHeader->MemoryLength = ((qword)currentMemorySegment->MemoryLength) - (sizeof(MemorySegmentHeader_T) + size);
                newSegmentHeader->NextFreeSegment = currentMemorySegment->NextFreeSegment;
                newSegmentHeader->NextSegment = currentMemorySegment->NextSegment;
                newSegmentHeader->PreviousFreeSegment = currentMemorySegment->PreviousFreeSegment;
                newSegmentHeader->PreviousSegment = currentMemorySegment->PreviousSegment;

                currentMemorySegment->NextFreeSegment = newSegmentHeader;
                currentMemorySegment->NextSegment = newSegmentHeader;
                currentMemorySegment->MemoryLength = size;
            }

            if (currentMemorySegment == firstFreeMemorySegment){
                firstFreeMemorySegment = currentMemorySegment->NextFreeSegment;
            }

            currentMemorySegment->Free = false;

            if (currentMemorySegment->PreviousFreeSegment != 0 ){
                currentMemorySegment->PreviousFreeSegment->NextFreeSegment = currentMemorySegment->NextFreeSegment;
            }

            if (currentMemorySegment->NextFreeSegment != 0 ){
                currentMemorySegment->NextFreeSegment->PreviousFreeSegment = currentMemorySegment->PreviousFreeSegment;
            }

            if (currentMemorySegment->PreviousSegment != 0 ){
                currentMemorySegment->PreviousSegment->NextFreeSegment = currentMemorySegment->NextFreeSegment;
            }
            if (currentMemorySegment->NextSegment != 0 ){
                currentMemorySegment->NextSegment->PreviousFreeSegment = currentMemorySegment->PreviousFreeSegment;
            }

            return currentMemorySegment + 1;
        }

        if (currentMemorySegment->NextFreeSegment = 0){
            return 0;
        }

        currentMemorySegment = currentMemorySegment->NextFreeSegment;
    }
    return 0;
}

void combineFreeSegments(MemorySegmentHeader_T* a, MemorySegmentHeader_T* b) {
	if (a == 0) return;
	if (b == 0) return;
	if (a < b) {
		a->MemoryLength += b->MemoryLength + sizeof(MemorySegmentHeader_T);
		a->NextSegment = b->NextSegment;
		a->NextFreeSegment = b->NextFreeSegment;
		b->NextSegment->PreviousSegment = a;
		b->NextSegment->PreviousFreeSegment = a;
		b->NextFreeSegment->PreviousFreeSegment = a;
	}
	else {
		b->MemoryLength += a->MemoryLength + sizeof(MemorySegmentHeader_T);
		b->NextSegment = a->NextSegment;
		b->NextFreeSegment = a->NextFreeSegment;
		a->NextSegment->PreviousSegment = b;
		a->NextSegment->PreviousFreeSegment = b;
		a->NextFreeSegment->PreviousFreeSegment = b;
	}
}


void kfree(void* address) {
	MemorySegmentHeader_T* currentMemorySegment = ((MemorySegmentHeader_T*)address) - 1;
	currentMemorySegment->Free = true;

	if (currentMemorySegment < firstFreeMemorySegment) firstFreeMemorySegment = currentMemorySegment;

	if (currentMemorySegment->NextFreeSegment != 0) {
		if (currentMemorySegment->NextFreeSegment->PreviousFreeSegment < currentMemorySegment)
			currentMemorySegment->NextFreeSegment->PreviousFreeSegment = currentMemorySegment;
	}
	if (currentMemorySegment->PreviousFreeSegment != 0) {
		if (currentMemorySegment->PreviousFreeSegment->NextFreeSegment > currentMemorySegment)
			currentMemorySegment->PreviousFreeSegment->NextFreeSegment = currentMemorySegment;
	}
	if (currentMemorySegment->NextSegment != 0) {
		currentMemorySegment->NextSegment->PreviousSegment = currentMemorySegment;
		if (currentMemorySegment->NextSegment->Free) combineFreeSegments(currentMemorySegment, currentMemorySegment->NextSegment);
	}
	if (currentMemorySegment->PreviousSegment != 0) {
		currentMemorySegment->PreviousSegment->NextSegment = currentMemorySegment;
		if (currentMemorySegment->PreviousSegment->Free) combineFreeSegments(currentMemorySegment, currentMemorySegment->PreviousSegment);

	}
}


void* kcalloc(qword count, qword size) {
	void* mallocVal = kmalloc(size * count);
	memset(mallocVal, 0, size * count);
	return mallocVal;
}


void* krealloc(void* address, qword newSize) {
	MemorySegmentHeader_T* oldSegmentHeader = (MemorySegmentHeader_T*)address - 1;
	qword smallerSize = newSize;

	if (oldSegmentHeader->MemoryLength < newSize) {
        smallerSize = oldSegmentHeader->MemoryLength;
    }

	void* newMem = kmalloc(newSize);
	memcpy(newMem, address, smallerSize);
	kfree(address);
	return(newMem);
}
