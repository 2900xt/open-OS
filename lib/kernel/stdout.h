#ifndef STDOUT_H
#define STDOUT_H

void stdoutINIT(void);
void putChar(char c);


enum class COLORS{
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


#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480


#endif