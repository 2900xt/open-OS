#pragma once
#include <io.hpp>
#include <types.hpp>


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

class VGA{
    protected:
        Character font[256]; 
    public:
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
        void putString(string str){
            while(*str){
                if(*str=='\r'){
                    Screen_x=0;
                    str++;
                    continue;
                }
                else if(*str=='\n'){
                    Screen_y+=8;
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
        void clearScreen(){
            for(int i = 0;i<80;i++){
                for(int j = 0;j<60;j++){
                    putChar(' ');
                }
            }
        }

    const char g_HexChars[17] = "0123456789ABCDEF";

    void putUnsigned(qword number, int radix = 10)
    {
        char buffer[32];
        int pos = 0;

        // convert number to ASCII
        do 
        {
            qword rem = number % radix;
            number /= radix;
            buffer[pos++] = g_HexChars[rem];
        } while (number > 0);

        // print number in reverse order
        putString(buffer);
    }
    
};


extern VGA TTY1;
void initTTY(VGA* tty);