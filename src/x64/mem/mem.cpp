#include <types.hpp>
#include <io.hpp>

MemorySegmentHeader_T* FirstFreeMemorySegment;

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


void heap_init(qword heap_address, qword heap_length){
    FirstFreeMemorySegment = (MemorySegmentHeader_T*)heap_address;
    FirstFreeMemorySegment->MemoryLength = heap_length - sizeof(MemorySegmentHeader_T);
    FirstFreeMemorySegment->NextSegment=0;
    FirstFreeMemorySegment->PreviousSegment=0;
    FirstFreeMemorySegment->NextFreeSegment=0;
    FirstFreeMemorySegment->PreviousFreeSegment=0;
    FirstFreeMemorySegment->Free = true;
}

void* malloc(qword size){
    qword remainder = size % 8;
    size -= remainder;
    if (remainder != 0)
        size += 8;
    
    MemorySegmentHeader_T* currentMemorySegment = FirstFreeMemorySegment;

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

            if (currentMemorySegment == FirstFreeMemorySegment){
                FirstFreeMemorySegment = currentMemorySegment->NextFreeSegment;
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
void CombineFreeSegments(MemorySegmentHeader_T* a, MemorySegmentHeader_T* b) {
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

void free(void* address) {
	MemorySegmentHeader_T* currentMemorySegment = ((MemorySegmentHeader_T*)address) - 1;
	currentMemorySegment->Free = true;

	if (currentMemorySegment < FirstFreeMemorySegment) FirstFreeMemorySegment = currentMemorySegment;

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
		if (currentMemorySegment->NextSegment->Free) CombineFreeSegments(currentMemorySegment, currentMemorySegment->NextSegment);
	}
	if (currentMemorySegment->PreviousSegment != 0) {
		currentMemorySegment->PreviousSegment->NextSegment = currentMemorySegment;
		if (currentMemorySegment->PreviousSegment->Free) CombineFreeSegments(currentMemorySegment, currentMemorySegment->PreviousSegment);

	}
}


void* calloc(qword size) {
	void* mallocVal = malloc(size);
	memset(mallocVal, 0, size);
	return mallocVal;
}


void* realloc(void* address, qword newSize) {
	MemorySegmentHeader_T* oldSegmentHeader = (MemorySegmentHeader_T*)address - 1;
	qword smallerSize = newSize;
	if (oldSegmentHeader->MemoryLength < newSize) smallerSize = oldSegmentHeader->MemoryLength;
	void* newMem = malloc(newSize);
	memcpy(newMem, address, smallerSize);
	free(address);
	return(newMem);
}