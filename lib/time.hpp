#ifndef TIME_HPP
#define TIME_HPP

#define CURRENT_YEAR 2022

#include <io.hpp>
#include <fonts.hpp>

struct TIME_T
{
    byte second;
    byte minute;
    byte hour;
    byte day;
    byte month;
    dword year;
};

enum CMOS_REGS
{
    CMOS_ADDRESS    = 0x70,
    CMOS_DATA       = 0x71
};

void readRTC(TIME_T* current);

#endif