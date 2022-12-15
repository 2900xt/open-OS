#include <io.hpp>

qword uptime = 0;

void MagicBreak(){
    asm volatile("xchg %bx,%bx");
}

void debugPrint(const char* str){
    while(*str)
        outb(0xE9, *str++);
}

void debugPrintInteger(qword i){
	debugPrint(IntegerToString(i));
}

void outw(unsigned short port, unsigned short val){
  asm volatile ("outw %0, %1" : : "Na"(val), "Nd"(port));
}

int strlen(const char* str){
    int length = 0;
    while(*str++)
        length++;
    return length;
}

char integerToStringOutput[128];
template<typename T>
const char* IntegerToString(T value) {

	byte isNegative = 0;

	if (value < 0) {
		isNegative = 1;
		value *= -1;
		integerToStringOutput[0] = '-';
	}

	byte size = 0;
	qword sizeTester = (qword)value;
	while (sizeTester / 10 > 0) {
		sizeTester /= 10;
		size++;
	}

	byte index = 0;
	qword newValue = (qword)value;
	while (newValue / 10 > 0) {
		byte remainder = newValue % 10;
		newValue /= 10;
		integerToStringOutput[isNegative + size - index] = remainder + 48; 
		index++;
	}
	byte remainder = newValue % 10;
	integerToStringOutput[isNegative + size - index] = remainder + 48;
	integerToStringOutput[isNegative + size + 1] = 0;
	return integerToStringOutput;
}