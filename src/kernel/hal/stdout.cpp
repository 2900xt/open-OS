#include <types.h>
#include <stdout.h>
#include <stdlib.h>

word screenX = 0, screenY = 0;
byte* screenBuffer = (byte*)0xA0000;

typedef byte character;

character font[256][8];

void putPixelsAt(word x, word y, byte src){
    *(screenBuffer + ((SCREEN_WIDTH / 8) * y) + x / 8) = src;
}

void addCharIntoFont(char c, byte r1, byte r2 , byte r3, byte r4, byte r5, byte r6, byte r7, byte r8){
    font[c][0] = r1;
    font[c][1] = r2;
    font[c][2] = r3;
    font[c][3] = r4;
    font[c][4] = r5;
    font[c][5] = r6;
    font[c][6] = r7;
    font[c][7] = r8;
}

void setColor(COLORS color){
    outb(0x3c4, 0x02);
    outb(0x3c5, (int)color & 0b00001111);
}

void putc(char c){

    if(screenX == SCREEN_WIDTH){
        screenX = 0;
        screenY += 8;
    }

    for(int i = 0; i < 8 ; i++){
        putPixelsAt(screenX, screenY++, font[c][i]);
    }

    screenX += 8;
    screenY -= 8;
}

void puts(const char* src){
    while(*src != '\0'){
        if(*src == '\n'){
            screenY += 8;
            screenX = 0;
        } else {
            putc(*src);
        }

        src++;
    }
}

int strlen(const char* src){
    int length = 0;
    while(*src++ != '\0'){
        length++;
    }
    return length;
}

void printf(const char* fmt, ...){
    if(fmt == NULL)
        return;
    
    va_list valist;
    va_start(valist, fmt);

    int num = 0;
    char* token = NULL;
    int i = 0;
    bool found = false;

    while(fmt[i] != '\0'){
        if((fmt[i] == '%'))
        {
            switch(fmt[i+1]){
            case 'd':
                {
                    found = true;
                    num = va_arg(valist, int);
                    puts(IntegerToString(num));
                }
                break;
            case 's':
                {
                    found = true;
                    token = va_arg(valist, char*);
                    puts(token);
                }
                break;
            case 'c':
                {
                    found = true;
                    num = (int)va_arg(valist, COLORS);
                    setColor((COLORS)num);
                }
                break;
            case 'x':
                {
                    found = true;
                    num = va_arg(valist, int);
                    int ind = 1, j, temp;
                    char base16[16];

                    while(num != 0){
                        temp = num % 16;
                        if(temp < 10) temp = temp + 48;
                        else temp = temp + 55;
                        base16[ind++] = temp;
                        num /= 16;
                    }

                    puts("0x");
                    for(int j = ind - 1; j > 0; j--)
                        putc(base16[j]);
                    
                    break;
                }
            }

            if(found){
                i += 2;
                continue;
            }

        }

        if(fmt[i] == '\n'){
            screenY += 8;
            screenX = 0;
        }
        else{
            putc(fmt[i]);
        }
        
        i++;

    }

    va_end(valist);
    
}

char* fprintf(const char* fmt , ...){
    char* buffer = (char*)kcalloc(1, 512);

    va_list valist;
    va_start(valist, fmt);

    int i = 0;
    int ind = 0;
    bool found = false;
    char* token = NULL;

    while(fmt[i] != '\0'){
        if((fmt[i] == '%'))
        {
            switch(fmt[i+1]){
            case 'd':
                {
                    found = true;
                    int num = va_arg(valist, int);
                    token = (char*)IntegerToString(num);
                    memcpy(buffer + ind, token, strlen(token));
                    ind += strlen(token);
                }
                break;
            case 's':
                {
                    found = true;
                    token = va_arg(valist, char*);
                    memcpy(buffer + ind, token, strlen(token));
                    ind += strlen(token);
                }
                break;
            case 'x':
                {
                    found = true;
                    int num = va_arg(valist, int);
                    int index = 1, j, temp;
                    char base16[16];

                    while(num != 0){
                        temp = num % 16;
                        if(temp < 10) temp = temp + 48;
                        else temp = temp + 55;
                        base16[index++] = temp;
                        num /= 16;
                    }

                    buffer[ind++] = '0';
                    buffer[ind++] = 'x';

                    for(int j = index - 1; j > 0; j--)
                        buffer[ind++] = base16[j];
                    
                    break;
                }
            }
            
            if(found){
                i += 2;
                continue;
            }

        }

        else{
            buffer[ind] = fmt[i];
            ind++;
        }
        
        i++;

    }

    va_end(valist);

    return buffer;

}

void kpanic(const char* msg){

    __asm__("cli");

    screenX = 0;
    screenY = 0;

    setColor(COLORS::BLUE);

    for(int i = 0; i < (SCREEN_WIDTH *SCREEN_HEIGHT) / 64; i++){
        putc('#');
    }


    screenX = 0;
    screenY = 0;

    setColor(COLORS::WHITE);

    printf(msg);

    for(;;);

}

void stdoutINIT(void) {

    addCharIntoFont('A',
    0b00000000,
    0b00011000,
    0b00100100,
    0b01100110,
    0b01111110,
    0b01000010,
    0b01000010,
    0b00000000
);

    addCharIntoFont('S',
    0b00000000,
    0b00111110,
    0b01110000,
    0b00111000,
    0b00001110,
    0b00011100,
    0b01111000,
    0b00000000 
    );

    addCharIntoFont( 'D',
    0b00000000,
    0b01111000,
    0b01001100,
    0b01000110,
    0b01000110,
    0b01001100,
    0b01111000,
    0b00000000
    );
    addCharIntoFont('F',
    0b00000000,
    0b01111110,
    0b01000000,
    0b01000000,
    0b01111110,
    0b01000000,
    0b01000000,
    0b00000000
    );
    addCharIntoFont('G',
    0b00000000,
    0b00111110,
    0b01100000,
    0b01000000,
    0b01000110,
    0b01100010,
    0b00111110,
    0b00000000
    );
    addCharIntoFont('H',
    0b00000000,
    0b01000010,
    0b01000010,
    0b01111110,
    0b01111110,
    0b01000010,
    0b01000010,
    0b00000000
    );
    addCharIntoFont('J',
    0b00000000,
    0b00000010,
    0b00000010,
    0b00000010,
    0b00000010,
    0b01100110,
    0b00111100,
    0b00000000 
    );
    addCharIntoFont('K',
    0b00000000,    
    0b01001100,    
    0b01011000,    
    0b01110000,    
    0b01110000,    
    0b01011000,    
    0b01001100,    
    0b00000000    
    );
    addCharIntoFont('L',
    0b00000000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01111110,
    0b00000000
    );
    addCharIntoFont('Z',
    0b0000000,
    0b1111110,
    0b0000110,
    0b0001110,
    0b0011100,
    0b0110000,
    0b0111111,
    0b0000000
    );
    addCharIntoFont('X',
    0b00000000,
    0b01000010,
    0b01100110,
    0b00111100,
    0b00111100,
    0b01100110,
    0b01000010,
    0b00000000
    );
    addCharIntoFont('C',
    0b00000000,
    0b00111110,
    0b01100000,
    0b01000000,
    0b01000000,
    0b01100000,
    0b00111110,
    0b00000000    
    );
    addCharIntoFont('V',
    0b00000000,
    0b01000010,
    0b01100110,
    0b01100100,
    0b01101100,
    0b00111000,
    0b00010000,
    0b00000000
    );
    addCharIntoFont('B',
    0b00000000   ,
    0b01111100   ,
    0b01000110   ,
    0b01111100   ,
    0b01000100  , 
    0b01000110   ,
    0b01111100 ,  
    0b00000000   
    );
    addCharIntoFont('N',
    0b00000000,
    0b01100010,
    0b01010010,
    0b01001010,
    0b01001010,
    0b01001010,
    0b01000110,
    0b00000000
    );
    addCharIntoFont('M',
    0b00000000,
    0b01000010,
    0b01100110,
    0b01111110,
    0b01011010,
    0b01000010,
    0b01000010,
    0b00000000
    );
    addCharIntoFont('Q',
    0b00000000,
    0b00111100,
    0b01100110,
    0b01000010,
    0b01100110,
    0b00111100,
    0b00000110,
    0b00000000    
    );
    addCharIntoFont('W',
    0b00000000,
    0b01000010,
    0b01000010,
    0b01011010,
    0b01111110,
    0b01100110,
    0b01000010,
    0b00000000    
    );
    addCharIntoFont('E',
    0b00000000,
    0b01111110,
    0b01000000,
    0b01000000,
    0b01111110,
    0b01000000,
    0b01111110,
    0b00000000
    );
    addCharIntoFont('R',
    0b00000000,
    0b01111100,
    0b01000010,
    0b01111100,
    0b01110000,
    0b01011000,
    0b01001110,
    0b00000000    
    );
    addCharIntoFont('T',
    0b00000000,
    0b01111110,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00000000
    );
    addCharIntoFont('Y',
    0b00000000,
    0b01000010,
    0b01100110,
    0b00111100,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00000000
    );
    addCharIntoFont('U',
    0b00000000,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01111110,
    0b00111100,
    0b00000000
    );
    addCharIntoFont('I',
    0b00000000,
    0b01111110,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b01111110,
    0b00000000
    );
    addCharIntoFont('O',
    0b00000000,
    0b00111100,
    0b01100110,
    0b01000010,
    0b01000010,
    0b01100110,
    0b00111100,
    0b00000000    
    );
    addCharIntoFont('P',
    0b00000000,
    0b01111100,
    0b01000010,
    0b01111100,
    0b01000000,
    0b01000000,
    0b01000000,
    0b00000000
    );
    addCharIntoFont(' ',
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
    );
    addCharIntoFont('`',
    0b00000000,
    0b00000000,
    0b00110000,
    0b00011000,
    0b00000100,
    0b00000000,
    0b00000000,
    0b00000000
    );
    addCharIntoFont('~',
    0b00000000,
    0b00000000,
    0b11100001,
    0b10111011,
    0b00001110,
    0b00000000,
    0b00000000,
    0b00000000
    );
    addCharIntoFont('1',
    0b00000000,
    0b00111000,
    0b11111000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b11111110,
    0b00000000
    );
    addCharIntoFont('!',
    0b00000000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00011000,
    0b00000000
    );
    addCharIntoFont('2',
    0b00000000,
    0b11111000,
    0b00011100,
    0b00111000,
    0b01100000,
    0b11000000,
    0b11111100,
    0b00000000
    );
    addCharIntoFont('3',
    0b00000000,
    0b11111100,
    0b00000110,
    0b11111100,
    0b00000110,
    0b00000110,
    0b11111100,
    0b00000000
    );
    addCharIntoFont('4',
    0b00000000,
    0b00001100,
    0b00010100,
    0b00100100,
    0b01000100,
    0b01111110,
    0b00000100,
    0b00000000
    );
    addCharIntoFont('5',
    0b00000000,
    0b01111110,
    0b01000000,
    0b01111100,
    0b00000010,
    0b00000010,
    0b01111100,
    0b00000000
    );
    addCharIntoFont('6',
    0b00000000,
    0b00111110,
    0b01100000,
    0b01000000,
    0b01111100,
    0b01000010,
    0b00111100,
    0b00000000
    );
    addCharIntoFont('7',
    0b00000000,
    0b01111110,
    0b00000110,
    0b00001100,
    0b00011000,
    0b00110000,
    0b01100000,
    0b00000000
    );
    addCharIntoFont('8',
    0b00000000,
    0b00111100,
    0b01000010,
    0b01000010,
    0b00111100,
    0b01000010,
    0b00111100,
    0b00000000
    );
    addCharIntoFont('9',
    0b00000000,
    0b00111100,
    0b01000010,
    0b00111110,
    0b00000010,
    0b00000010,
    0b00000010,
    0b00000000
    );
    addCharIntoFont('0',
    0b00000000,
    0b00111100,
    0b01100110,
    0b01000010,
    0b01000010,
    0b01100110,
    0b00111100,
    0b00000000
    );
    addCharIntoFont('-',
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00111100,
    0b00000000,
    0b00000000,
    0b00000000
    );
    addCharIntoFont('_',
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01111110,
    0b00000000
    );
    addCharIntoFont('=',
    0b00000000,
    0b00000000,
    0b00111100,
    0b00000000,
    0b00000000,
    0b00111100,
    0b00000000,
    0b00000000
    );
    addCharIntoFont('+',
    0b00000000,
    0b00011000,
    0b00011000,
    0b01111110,
    0b01111110,
    0b00011000,
    0b00011000,
    0b00000000
    );
    addCharIntoFont('[',
    0b00000000,
    0b01100000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01000000,
    0b01100000,
    0b00000000
    );
    addCharIntoFont(']',
    0b00000000,
    0b00000110,
    0b00000010,
    0b00000010,
    0b00000010,
    0b00000010,
    0b00000110,
    0b00000000
    );
    addCharIntoFont('{',
    0b00000000,
    0b00110000,
    0b00100000,
    0b01100000,
    0b01100000,
    0b00100000,
    0b00110000,
    0b00000000
    );
    addCharIntoFont('}',
    0b00000000,
    0b00001100,
    0b00000100,
    0b00000110,
    0b00000110,
    0b00000100,
    0b00001100,
    0b00000000
    );
    addCharIntoFont('\\',
    0b00000000,
    0b01100000,
    0b00110000,
    0b00011000,
    0b00001100,
    0b00000110,
    0b00000010,
    0b00000000
    );
    addCharIntoFont('|',
    0b00000000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00000000
    );
    addCharIntoFont(';',
    0b00000000,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00110000,
    0b00000000
    );
    addCharIntoFont(':',
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000
    );
    addCharIntoFont('\'',
    0b00000000,
    0b00000000,
    0b00010000,
    0b00010000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
    );
    addCharIntoFont('\"',
    0b00000000,
    0b00000000,
    0b00101000,
    0b00101000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
    );
    addCharIntoFont(',',
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00001000,
    0b00001000,
    0b00010000,
    0b00000000
    );
    addCharIntoFont('.',
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00010000,
    0b00000000
    );
    addCharIntoFont('<',
    0b00000000,
    0b00000000,
    0b00011000,
    0b00100000,
    0b01000000,
    0b00100000,
    0b00011000,
    0b00000000
    );
    addCharIntoFont('>',
    0b00000000,
    0b00000000,
    0b00011000,
    0b00000100,
    0b00000010,
    0b00000100,
    0b00011000,
    0b00000000
    );
    addCharIntoFont('/',
    0b00000000,
    0b00000110,
    0b00001100,
    0b00011000,
    0b00110000,
    0b01100000,
    0b01000000,
    0b00000000
    );
    addCharIntoFont('?',
    0b00000000,
    0b01111000,
    0b00000100,
    0b00011000,
    0b00010000,
    0b00000000,
    0b00010000,
    0b00000000
    );
    addCharIntoFont('^',
    0b00000000,
    0b00011000,
    0b00100100,
    0b01000010,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
    );
    addCharIntoFont('*',
    0b00000000,
    0b00010000,
    0b00111000,
    0b00010000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
    );
    addCharIntoFont('(',
    0b00000000,
    0b00011000,
    0b00100000,
    0b01000000,
    0b01000000,
    0b00100000,
    0b00011000,
    0b00000000
    );
    addCharIntoFont(')',
    0b00000000,
    0b00011000,
    0b00000100,
    0b00000010,
    0b00000010,
    0b00000100,
    0b00011000,
    0b00000000
    );
    addCharIntoFont('q',
    0b00000000,
    0b00000000,
    0b00011100,
    0b00100010,
    0b00011110,
    0b00000010,
    0b00000010,
    0b00000000
    );
    addCharIntoFont('w',
    0b00000000,
    0b00000000,
    0b00000000,
    0b01000010,
    0b01000010,
    0b01011010,
    0b00100100,
    0b00000000
    );
    addCharIntoFont('e',
    0b00000000,
    0b00000000,
    0b00111100,
    0b01000010,
    0b01111100,
    0b01000000,
    0b00111100,
    0b00000000
    );
    addCharIntoFont('r',
    0b00000000,
    0b00000000,
    0b00000000,
    0b01011110,
    0b00100000,
    0b00100000,
    0b01110000,
    0b00000000
    );
    addCharIntoFont('t',
    0b00000000,
    0b00000000,
    0b00010000,
    0b00111000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00000000
    );
    addCharIntoFont('y',
    0b00000000,
    0b00000000,
    0b01000100,
    0b00101000,
    0b00010000,
    0b00100000,
    0b01000000,
    0b00000000
    );
    addCharIntoFont('u',
    0b00000000,
    0b00000000,
    0b00000000,
    0b01000100,
    0b01000100,
    0b01000100,
    0b00111000,
    0b00000000
    );
    addCharIntoFont('i',
    0b00000000,
    0b00000000,
    0b00010000,
    0b00000000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00000000
    );
    addCharIntoFont('o',
    0b00000000,
    0b00000000,
    0b00000000,
    0b00111000,
    0b01000100,
    0b01000100,
    0b00111000,
    0b00000000
    );
    addCharIntoFont('p',
    0b00000000,
    0b00000000,
    0b01111000,
    0b01000100,
    0b01111000,
    0b01000000,
    0b01000000,
    0b00000000
    );
    addCharIntoFont('a',
    0b00000000,
    0b00000000,
    0b00011100,
    0b00000010,
    0b00011110,
    0b00100010,
    0b00011110,
    0b00000000
    );
    addCharIntoFont('s',
    0b00000000,
    0b00000000,
    0b00011110,
    0b01100000,
    0b00011000,
    0b00000110,
    0b00111100,
    0b00000000
    );
    addCharIntoFont('d',
    0b00000000,
    0b00000000,
    0b00000010,
    0b00000010,
    0b00001110,
    0b00010010,
    0b00001110,
    0b00000000
    );
    addCharIntoFont('f',
    0b00000000,
    0b00000000,
    0b00011100,
    0b00100000,
    0b01111000,
    0b00100000,
    0b00100000,
    0b00000000
    );
    addCharIntoFont('g',
    0b00000000,
    0b00000000,
    0b00011100,
    0b00100010,
    0b00011110,
    0b00000010,
    0b00011100,
    0b00000000
    );

    addCharIntoFont('h',
    0b00000000,
    0b00000000,
    0b01000000,
    0b01000000,
    0b01110000,
    0b01001000,
    0b01001000,
    0b00000000
    );
    addCharIntoFont('j',
    0b00000000,
    0b00000100,
    0b00000000,
    0b00000100,
    0b00000100,
    0b00100100,
    0b00011000,
    0b00000000
    );
    addCharIntoFont('k',
    0b00000000,
    0b00000000,
    0b01000000,
    0b01010000,
    0b01100000,
    0b01010000,
    0b01001000,
    0b00000000
    );
    addCharIntoFont('l',
    0b00000000,
    0b00000000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00000000
    );
    addCharIntoFont('z',
    0b00000000,
    0b00000000,
    0b00111100,
    0b00000100,
    0b00011000,
    0b00100000,
    0b00111100,
    0b00000000
    );
    addCharIntoFont('x',
    0b00000000,
    0b00000000,
    0b00100010,
    0b00010100,
    0b00001000,
    0b00010100,
    0b00100010,
    0b00000000
    );
    addCharIntoFont('c',
    0b00000000,
    0b00000000,
    0b00000000,
    0b00111100,
    0b01000000,
    0b01000000,
    0b00111100,
    0b00000000
    );
    addCharIntoFont('v',
    0b00000000,
    0b00000000,
    0b01000100,
    0b01000100,
    0b00101000,
    0b00101000,
    0b00010000,
    0b00000000
    );
    addCharIntoFont('b',
    0b00000000,
    0b00000000,
    0b01000000,
    0b01110000,
    0b01001000,
    0b01001000,
    0b01110000,
    0b00000000
    );
    addCharIntoFont('n',
    0b00000000,
    0b00000000,
    0b00000000,
    0b00111000,
    0b01000100,
    0b01000100,
    0b01000100,
    0b00000000
    );
    addCharIntoFont('m',
    0b00000000,
    0b00000000,
    0b00000000,
    0b00101000,
    0b01010100,
    0b01010100,
    0b01010100,
    0b00000000
    );
    addCharIntoFont('#',
    0b11111111,    
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111
    );
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