
#include <p32xxxx.h>
#include <plib.h>
#include "OledDriver.h"
#include <inttypes.h>
#include "delay.h"
#include <stdio.h>

// Oscillator Settings
#pragma config FNOSC		= PRIPLL	// Oscillator selection
#pragma config POSCMOD		= XT		// Primary oscillator mode
#pragma config FPLLIDIV 	= DIV_2		// PLL input divider
#pragma config FPLLMUL		= MUL_20	// PLL multiplier
#pragma config FPLLODIV 	= DIV_1		// PLL output divider
#pragma config FPBDIV		= DIV_1		// Peripheral bus clock divider
#pragma config FSOSCEN		= OFF		// Secondary oscillator enable

// Clock control settings
#pragma config IESO		= OFF		// Internal/external clock switchover
#pragma config FCKSM		= CSDCMD	// Clock switching (CSx)/Clock monitor (CMx)
#pragma config OSCIOFNC		= OFF		// Clock output on OSCO pin enable


//Other Peripheral Device settings

#pragma config FWDTEN		= OFF		// Watchdog timer enable
#pragma config WDTPS		= PS1024	// Watchdog timer postscaler

//Code Protection settings

#pragma config CP		= OFF		// Code protection
#pragma config BWP		= OFF		// Boot flash write protect
#pragma config PWP		= OFF		// Program flash write protect


//Local Type Definitions

#define	fTrue	1
#define fFalse	0
#define cbPageMax 256
#define _BOARD_UC32_ 1      // this is the board we use.
#define	cntMsDelay 10000    //timer 1 delay for 1ms

#define SYS_FREQ (80000000L)

#define	GetPeripheralClock()		(SYS_FREQ/(1 << OSCCONbits.PBDIV))
#define	GetInstructionClock()		(SYS_FREQ)

#if defined (__32MX340F512H__)
#define UART_MODULE_ID UART1 // use first UART.  ChipKit uC32 USB is on UART1
#endif


//config the switches!!!
#define sw_config() TRISD=0xFFFF // switches as inputs
#define sw1_up (_RD8) 
#define sw2_up (_RD9) 
#define sw3_up (_RD10) 
#define sw4_up (_RD11) 

//  Global Variables
extern BYTE rgbOledBmp[];
BYTE rgbBmp0[] = {
    0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81,
    0x18, 0x24, 0x42, 0x81, 0x81, 0x42, 0x24, 0x18
};
int testgraph[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8
};
BYTE rgbBmp1[32];
BYTE rgbUserFont[cbOledFontUser] = {
    0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7E, // 0x00
    0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81, // 0x01
    0x7E, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, // 0x02
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E, // 0x03
    0x7E, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, // 0x04
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x7E, // 0x05
    0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, // 0x06
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, // 0x07
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0x08
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, // 0x09
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // 0x0A
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // 0x0B
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, // 0x0C
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, // 0x0D
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, // 0x0E
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 0x0F
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x10
    0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x11
    0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x12
    0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, // 0x13
    0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, // 0x14
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, // 0x15
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, // 0x16
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF // 0x17
};
const BYTE rgbOledFont0[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x20, space
    0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x00, // 0x21, !
    0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, // 0x22, "
    0x64, 0x3c, 0x26, 0x64, 0x3c, 0x26, 0x24, 0x00, // 0x23, #
    0x26, 0x49, 0x49, 0x7f, 0x49, 0x49, 0x32, 0x00, // 0x23, $
    0x42, 0x25, 0x12, 0x08, 0x24, 0x52, 0x21, 0x00, // 0x25, %
    0x20, 0x50, 0x4e, 0x55, 0x22, 0x58, 0x28, 0x00, // 0x26, &
    0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, // 0x27, '
    0x00, 0x00, 0x1c, 0x22, 0x41, 0x00, 0x00, 0x00, // 0x28, (
    0x00, 0x00, 0x00, 0x41, 0x22, 0x1c, 0x00, 0x00, // 0x29, )
    0x00, 0x15, 0x15, 0x0e, 0x0e, 0x15, 0x15, 0x00, // 0x2A, *
    0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00, // 0x2B, +
    0x00, 0x00, 0x00, 0x50, 0x30, 0x00, 0x00, 0x00, // 0x2C, ,
    0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, // 0x2D, -
    0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, // 0x2E, .
    0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, // 0x2F, /
    0x00, 0x3e, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00, // 0x30, 0
    0x00, 0x00, 0x41, 0x7f, 0x40, 0x00, 0x00, 0x00, // 0x31, 1
    0x00, 0x42, 0x61, 0x51, 0x49, 0x6e, 0x00, 0x00, // 0x32, 2
    0x00, 0x22, 0x41, 0x49, 0x49, 0x36, 0x00, 0x00, // 0x33, 3
    0x00, 0x18, 0x14, 0x12, 0x7f, 0x10, 0x00, 0x00, // 0x33, 4
    0x00, 0x27, 0x49, 0x49, 0x49, 0x71, 0x00, 0x00, // 0x35, 5
    0x00, 0x3c, 0x4a, 0x49, 0x48, 0x70, 0x00, 0x00, // 0x36, 6
    0x00, 0x43, 0x21, 0x11, 0x0d, 0x03, 0x00, 0x00, // 0x37, 7
    0x00, 0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, // 0x38, 8
    0x00, 0x06, 0x09, 0x49, 0x29, 0x1e, 0x00, 0x00, // 0x39, 9
    0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00, // 0x3A, :
    0x00, 0x00, 0x00, 0x52, 0x30, 0x00, 0x00, 0x00, // 0x3B, //
    0x00, 0x00, 0x08, 0x14, 0x14, 0x22, 0x00, 0x00, // 0x3C, <
    0x00, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00, // 0x3D, =
    0x00, 0x00, 0x22, 0x14, 0x14, 0x08, 0x00, 0x00, // 0x3E, >
    0x00, 0x02, 0x01, 0x59, 0x05, 0x02, 0x00, 0x00, // 0x3F, ?
    0x3e, 0x41, 0x5d, 0x55, 0x4d, 0x51, 0x2e, 0x00, // 0x40, @
    0x40, 0x7c, 0x4a, 0x09, 0x4a, 0x7c, 0x40, 0x00, // 0x41, A
    0x41, 0x7f, 0x49, 0x49, 0x49, 0x49, 0x36, 0x00, // 0x42, B
    0x1c, 0x22, 0x41, 0x41, 0x41, 0x41, 0x22, 0x00, // 0x43, C
    0x41, 0x7f, 0x41, 0x41, 0x41, 0x22, 0x1c, 0x00, // 0x44, D
    0x41, 0x7f, 0x49, 0x49, 0x5d, 0x41, 0x63, 0x00, // 0x45, E
    0x41, 0x7f, 0x49, 0x09, 0x1d, 0x01, 0x03, 0x00, // 0x46, F
    0x1c, 0x22, 0x41, 0x49, 0x49, 0x3a, 0x08, 0x00, // 0x47, G
    0x41, 0x7f, 0x08, 0x08, 0x08, 0x7f, 0x41, 0x00, // 0x48, H
    0x00, 0x41, 0x41, 0x7F, 0x41, 0x41, 0x00, 0x00, // 0x49, I
    0x30, 0x40, 0x41, 0x41, 0x3F, 0x01, 0x01, 0x00, // 0x4A, J
    0x41, 0x7f, 0x08, 0x0c, 0x12, 0x61, 0x41, 0x00, // 0x4B, K
    0x41, 0x7f, 0x41, 0x40, 0x40, 0x40, 0x60, 0x00, // 0x4C, L
    0x41, 0x7f, 0x42, 0x0c, 0x42, 0x7f, 0x41, 0x00, // 0x4D, M
    0x41, 0x7f, 0x42, 0x0c, 0x11, 0x7f, 0x01, 0x00, // 0x4E, N
    0x1c, 0x22, 0x41, 0x41, 0x41, 0x22, 0x1c, 0x00, // 0x4F, O
    0x41, 0x7f, 0x49, 0x09, 0x09, 0x09, 0x06, 0x00, // 0x50, P
    0x0c, 0x12, 0x21, 0x21, 0x61, 0x52, 0x4c, 0x00, // 0x51, Q
    0x41, 0x7f, 0x09, 0x09, 0x19, 0x69, 0x46, 0x00, // 0x52, R
    0x66, 0x49, 0x49, 0x49, 0x49, 0x49, 0x33, 0x00, // 0x53, S
    0x03, 0x01, 0x41, 0x7f, 0x41, 0x01, 0x03, 0x00, // 0x54, T
    0x01, 0x3f, 0x41, 0x40, 0x41, 0x3f, 0x01, 0x00, // 0x55, U
    0x01, 0x0f, 0x31, 0x40, 0x31, 0x0f, 0x01, 0x00, // 0x56, V
    0x01, 0x1f, 0x61, 0x14, 0x61, 0x1f, 0x01, 0x00, // 0x57, W
    0x41, 0x41, 0x36, 0x08, 0x36, 0x41, 0x41, 0x00, // 0x58, X
    0x01, 0x03, 0x44, 0x78, 0x44, 0x03, 0x01, 0x00, // 0x59, Y
    0x43, 0x61, 0x51, 0x49, 0x45, 0x43, 0x61, 0x00, // 0x5A, Z
    0x00, 0x00, 0x7f, 0x41, 0x41, 0x00, 0x00, 0x00, // 0x5B, [
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, // 0x5C,
    0x00, 0x00, 0x41, 0x41, 0x7f, 0x00, 0x00, 0x00, // 0x5D, ]
    0x00, 0x04, 0x02, 0x01, 0x01, 0x02, 0x04, 0x00, // 0x5E, ^
    0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, // 0x5F, _
    0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x60, `
    0x00, 0x34, 0x4a, 0x4a, 0x4a, 0x3c, 0x40, 0x00, // 0x61, a
    0x00, 0x41, 0x3f, 0x48, 0x48, 0x48, 0x30, 0x00, // 0x62. b
    0x00, 0x3c, 0x42, 0x42, 0x42, 0x24, 0x00, 0x00, // 0x63, c
    0x00, 0x30, 0x48, 0x48, 0x49, 0x3f, 0x40, 0x00, // 0x63, d
    0x00, 0x3c, 0x4a, 0x4a, 0x4a, 0x2c, 0x00, 0x00, // 0x65, e
    0x00, 0x00, 0x48, 0x7e, 0x49, 0x09, 0x00, 0x00, // 0x66, f
    0x00, 0x26, 0x49, 0x49, 0x49, 0x3f, 0x01, 0x00, // 0x67, g
    0x41, 0x7f, 0x48, 0x04, 0x44, 0x78, 0x40, 0x00, // 0x68, h
    0x00, 0x00, 0x44, 0x7d, 0x40, 0x00, 0x00, 0x00, // 0x69, i
    0x00, 0x00, 0x40, 0x44, 0x3d, 0x00, 0x00, 0x00, // 0x6A, j
    0x41, 0x7f, 0x10, 0x18, 0x24, 0x42, 0x42, 0x00, // 0x6B, k
    0x00, 0x40, 0x41, 0x7f, 0x40, 0x40, 0x00, 0x00, // 0x6C, l
    0x42, 0x7e, 0x02, 0x7c, 0x02, 0x7e, 0x40, 0x00, // 0x6D, m
    0x42, 0x7e, 0x44, 0x02, 0x42, 0x7c, 0x40, 0x00, // 0x6E, n
    0x00, 0x3c, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00, // 0x6F, o
    0x00, 0x41, 0x7f, 0x49, 0x09, 0x09, 0x06, 0x00, // 0x70, p
    0x00, 0x06, 0x09, 0x09, 0x49, 0x7f, 0x41, 0x00, // 0x71, q
    0x00, 0x42, 0x7e, 0x44, 0x02, 0x02, 0x04, 0x00, // 0x72, r
    0x00, 0x64, 0x4a, 0x4a, 0x4a, 0x36, 0x00, 0x00, // 0x73, s
    0x00, 0x04, 0x3f, 0x44, 0x44, 0x20, 0x00, 0x00, // 0x73, t
    0x00, 0x02, 0x3e, 0x40, 0x40, 0x22, 0x7e, 0x40, // 0x75, u
    0x02, 0x0e, 0x32, 0x40, 0x32, 0x0e, 0x02, 0x00, // 0x76, v
    0x02, 0x1e, 0x62, 0x18, 0x62, 0x1e, 0x02, 0x00, // 0x77, w
    0x42, 0x62, 0x14, 0x08, 0x14, 0x62, 0x42, 0x00, // 0x78, x
    0x01, 0x43, 0x45, 0x38, 0x05, 0x03, 0x01, 0x00, // 0x79, y
    0x00, 0x46, 0x62, 0x52, 0x4a, 0x46, 0x62, 0x00, // 0x7A, z
    0x00, 0x00, 0x08, 0x36, 0x41, 0x00, 0x00, 0x00, // 0x7B, {
    0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, // 0x7C, |
    0x00, 0x00, 0x00, 0x41, 0x36, 0x08, 0x00, 0x00, // 0x7D, }
    0x00, 0x18, 0x08, 0x08, 0x10, 0x10, 0x18, 0x00, // 0x7E, ~
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 // 0x7F, DEL
};


//Local Variables								
BYTE rgbCmd[cbPageMax];
BYTE rgbBuf[cbPageMax];


//function prototypes
void setup();
void DeviceInit();
void AppInit();
void menu();
void UARTconfig();
UINT32 GetMenuChoice();
void SendDataBuffer(const char *buffer, UINT32 size);
UINT32 GetDataBuffer(char *buffer, UINT32 max_size);
void upload(int* times); // option 1: upload mode
void remind(int* appoint); // option 2: remind mode

// Constant Data
const char mainMenu[] =
{
    "\r\nPill Reminder and Scheduler\r\n"\
    "Choose an option\r\n"\
    "1. Upload Pill Reminder Schedule rate\r\n"\
    "2. Pill Reminder Mode\r\n"\
    "\r\n Choose a number\r\n"
};


//main

int main() {
    UINT32 menu_choice;
    UINT8 buf[1024];
    int schedule[4];

    setup();
    LATE = 0x01; //light LED when setup complete
    menu();
    delay(10);

    while (1) {
        menu(); // display the menu on the OLED screen
        SendDataBuffer(mainMenu, sizeof(mainMenu));
        menu_choice = GetMenuChoice(); //gets choice from user
        switch (menu_choice) {
            //if user types "1":
            case 1:
                OledClear();
                OledSetCursor(0, 0);
                OledPutString("uploading..");
                DelayMs(500);
                upload(schedule);
                SendDataBuffer("\r\n\r\nPress any key to continue\r\n", strlen("\r\n\r\nPress any key to continue\r\n"));
                GetMenuChoice();
                break;
                //if user types "2":
            case 2:
                OledClear();
                OledSetCursor(0, 0);
                OledPutString("reminder mode"); // display mode on OLED
                DelayMs(1500); // wait a bit
                remind(schedule); // begin with reminder mode!* 
                //*testing-> 480 is the amount of minutes after 00:00 when it's time to take pill
                break;
            default:
                SendDataBuffer(mainMenu, sizeof(mainMenu));
        }// switch
    }// while
} // main


//remind function -- displays the time and counts down 
void remind(int* appoint) {
    int hours, min, flag;
    char time[6]; //string for time
    char cdown[16]; //string for the count
    int count1 = appoint[0];
    int count2 = appoint[1];
    int count3 = appoint[2];
    int count4 = appoint[3];
    int zero=0;
    
    int i;
    int* count; // current number minutes left to take next pill 
    int min_left;
    int hour_left;
    
    LATE = 0;
    OledClear(); // clear the screen
    OledSetCharUpdate(1);

        hours = 0; // start clock at 00:00
        count = &count1;
        while (hours < 24) {
            flag = 0;
            min = 0; // reset minutes to 00
            while (min < 60) {
                min_left=(*count)%60;
                hour_left=(*count)/60;
                sprintf(time, "%02d:%02d", hours, min); // time= "hours:min" ==> at first: "00:00"
                sprintf(cdown, "%d hrs %d mins ", hour_left, min_left);

                // display time on OLED
                OledSetCursor(0, 0);
                OledPutString("Time:");
                OledSetCursor(7, 0);
                OledPutString(time);

                // display count on OLED
                OledSetCursor(0, 2); // next line
                OledPutString("next pill in:   ");
                OledSetCursor(0, 3);
                OledPutString(cdown); // display the countdown on screen
                DelayMs(60); // wait 60ms seconds
                min++; // update min (increases by one)

                // check the switch -- this part doesn't work properly--
                if (sw1_up) { // if switch is on, set flag
                    flag = 1;
                }
                if (sw2_up) { // if switch is on, set flag
                    flag = 2;
                }
                if (sw3_up) { // if switch is on, set flag
                    flag = 3;
                }
                if (sw4_up) { // if switch is on, set flag
                    flag = 4;
                }

                if (count4 == 0) {
                    // check of switch went up
                    if (flag != 4) {
                        LATE = 0xFF; // flash all LEDs if switch wasn't toggled
                        OledSetCursor(0, 2);
                        OledPutString("  missed pill-4 ");
                        for (i = 0; i < 19; i++) {
                            delay(2);
                            LATEINV = 0xFF;
                        }
                        LATE = 0x00;
                    }
                    count = &zero;
                } else if (count3 == 0) {
                    // check of switch went up
                    if (flag != 3) {
                        LATE = 0xFF; // flash all LEDs if switch wasn't toggled
                        OledSetCursor(0, 2);
                        OledPutString("  missed pill-3 ");
                        for (i = 0; i < 19; i++) {
                            delay(2);
                            LATEINV = 0xFF;
                        }
                        LATE = 0x00;
                    }
                    count = &count4;
                } else if (count2 == 0) {
                    // check of switch went up
                    if (flag != 2) {
                        LATE = 0xFF; // flash all LEDs if switch wasn't toggled
                        OledSetCursor(0, 2);
                        OledPutString("  missed pill-2 ");
                        for (i = 0; i < 19; i++) {
                            delay(2);
                            LATEINV = 0xFF;
                        }
                        LATE = 0x00;
                    }
                    count = &count3;
                } else if (count1 == 0) {
                    // check of switch went up
                    if (flag != 1) {
                        LATE = 0xFF; // flash all LEDs if switch wasn't toggled
                        OledSetCursor(0, 2);
                        OledPutString("  missed pill-1 ");
                        for (i = 0; i < 19; i++) {
                            delay(2);
                            LATEINV = 0xFF;
                        }
                        LATE = 0x00;
                    }

                    count = &count2;
                }

                count1--;
                count2--;
                count3--;
                count4--;

                if (min == 60)
                    hours++; // after 60 minutes, update hours (inc. by 1) 

            }// end of while(min<60)
            //if hours<24, reset min=0
        }// end of while(hours<24)
    }//end of remind

void upload(int* times) {
    int j, l, n;
    int m;
    int i;
    UINT32 temp;
    UINT8 buf[1024];
    UINT8 ibuf2[1024];

    sprintf(buf, "Enter the hours the pills need to be taken (press enter after each):\r\n");
    SendDataBuffer(buf, strlen(buf));

    for (l = 0; l < 4; l++) {
        m = 0;
        for (j = 0; j < 1024; j++) {
            ibuf2[j] = NULL;
        }
        temp = GetDataBuffer(ibuf2, 1024);
        SendDataBuffer(">>", strlen(">>"));
        SendDataBuffer(ibuf2, temp);
        SendDataBuffer("\r\n", strlen("\r\n"));
        for (n = 0; n < temp; n++) {
            m = (10 * m) + (ibuf2[n] - '0');
        }
        times[l] = m;
    }
    //convert times to number of minutes from midnight
    for (i = 0; i < 4; i++) {
        times[i] = times[i]*60;
    }

    OledClear();
    OledSetCursor(0, 0);
    OledPutString("uploaded!");
    DelayMs(500);
}

void setup() {
    // (using LEDs to track the the setup)
    TRISE = 0x00;
    LATE = 0x00;
    _LATE2 = 1;

    DeviceInit();
    _LATE1 = 1;

    AppInit();
    sw_config(); //switches as inputs   
    UARTconfig(); // UART configuration stuff
}

// first menu displayed on the OLED screen
void menu() {
    OledClearBuffer();
    OledClear();
    OledSetCursor(0, 0); // first line
    OledPutString("1. upload sch.");
    OledSetCursor(0, 2); // second line
    OledPutString("2. reminder mode");
    DelayMs(100);
}

// configure the UART
void UARTconfig() {
    UARTConfigure(UART_MODULE_ID, UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetFifoMode(UART_MODULE_ID, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART_MODULE_ID, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART_MODULE_ID, GetPeripheralClock(), 57600);
    UARTEnable(UART_MODULE_ID, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
}

//	Perform basic hardware initialization of the board.
void DeviceInit() {
    DelayInit();
    OledInit();
}

// Initialize the application.
void AppInit() {
    char ch;
    OledInit();
    OledClear();

    // Define the user definable characters
    for (ch = 0; ch < 0x18; ch++) {
        OledDefUserChar(ch, &rgbUserFont[ch * cbOledChar]);
    }
}

//some other functions from the example programs:
void delay(int amount) {
    int i = 0;

    while (i < (amount * 100000)) {
        i++;
    }
}

// Initialized the hardware for use by delay functions
void DelayInit() {
    unsigned int tcfg;
    //Configure Timer 1. This sets it up to count a 10Mhz.
    tcfg = T1_ON | T1_IDLE_CON | T1_SOURCE_INT | T1_PS_1_8 | T1_GATE_OFF | T1_SYNC_EXT_OFF;
    OpenTimer1(tcfg, 0xFFFF);
}

//Delay the requested number of milliseconds. Uses Timer1.
void DelayMs(int cms) {
    int ims;

    for (ims = 0; ims < cms; ims++) {
        WriteTimer1(0);
        while (ReadTimer1() < cntMsDelay);
    }
}

// writes stuff on the terminal (UART))
void SendDataBuffer(const char *buffer, UINT32 size) {
    while (size) {
        while (!UARTTransmitterIsReady(UART_MODULE_ID))
            ;

        UARTSendDataByte(UART_MODULE_ID, *buffer);

        buffer++;
        size--;
    }

    while (!UARTTransmissionHasCompleted(UART_MODULE_ID))
        ;
}

//gets input from the terminal, reads everything up until user presses enter
UINT32 GetDataBuffer(char *buffer, UINT32 max_size) {
    UINT32 num_char;

    num_char = 0;

    while (num_char < max_size) {
        UINT8 character;

        while (!UARTReceivedDataIsAvailable(UART_MODULE_ID))
            ;

        character = UARTGetDataByte(UART_MODULE_ID);

        if (character == '\r')
            break;

        *buffer = character;

        buffer++;
        num_char++;
    }

    return num_char;
}

// gets the choice from the initial menu, reads one character right away
UINT32 GetMenuChoice(void) {
    UINT8 menu_item;

    while (!UARTReceivedDataIsAvailable(UART_MODULE_ID))
        ;

    menu_item = UARTGetDataByte(UART_MODULE_ID);

    menu_item -= '0';

    return (UINT32) menu_item;
}
