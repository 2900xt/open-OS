#pragma once
#include <io.hpp>
#include <types.hpp>
#include <stdarg.h>


#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480


extern word Screen_x;
extern word Screen_y ;
extern byte* ScreenBuffer;


struct Character{
    byte row1;
    byte row2;
    byte row3;
    byte row4;
    byte row5;
    byte row6;
    byte row7;
    byte row8;
};

enum VGA_COLORS{
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

class VGA{
    protected:
        Character* font;
        uint16_t cursorPosX, cursorPosY;
    public:
    
        VGA() {
            font = (Character*)malloc(sizeof(Character) * 256);
        }
        static void putPixelArray(word x, word y, byte pixelsX){
            *(ScreenBuffer+((SCREEN_WIDTH/8)*y)+x/8) = pixelsX;
        }
        void addChar(char c,byte r1, byte r2 , byte r3, byte r4, byte r5, byte r6, byte r7, byte r8){
            font[c].row1=r1;
            font[c].row2=r2; 
            font[c].row3=r3; 
            font[c].row4=r4; 
            font[c].row5=r5; 
            font[c].row6=r6; 
            font[c].row7=r7; 
            font[c].row8=r8; 
        }
        void putChar(char c){
            if(font[c].row1 = 0)
                return;
            if(Screen_x==640){
                Screen_x=0;
                Screen_y+=8;
            }
            word old_y= Screen_y;
            putPixelArray(Screen_x,Screen_y++,font[c].row1);
            putPixelArray(Screen_x,Screen_y++,font[c].row2);
            putPixelArray(Screen_x,Screen_y++,font[c].row3);
            putPixelArray(Screen_x,Screen_y++,font[c].row4);
            putPixelArray(Screen_x,Screen_y++,font[c].row5);
            putPixelArray(Screen_x,Screen_y++,font[c].row6);
            putPixelArray(Screen_x,Screen_y++,font[c].row7);
            putPixelArray(Screen_x,Screen_y++,font[c].row8);

            Screen_x+=8;
            Screen_y=old_y;
        }
        void putString(const char* str){
            while(*str){
                if(*str=='\r'){
                    Screen_x=0;
                    str++;
                    continue;
                }
                else if(*str=='\n'){
                    Screen_y+=8;
                    Screen_x=0;
                    str++;
                    continue;
                }
                else if(*str== '\\'){
                    putChar('\\');
                    str++;
                    continue;
                }
                else if(*str== '\''){
                    putChar('\'');
                    str++;
                    continue;
                }
                else if(*str== '\"'){
                    putChar('\"');
                    str++;
                    continue;
                }

                putChar(*str++);
            }
        }

        void putInt(qword integer){
            putString(IntegerToString(integer));
        }

        void clearScreen(){
            for(int i = 0;i<80;i++){
                for(int j = 0;j<60;j++){
                    putChar(' ');
                }
            }
        }

        void printf(const char* fmt, ...){
            if(fmt == nullptr)
                return;
            
            va_list valist;
            va_start(valist, fmt);
            int num = 0;
            char* token = nullptr;
            int i = 0;
            int found = 0;
            float fp = 0;

            while(fmt[i] != '\0'){
                num = 0;
                found = 0;
                token = nullptr;

                if((fmt[i] == '%')){
                    switch(fmt[i+1]){
                        case 'd':
                            {
                                found = 1;
                                num = va_arg(valist, int);
                                //this->putString(IntegerToString(num));
                            }
                            break;
                        case 's':
                            {
                                found = 1;
                                token = va_arg(valist, char*);
                                this->putString(token);
                            }
                            break;
                        case 'c':
                            {
                                found = 1;
                                num = va_arg(valist, int);
                                outb(0x3c4,0x02);
                                outb(0x3c5,num & 0b00001111);
                            }
                            break;
                        case 'x':
                            {
                                found = 1;
                                num = va_arg(valist, int);
                                int ind = 1, j, temp;
                                char base16[16];

                                while(num != 0){
                                    temp = num % 16;
                                    if(temp < 10)
                                        temp = temp+48;
                                    else 
                                        temp = temp+55;
                                    base16[ind++] = temp;
                                    num /= 16;
                                }

                                this->putString("0x");
                                for(int j = ind - 1; j > 0;j--)
                                    this->putChar(base16[j]);
                                
                                break;
                            }
                        case 'f':
                            {
                                found = 1;
                                fp = va_arg(valist, float);
                                num = fmt[i+1] - 48;
                                if(num > 9 || num < 1){
                                    break;
                                }
                                //this->putString(FloatToString(fp, num));
                                i++;
                                break;
                            }
                    }

                    if(found != 0){
                        i+=2;
                        continue;
                    }
                }

                if(fmt[i] == '\n'){
                    Screen_y += 8;
                    Screen_x = 0;
                }
                else{
                    putChar(fmt[i]);
                }
                i++;
            }

            va_end(valist);
        }


};


extern VGA TTY1;
void initTTY(VGA* tty);