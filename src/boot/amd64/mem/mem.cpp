#include <types.hpp>
#include <io.hpp>

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
