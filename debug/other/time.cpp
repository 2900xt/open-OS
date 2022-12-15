#include <time.hpp>

int getUpdateFlag(){
    outb(CMOS_ADDRESS,0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

byte getRTCReg(int reg){
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

void readRTC(TIME_T* current){
      byte last_second;
      byte last_minute;
      byte last_hour;
      byte last_day;
      byte last_month;
      byte last_year;
      byte registerB;
 
      // Note: This uses the "read registers until you get the same values twice in a row" technique
      //       to avoid getting dodgy/inconsistent values due to RTC updates
 
      while (getUpdateFlag());                // Make sure an update isn't in progress
      current->second = getRTCReg(0x00);
      current->minute = getRTCReg(0x02);
      current->hour = getRTCReg(0x04);
      current->day = getRTCReg(0x07);
      current->month = getRTCReg(0x08);
      current->year = getRTCReg(0x09);
 
      do {
            last_second = current->second;
            last_minute = current->minute;
            last_hour = current->hour;
            last_day = current->day;
            last_month = current->month;
            last_year = current->year;
 
            while (getUpdateFlag());           // Make sure an update isn't in progress
            current->second = getRTCReg(0x00);
            current->minute = getRTCReg(0x02);
            current->hour = getRTCReg(0x04);
            current->day = getRTCReg(0x07);
            current->month = getRTCReg(0x08);
            current->year = getRTCReg(0x09);

      } while( (last_second != current->second) || (last_minute != current->minute) || (last_hour != current->hour) ||
               (last_day != current->day) || (last_month != current->month) || (last_year != current->year));
 
      registerB = getRTCReg(0x0B);
 
      // Convert BCD to binary values if necessary
 
      if (!(registerB & 0x04)) {
            current->second = (current->second & 0x0F) + ((current->second / 16) * 10);
            current->minute = (current->minute & 0x0F) + ((current->minute / 16) * 10);
            current->hour = ( (current->hour & 0x0F) + (((current->hour & 0x70) / 16) * 10) ) | (current->hour & 0x80);
            current->day = (current->day & 0x0F) + ((current->day / 16) * 10);
            current->month = (current->month & 0x0F) + ((current->month / 16) * 10);
            current->year = (current->year & 0x0F) + ((current->year / 16) * 10);
      }
 
      // Convert 12 current->hour clock to 24 current->hour clock if necessary
 
      if (!(registerB & 0x02) && (current->hour & 0x80)) {
            current->hour = ((current->hour & 0x7F) + 12) % 24;
      }
 
      // Calculate the full (4-digit) year
 
        {
            current->year += (CURRENT_YEAR / 100) * 100;
            if(current->year < CURRENT_YEAR) current->year += 100;
        }
}