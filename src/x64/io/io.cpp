#include <io.hpp>
#include <int.hpp>

qword uptime = 0;

void MagicBreak(){
    asm volatile("xchg %bx,%bx");
}

void debugPrint(string str){
    while(*str)
        outb(0xE9, *str++);
}

void debugPrintInteger(qword i){
	debugPrint(IntegerToString(i));
}

char getChar(){
	while(!keyPressed);
	keyPressed = false;
	return lastKey;
}

char buffer[80];

char* getLine(){
	int i = 0;
	while(!enterPressed)
		buffer[i++]=getChar();
	
	buffer[i]='\0';
	return buffer;
}

int strlen(const char* str){
    int length = 0;
    while(*str++)
        length++;
    return length;
}

void strcat(char* destination, char* source){
    char* ptr = destination + strlen(destination);

    while(*source)
        *ptr++ = *source++;

    *ptr = '\0';
    return;
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

char floatToStringOutput[128];
const char* FloatToString(float value, byte decimalPlaces) {
	char* intPtr = (char*)IntegerToString((int)value);
	char* floatPtr = floatToStringOutput;

	if (value < 0) {
		value *= -1;
	}

	while (*intPtr != 0) {
		*floatPtr = *intPtr;
		intPtr++;
		floatPtr++;
	}
	*floatPtr = '.';
	floatPtr++;

	float newValue = value - (int)value;

	for (byte i = 0; i < decimalPlaces; i++) {
		newValue *= 10;
		*floatPtr = (int)newValue + 48;
		newValue -= (int)newValue;
		floatPtr++;
	}

	*floatPtr = 0;

	return floatToStringOutput;
}

