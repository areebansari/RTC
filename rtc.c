#include <18f252.h>
#use delay (clock=20M)
#fuses NOLVP, NOPROTECT, NOWDT, HS
#include "C:\Users\Administrator\Desktop\New folder\RTC\LCD_Driver.c"

#use i2c(master, fast=100000, SDA=PIN_C4, SCL=PIN_C3, force_hw)

#define RTC_ReadMode   0xD1
#define RTC_WriteMode  0xD0
#define RTC_ControlReg 0x07
#define RTC_Date       0x04 
#define RTC_Month      0x05
#define RTC_Year       0x06 
#define RTC_Weekday    0x03
#define RTC_Sec        0x00 
#define RTC_Min        0x01
#define RTC_Hrs        0x02 
#define setTime        input(PIN_B0)
#define incr           input(PIN_B1)
#define decr           input(PIN_B2)
#define setAlarm       input(PIN_B3)
#define setFlag        input(PIN_B4)

unsigned int8 rtcDay ,rtcDate, rtcMonth, rtcYear, rtcSec, rtcMin, rtcHours, AMPM; //for reading values from DS1307
unsigned int8 setDate, setMonth, setYear, setSec, setMin, setHours, setAMPM; //for setting values by taking user input
//-----------------------------------alarm time settings by user------------------------------------------------------------//
unsigned int8 _setmin[] = {0,0,0,0,0,0,0,0,0,0};
unsigned int8 sethour[] = {0,0,0,0,0,0,0,0,0,0};
int1 ap[] = {0,0,0,0,0,0,0,0,0,0};
//--------------------------------------------------------------------------------------------------------------------------//
int1 a1=FALSE, a2=FALSE, a3=FALSE, a4=FALSE, a5=FALSE, a6=FALSE, a7=FALSE, a8=FALSE, a9=FALSE, a10=FALSE; //to activate or deactivate alarms
int btn_count = -1;
int alm_count = -1;
int flag_count = -1;
unsigned int8 timer_counter = 0;
int1 set = FALSE;
int1 debounce = TRUE;
int1 _debounce = TRUE;
int1 __debounce = TRUE;
int1 timeFlag = FALSE;
int1 alarmFlag = FALSE;
int1 Flag = FALSE;
//-----for debouncing-----------//
int1 a = 1, b = 1; 
int1 c = 1, d = 1; 
int1 e = 1, f = 1;             
//-----------------------------//
const char AM[] = {'A', 'M'};
const char PM[] = {'P', 'M'};

#INT_TIMER1
void tmr1_isr()
{
   timer_counter++;  
   
   if(timer_counter == 100)
   {
      output_low(PIN_A0);
      timer_counter = 0;
      setup_timer_1(T1_DISABLED);
   }
      
}

unsigned int bcd2dec(int num)
{
   return ((num/16*10) + (num%16));
}

void initRTC(unsigned int8 date, month, year, weekDay, sec, mins, hours)
{
   /* Disabling SQW output */
   i2c_start();
   i2c_write(RTC_WriteMode);
   i2c_write(RTC_ControlReg);
   i2c_write(0x00);
   i2c_stop();
   /****************************************/
   
   i2c_start();
   i2c_write(RTC_WriteMode);
   i2c_write(RTC_Sec);
  
   i2c_write(sec);
   
   i2c_write(mins);
   
   i2c_write(hours);
   
   i2c_write(weekDay);
   
   i2c_write(date);
   
   i2c_write(month);
   
   i2c_write(year);  
   
   i2c_stop();
}

void main()
{
//!   initRTC(0x24, 0x09, 0x18, 0x01, 0x00, 0x04, 0x65);
   //initRTC(date, month, year, weekday, sec, mins, hours)
   delay_ms(100); lcd_init();
   
   clear_interrupt(INT_TIMER1);
   enable_interrupts(INT_TIMER1);   
   enable_interrupts(GLOBAL);
   
   _setmin[0]= read_eeprom(0);
   sethour[0] = read_eeprom(1);
   ap[0] = read_eeprom(2);
   _setmin[1] = read_eeprom(3);
   sethour[1] = read_eeprom(4);
   ap[1] = read_eeprom(5);
   _setmin[2] = read_eeprom(6);
   sethour[2] = read_eeprom(7);
   ap[2] = read_eeprom(8);
   _setmin[3] = read_eeprom(9);
   sethour[3] = read_eeprom(10);
   ap[3] = read_eeprom(11);
   _setmin[4] = read_eeprom(12);
   sethour[4] = read_eeprom(13);
   ap[4] = read_eeprom(14);
   _setmin[5] = read_eeprom(15);
   sethour[5] = read_eeprom(16);
   ap[5] = read_eeprom(17);
   _setmin[6] = read_eeprom(18);
   sethour[6] = read_eeprom(19);
   ap[6] = read_eeprom(20);
   _setmin[7] = read_eeprom(21);
   sethour[7] = read_eeprom(22);
   ap[7] = read_eeprom(23);
   _setmin[8] = read_eeprom(24);
   sethour[8] = read_eeprom(25);
   ap[8] = read_eeprom(26);
   _setmin[9] = read_eeprom(27);
   sethour[9] = read_eeprom(28);
   ap[9] = read_eeprom(29);
   a1 = read_eeprom(30);
   a2 = read_eeprom(31);
   a3 = read_eeprom(32);
   a4 = read_eeprom(33);
   a5 = read_eeprom(34);
   a6 = read_eeprom(35);
   a7 = read_eeprom(36);
   a8 = read_eeprom(37);
   a9 = read_eeprom(38);
   a10 = read_eeprom(39);
   
   while(1)
   { 
      if(setTime & debounce & !alarmFlag & !Flag)
      {
         timeFlag = TRUE;
         debounce = FALSE;
         btn_count += 1;
         set = TRUE;
         lcd_gotoxy(1,1);
         printf(lcd_putc,"                ",);
         lcd_gotoxy(1,0);
         printf(lcd_putc,"                ",);
      }
      else if (!setTime & (!debounce)) debounce = TRUE;
      
      switch(btn_count)
         {
            case 0:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Set Minutes:"); 
               if(incr & (a==1)) { a = 0; setMin++; }
               else if (!incr & (a==0)) a = 1;
               if (decr & (b==1)) { b = 0; setMin--; }
               else if (!decr & (b==0)) b = 1;
               if(setMin > 59) setMin = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "%u ", setMin);           
               break;
            
            case 1:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Set Hours:"); 
               if(incr & (a==1)) { a = 0; setHours++; }
               else if (!incr & (a==0)) a = 1;
               if (decr & (b==1)) { b = 0; setHours--; }
               else if (!decr & (b==0)) b = 1;
               if(setHours > 12 || setHours < 1) setHours = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "%u ", setHours);
               break;
               
            case 2:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Set Seconds:"); 
               if(incr & (a==1)) { a = 0; setSec++; }
               else if (!incr & (a==0)) a = 1;
               if (decr & (b==1)) { b = 0; setSec--; }
               else if (!decr & (b==0)) b = 1;
               if(setSec > 59) setSec = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "%u ", setSec);
               break;
               
            case 3:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Set AM/PM:"); 
               if(incr & (a==1)) { a = 0; setAMPM = 1; }
               else if (!incr & (a==0)) a = 1;
               if (decr & (b==1)) { b = 0; setAMPM = 0; }
               else if (!decr & (b==0)) b = 1;
               if(setAMPM > 1) setAMPM = 1;
               lcd_gotoxy(1,0);
               if(setAMPM) printf(lcd_putc, "PM ");
               else if(!setAMPM) printf(lcd_putc, "AM ");
               break;
               
            case 4:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Set Date:"); 
               if(incr & (a==1)) { a = 0; setDate++; }
               else if (!incr & (a==0)) a = 1;
               if (decr & (b==1)) { b = 0; setDate--; }
               else if (!decr & (b==0)) b = 1;
               if(setDate > 31 || setDate < 1) setDate = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "%u ", setDate);
               break;
               
            case 5:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Set Month:"); 
               if(incr & (a==1)) { a = 0; setMonth++; }
               else if (!incr & (a==0)) a = 1;
               if (decr & (b==1)) { b = 0; setMonth--; }
               else if (!decr & (b==0)) b = 1;
               if(setMonth > 12 || setMonth < 1) setMonth = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "%u ", setMonth);
               break;
               
            case 6:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Set Year:"); 
               if(incr & (a==1)) { a = 0; setYear++; }
               else if (!incr & (a==0)) a = 1;
               if (decr & (b==1)) { b = 0; setYear--; }
               else if (!decr & (b==0)) b = 1;
               if(setYear > 99) setYear = 99;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "%u ", setYear);
               break;
            
            case 7:
               setMin = ((setMin/10) << 4) + (setMin % 10);
               if(setAMPM == 1)
               {
                  if(setHours == 10) setHours = 112;
                  else if(setHours == 11) setHours = 113;
                  else if(setHours == 12) setHours = 114;
                  else setHours = 0b01100000 | setHours; 
               }
               else if(setAMPM == 0)
               {
                  if(setHours == 10) setHours = 80;
                  else if(setHours == 11) setHours = 81;
                  else if(setHours == 12) setHours = 82;
                  else setHours = 0b01000000 | setHours; 
               }
               setDate = ((setDate/10) << 4) + (setDate % 10);
               setMonth = ((setMonth/10) << 4) + (setMonth % 10);
               setYear = ((setYear/10) << 4) + (setYear % 10);
               initRTC(setDate, setMonth, setYear, 1, setSec, setMin, setHours);
               btn_count = -1;
               set = FALSE;
               timeFlag = FALSE;
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Time/Date Set");
               delay_ms(2000);
               break;
               
            default:
               btn_count = -1;
               break;
         }

      if(setAlarm & _debounce & !timeFlag & !Flag)
      {
         _debounce = FALSE;
         alarmFlag = TRUE;
         alm_count += 1;
         set = TRUE;
         lcd_gotoxy(1,1);
         printf(lcd_putc,"                ",);
         lcd_gotoxy(1,0);
         printf(lcd_putc,"                ",);
      }
      else if (!setAlarm & (!_debounce)) _debounce = TRUE;
      
      switch(alm_count)
         {
            case 0:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 1:"); 
               if(incr & (a==1)) { a = 0; _setmin[0]++; }
               else if (!incr & (a==0)) a = 1;
               if (decr & (b==1)) { b = 0; _setmin[0]--; }
               else if (!decr & (b==0)) b = 1;
               if(_setmin[0] > 59) _setmin[0] = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Min: %u ", _setmin[0]);   
               break;
            
            case 1:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 1:"); 
               if(incr & (c==1)) { c = 0; sethour[0]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; sethour[0]--; }
               else if (!decr & (d==0)) d = 1;
               if(sethour[0] > 12 || sethour[0] < 1) sethour[0] = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Hrs: %u ", sethour[0]);
               break;
            
            case 2:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 1:"); 
               if(incr & (c==1)) { c = 0; ap[0] = 1; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; ap[0] = 0; }
               else if (!decr & (d==0)) d = 1;
               lcd_gotoxy(1,0);
               if(ap[0]) printf(lcd_putc, "PM ");
               else if(!ap[0]) printf(lcd_putc, "AM ");
               break;
            
            case 3:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 2:"); 
               if(incr & (c==1)) { c = 0; _setmin[1]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; _setmin[1]--; }
               else if (!decr & (d==0)) d = 1;
               if(_setmin[1] > 59) _setmin[1] = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Min: %u ", _setmin[1]);
               break;
               
            case 4:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 2:"); 
               if(incr & (c==1)) { c = 0; sethour[1]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; sethour[1]--; }
               else if (!decr & (d==0)) d = 1;
               if(sethour[1] > 12 || sethour[1] < 1) sethour[1] = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Hrs: %u ", sethour[1]);
               break;
            
            case 5:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 2:"); 
               if(incr & (c==1)) { c = 0; ap[1] = 1; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; ap[1] = 0; }
               else if (!decr & (d==0)) d = 1;
               lcd_gotoxy(1,0);
               if(ap[1]) printf(lcd_putc, "PM ");
               else if(!ap[1]) printf(lcd_putc, "AM ");
               break;
               
            case 6:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 3:"); 
               if(incr & (c==1)) { c = 0; _setmin[2]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; _setmin[2]--; }
               else if (!decr & (d==0)) d = 1;
               if(_setmin[2] > 59) _setmin[2] = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Min: %u ", _setmin[2]);
               break;
               
            case 7:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 3:"); 
               if(incr & (c==1)) { c = 0; sethour[2]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; sethour[2]--; }
               else if (!decr & (d==0)) d = 1;
               if(sethour[2] > 12 || sethour[2] < 1) sethour[2] = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Hrs: %u ", sethour[2]);
               break;
               
            case 8:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 3:"); 
               if(incr & (c==1)) { c = 0; ap[2] = 1; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; ap[2] = 0; }
               else if (!decr & (d==0)) d = 1;
               lcd_gotoxy(1,0);
               if(ap[2]) printf(lcd_putc, "PM ");
               else if(!ap[2]) printf(lcd_putc, "AM ");
               break;
               
            case 9:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 4:"); 
               if(incr & (c==1)) { c = 0; _setmin[3]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; _setmin[3]--; }
               else if (!decr & (d==0)) d = 1;
               if(_setmin[3] > 59) _setmin[3] = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Min: %u ", _setmin[3]);
               break;
               
            case 10:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 4:"); 
               if(incr & (c==1)) { c = 0; sethour[3]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; sethour[3]--; }
               else if (!decr & (d==0)) d = 1;
               if(sethour[3] > 12 || sethour[3] < 1) sethour[3] = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Hrs: %u ", sethour[3]);
               break;
               
            case 11:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 4:"); 
               if(incr & (c==1)) { c = 0; ap[3] = 1; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; ap[3] = 0; }
               else if (!decr & (d==0)) d = 1;
               lcd_gotoxy(1,0);
               if(ap[3]) printf(lcd_putc, "PM ");
               else if(!ap[3]) printf(lcd_putc, "AM ");
               break;
               
             case 12:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 5:"); 
               if(incr & (c==1)) { c = 0; _setmin[4]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; _setmin[4]--; }
               else if (!decr & (d==0)) d = 1;
               if(_setmin[4] > 59) _setmin[4] = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Min: %u ", _setmin[4]);
               break;
               
            case 13:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 5:"); 
               if(incr & (c==1)) { c = 0; sethour[4]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; sethour[4]--; }
               else if (!decr & (d==0)) d = 1;
               if(sethour[4] > 12 || sethour[4] < 1) sethour[4] = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Hrs: %u ", sethour[4]);
               break;
               
            case 14:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 5:"); 
               if(incr & (c==1)) { c = 0; ap[4] = 1; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; ap[4] = 0; }
               else if (!decr & (d==0)) d = 1;
               lcd_gotoxy(1,0);
               if(ap[4]) printf(lcd_putc, "PM ");
               else if(!ap[4]) printf(lcd_putc, "AM ");
               break;
               
             case 15:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 6:"); 
               if(incr & (c==1)) { c = 0; _setmin[5]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; _setmin[5]--; }
               else if (!decr & (d==0)) d = 1;
               if(_setmin[5] > 59) _setmin[5] = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Min: %u ", _setmin[5]);
               break;
               
            case 16:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 6:"); 
               if(incr & (c==1)) { c = 0; sethour[5]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; sethour[5]--; }
               else if (!decr & (d==0)) d = 1;
               if(sethour[5] > 12 || sethour[5] < 1) sethour[5] = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Hrs: %u ", sethour[5]);
               break;
               
            case 17:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 6:"); 
               if(incr & (c==1)) { c = 0; ap[5] = 1; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; ap[5] = 0; }
               else if (!decr & (d==0)) d = 1;
               lcd_gotoxy(1,0);
               if(ap[5]) printf(lcd_putc, "PM ");
               else if(!ap[5]) printf(lcd_putc, "AM ");
               break;
               
             case 18:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 7:"); 
               if(incr & (c==1)) { c = 0; _setmin[6]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; _setmin[6]--; }
               else if (!decr & (d==0)) d = 1;
               if(_setmin[6] > 59) _setmin[6] = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Min: %u", _setmin[6]);
               break;
               
            case 19:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 7:"); 
               if(incr & (c==1)) { c = 0; sethour[6]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; sethour[6]--; }
               else if (!decr & (d==0)) d = 1;
               if(sethour[6] > 12 || sethour[6] < 1) sethour[6] = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Hrs: %u ", sethour[6]);
               break;
               
            case 20:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 7:"); 
               if(incr & (c==1)) { c = 0; ap[6] = 1; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; ap[6] = 0; }
               else if (!decr & (d==0)) d = 1;
               lcd_gotoxy(1,0);
               if(ap[6]) printf(lcd_putc, "PM ");
               else if(!ap[6]) printf(lcd_putc, "AM ");
               break;
               
             case 21:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 8:"); 
               if(incr & (c==1)) { c = 0; _setmin[7]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; _setmin[7]--; }
               else if (!decr & (d==0)) d = 1;
               if(_setmin[7] > 59) _setmin[7] = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Min: %u ", _setmin[7]);
               break;
               
            case 22:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 8:"); 
               if(incr & (c==1)) { c = 0; sethour[7]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; sethour[7]--; }
               else if (!decr & (d==0)) d = 1;
               if(sethour[7] > 12 || sethour[7] < 1) sethour[7] = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Hrs: %u ", sethour[7]);
               break;
               
            case 23:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 8:"); 
               if(incr & (c==1)) { c = 0; ap[7] = 1; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; ap[7] = 0; }
               else if (!decr & (d==0)) d = 1;
               lcd_gotoxy(1,0);
               if(ap[7]) printf(lcd_putc, "PM ");
               else if(!ap[7]) printf(lcd_putc, "AM ");
               break;
               
             case 24:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 9:"); 
               if(incr & (c==1)) { c = 0; _setmin[8]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; _setmin[8]--; }
               else if (!decr & (d==0)) d = 1;
               if(_setmin[8] > 59) _setmin[8] = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Min: %u ", _setmin[8]);
               break;
               
            case 25:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 9:"); 
               if(incr & (c==1)) { c = 0; sethour[8]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; sethour[8]--; }
               else if (!decr & (d==0)) d = 1;
               if(sethour[8] > 12 || sethour[8] < 1) sethour[8] = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Hrs: %u ", sethour[8]);
               break;
               
            case 26:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 9:"); 
               if(incr & (c==1)) { c = 0; ap[8] = 1; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; ap[8] = 0; }
               else if (!decr & (d==0)) d = 1;
               lcd_gotoxy(1,0);
               if(ap[8]) printf(lcd_putc, "PM ");
               else if(!ap[8]) printf(lcd_putc, "AM ");
               break;
               
             case 27:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 10:"); 
               if(incr & (c==1)) { c = 0; _setmin[9]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; _setmin[9]--; }
               else if (!decr & (d==0)) d = 1;
               if(_setmin[9] > 59) _setmin[9] = 0;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Min: %u ", _setmin[9]);
               break;
               
            case 28:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 10:"); 
               if(incr & (c==1)) { c = 0; sethour[9]++; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; sethour[9]--; }
               else if (!decr & (d==0)) d = 1;
               if(sethour[9] > 12 || sethour[9] < 1) sethour[9] = 1;
               lcd_gotoxy(1,0);
               printf(lcd_putc, "Hrs: %u ", sethour[9]);
               break;
               
            case 29:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 10:"); 
               if(incr & (c==1)) { c = 0; ap[9] = 1; }
               else if (!incr & (c==0)) c = 1;
               if (decr & (d==1)) { d = 0; ap[9] = 0; }
               else if (!decr & (d==0)) d = 1;
               lcd_gotoxy(1,0);
               if(ap[9]) printf(lcd_putc, "PM ");
               else if(!ap[9]) printf(lcd_putc, "AM ");
               break;
               
            case 30:
               write_eeprom(0,_setmin[0]);
               write_eeprom(1,sethour[0]);
               write_eeprom(2,ap[0]);
               write_eeprom(3,_setmin[1]);
               write_eeprom(4,sethour[1]);
               write_eeprom(5,ap[1]);
               write_eeprom(6,_setmin[2]);
               write_eeprom(7,sethour[2]);
               write_eeprom(8,ap[2]);
               write_eeprom(9,_setmin[3]);
               write_eeprom(10,sethour[3]);
               write_eeprom(11,ap[3]);
               write_eeprom(12,_setmin[4]);
               write_eeprom(13,sethour[4]);
               write_eeprom(14,ap[4]);
               write_eeprom(15,_setmin[5]);
               write_eeprom(16,sethour[5]);
               write_eeprom(17,ap[5]);
               write_eeprom(18,_setmin[6]);
               write_eeprom(19,sethour[6]);
               write_eeprom(20,ap[6]);
               write_eeprom(21,_setmin[7]);
               write_eeprom(22,sethour[7]);
               write_eeprom(23,ap[7]);
               write_eeprom(24,_setmin[8]);
               write_eeprom(25,sethour[8]);
               write_eeprom(26,ap[8]);
               write_eeprom(27,_setmin[9]);
               write_eeprom(28,sethour[9]);
               write_eeprom(29,ap[9]);
  
               alm_count = -1;
               set = FALSE;
               alarmFlag = FALSE;
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarms Set");
               delay_ms(2000);
               break;
               
            default:
               alm_count = -1;
               break;
         }
         
         if(setFlag & __debounce & !timeFlag & !alarmFlag)
         {
            __debounce = FALSE;
            Flag = TRUE;
            flag_count += 1;
            set = TRUE;
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                ",);
            lcd_gotoxy(1,0);
            printf(lcd_putc,"                ",);
         }
         else if (!setFlag & (!__debounce)) __debounce = TRUE;
         
         switch(flag_count)
         {
            case 0:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 1:"); 
               if(incr & (e==1)) { e = 0; a1=TRUE; }
               else if (!incr & (e==0)) e = 1;
               if (decr & (f==1)) { f = 0; a1=FALSE; }
               else if (!decr & (f==0)) f = 1;
               lcd_gotoxy(1,0);
               if(a1) printf(lcd_putc, "ACT ");
               else if(!a1) printf(lcd_putc, "DACT");         
               break;
            
            case 1:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 2:"); 
               if(incr & (e==1)) { e = 0; a2=TRUE; }
               else if (!incr & (e==0)) e = 1;
               if (decr & (f==1)) { f = 0; a2=FALSE; }
               else if (!decr & (f==0)) f = 1;
               lcd_gotoxy(1,0);
               if(a2) printf(lcd_putc, "ACT ");
               else if(!a2) printf(lcd_putc, "DACT"); 
               break;
            
            case 2:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 3:"); 
               if(incr & (e==1)) { e = 0; a3=TRUE; }
               else if (!incr & (e==0)) e = 1;
               if (decr & (f==1)) { f = 0; a3=FALSE; }
               else if (!decr & (f==0)) f = 1;
               lcd_gotoxy(1,0);
               if(a3) printf(lcd_putc, "ACT ");
               else if(!a3) printf(lcd_putc, "DACT"); 
               break;
            
            case 3:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 4:"); 
               if(incr & (e==1)) { e = 0; a4=TRUE; }
               else if (!incr & (e==0)) e = 1;
               if (decr & (f==1)) { f = 0; a4=FALSE; }
               else if (!decr & (f==0)) f = 1;
               lcd_gotoxy(1,0);
               if(a4) printf(lcd_putc, "ACT ");
               else if(!a4) printf(lcd_putc, "DACT"); 
               break;
               
            case 4:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 5:"); 
               if(incr & (e==1)) { e = 0; a5=TRUE; }
               else if (!incr & (e==0)) e = 1;
               if (decr & (f==1)) { f = 0; a5=FALSE; }
               else if (!decr & (f==0)) f = 1;
               lcd_gotoxy(1,0);
               if(a5) printf(lcd_putc, "ACT ");
               else if(!a5) printf(lcd_putc, "DACT"); 
               break;
            
            case 5:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 6:"); 
               if(incr & (e==1)) { e = 0; a6=TRUE; }
               else if (!incr & (e==0)) e = 1;
               if (decr & (f==1)) { f = 0; a6=FALSE; }
               else if (!decr & (f==0)) f = 1;
               lcd_gotoxy(1,0);
               if(a6) printf(lcd_putc, "ACT ");
               else if(!a6) printf(lcd_putc, "DACT"); 
               break;
               
            case 6:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 7:"); 
               if(incr & (e==1)) { e = 0; a7=TRUE; }
               else if (!incr & (e==0)) e = 1;
               if (decr & (f==1)) { f = 0; a7=FALSE; }
               else if (!decr & (f==0)) f = 1;
               lcd_gotoxy(1,0);
               if(a7) printf(lcd_putc, "ACT ");
               else if(!a7) printf(lcd_putc, "DACT");
               break;
               
            case 7:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 8:"); 
               if(incr & (e==1)) { e = 0; a8=TRUE; }
               else if (!incr & (e==0)) e = 1;
               if (decr & (f==1)) { f = 0; a8=FALSE; }
               else if (!decr & (f==0)) f = 1;
               lcd_gotoxy(1,0);
               if(a8) printf(lcd_putc, "ACT ");
               else if(!a8) printf(lcd_putc, "DACT");
               break;
               
            case 8:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 9:"); 
               if(incr & (e==1)) { e = 0; a9=TRUE; }
               else if (!incr & (e==0)) e = 1;
               if (decr & (f==1)) { f = 0; a9=FALSE; }
               else if (!decr & (f==0)) f = 1;
               lcd_gotoxy(1,0);
               if(a9) printf(lcd_putc, "ACT ");
               else if(!a9) printf(lcd_putc, "DACT");
               break;
               
            case 9:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarm 10:"); 
               if(incr & (e==1)) { e = 0; a10=TRUE; }
               else if (!incr & (e==0)) e = 1;
               if (decr & (f==1)) { f = 0; a10=FALSE; }
               else if (!decr & (f==0)) f = 1;
               lcd_gotoxy(1,0);
               if(a10) printf(lcd_putc, "ACT ");
               else if(!a10) printf(lcd_putc, "DACT");
               break;
               
            case 10:
               write_eeprom(30,a1);
               write_eeprom(31,a2);
               write_eeprom(32,a3);
               write_eeprom(33,a4);
               write_eeprom(34,a5);
               write_eeprom(35,a6);
               write_eeprom(36,a7);
               write_eeprom(37,a8);
               write_eeprom(38,a9);
               write_eeprom(39,a10);
               
               flag_count = -1;
               set = FALSE;
               Flag = FALSE;
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Alarms Activated");
               delay_ms(2000);
               break;
               
            default:
               flag_count = -1;
               break;
         }
      
      i2c_start();
      i2c_write(RTC_WriteMode);
      i2c_write(RTC_Sec);
      
      i2c_start();
      i2c_write(RTC_ReadMode);

      rtcSec = bcd2dec(i2c_read(1));
      rtcMin = bcd2dec(i2c_read(1));
      rtcHours = i2c_read(1);
      rtcDay = bcd2dec(i2c_read(1));
      rtcDate = bcd2dec(i2c_read(1));
      rtcMonth = bcd2dec(i2c_read(1));
      rtcYear = bcd2dec(i2c_read(0));
      

      i2c_stop();
      
      AMPM = (rtcHours & 0x20) >> 5;
      rtcHours = bcd2dec(rtcHours & 0x1F);
      
      if(a1)
      {
         if(rtcHours == sethour[0] && rtcMin == _setmin[0] && rtcSec == 0 && AMPM == ap[0])
         {
            output_high(PIN_A0);
            setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
         }
      }
      
      if(a2)
      {
         if(rtcHours == sethour[1] && rtcMin == _setmin[1] && rtcSec == 0 && AMPM == ap[1])
         {
            output_high(PIN_A0);
            setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
         }
      }
      
      if(a3)
      {
         if(rtcHours == sethour[2] && rtcMin == _setmin[2] && rtcSec == 0 && AMPM == ap[2])
         {
            output_high(PIN_A0);
            setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
         }
      }
      
      if(a4)
      {
         if(rtcHours == sethour[3] && rtcMin == _setmin[3] && rtcSec == 0 && AMPM == ap[3])
         {
            output_high(PIN_A0);
            setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
         }
      }
      
      if(a5)
      {
         if(rtcHours == sethour[4] && rtcMin == _setmin[4] && rtcSec == 0 && AMPM == ap[4])
         {
            output_high(PIN_A0);
            setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
         }
      }
      
      if(a6)
      {
         if(rtcHours == sethour[5] && rtcMin == _setmin[5] && rtcSec == 0 && AMPM == ap[5])
         {
            output_high(PIN_A0);
            setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
         }
      }
      
      if(a7)
      {
         if(rtcHours == sethour[6] && rtcMin == _setmin[6] && rtcSec == 0 && AMPM == ap[6])
         {
            output_high(PIN_A0);
            setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
         }
      }
      
      if(a8)
      {
         if(rtcHours == sethour[7] && rtcMin == _setmin[7] && rtcSec == 0 && AMPM == ap[7])
         {
            output_high(PIN_A0);
            setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
         }
      }
      
      if(a9)
      {
         if(rtcHours == sethour[8] && rtcMin == _setmin[8] && rtcSec == 0 && AMPM == ap[8])
         {
            output_high(PIN_A0);
            setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
         }
      }
      
      if(a10)
      {
         if(rtcHours == sethour[9] && rtcMin == _setmin[9] && rtcSec == 0 && AMPM == ap[9])
         {
            output_high(PIN_A0);
            setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
         }
      }
      
      else if(rtcHours == 5 && rtcMin == 10 && rtcSec == 0 && AMPM == 1)
      {
         output_high(PIN_A0);
         delay_ms(1000);
         output_low(PIN_A0);
         delay_ms(1000);
         output_high(PIN_A0);
         delay_ms(1000);
         output_low(PIN_A0);
         delay_ms(1000);
         output_high(PIN_A0);
         delay_ms(1000);
         output_low(PIN_A0);
      } 
      
      if(!set)
      {
         if(AMPM == 1)
         {
            lcd_gotoxy(1,0);
            printf(lcd_putc, "Time:%u:%u:%u %c%c \n", rtcHours, rtcMin, rtcSec, PM[0], PM[1]);      
         }
      
         else
         {
            lcd_gotoxy(1,0);
            printf(lcd_putc, "Time:%u:%u:%u %c%c \n", rtcHours, rtcMin, rtcSec, AM[0], AM[1]);
         }  
          
         lcd_gotoxy(1,1);
         printf(lcd_putc, "Date: %u/%u/%u   ", rtcDate, rtcMonth, rtcYear);  
      }
   }
}
