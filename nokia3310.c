#include  <msp430g2231.h>

#define SEND_CMD                   0
#define SEND_CHR                   1

#define LCD_X_RES                  84
#define LCD_Y_RES                  48

#define COUNTDOWN				   1

// defines for 4250 pin connections to Nokia 3310
#define SCEPORT P1OUT
#define SDINPORT P1OUT
#define DCPORT P1OUT
#define SCKPORT P1OUT
#define RESPORT P1OUT
#define GNDPORT P1OUT
#define CARDPORT P1OUT
#define CARD  BIT6
#define GND	 BIT0
#define SCE  BIT1
#define SDIN BIT4
#define DC   BIT3
#define SCK  BIT5
#define RES  BIT2

void LCDSend(unsigned char,unsigned char);
void LCDClear(void);
void LCDInit(void);
void LCDBlack(void);
void updateDisplayString(void);
void LCDChar(unsigned char);
void LCDCurs(unsigned char,unsigned char);
void LCDString(unsigned char*,unsigned int, unsigned int, unsigned int);
void updateTime(void);
void LCDDot(void);
void LCDHour(void);

short second, minute, hour, day, subsec = 0;

unsigned char displayString[14] = {
	0x00,0x00,0x0a,0x00,0x00,0x0a,0x00,0x00,0x0a,0x00,0x00,0x0a,0x00,0x00
};

const unsigned char ofYouth[][41]={
{
  0xFC,0xFF,0xFF,0x07,0x03,0x03,0x06,0x1E,0xFC,0xF0,0x80,
  0x00,0x00,0x60,0x30,0x30,0x30,0x30,0xE0,0xE0,0x80,0x00,
  0x70,0xF0,0xF0,0x00,0x00,0x00,0x00,0xF0,0xF0,0x00,0x00,
  0xC0,0xE0,0x20,0x30,0x30,0x70,0x60
},
{
  0x7F,0xFF,0xFF,0xE0,0xC0,0xC0,0xE0,0x70,0x7F,0x3F,0x0F,
  0x00,0x30,0x7C,0x6E,0xC6,0xC2,0xC2,0xE3,0xFF,0xFF,0x00,
  0xE0,0xE1,0x67,0x7F,0x3C,0x1C,0x0F,0x07,0x00,0x00,0x00,
  0x70,0x63,0xC3,0xC7,0xE6,0x7E,0x7C,0x18
}
};

const unsigned char nums[][2][10] ={
 {// 0
   {0xE0,0xFC,0xFE,0x06,0x03,0x03,0x06,0xFE,0xFC,0xE0},
   {0x07,0x3F,0x7F,0x70,0xE0,0xE0,0x70,0x7F,0x3F,0x07}
 },
 {// 1
   {0x00,0x00,0x00,0x04,0x06,0xFE,0xFF,0xFF,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0x00,0x00}
 },
 {// 2
   {0x00,0x00,0x1C,0x0E,0x07,0x83,0x87,0xFE,0xFE,0x38},
   {0x00,0x60,0xFC,0xFE,0xEF,0xE7,0xE3,0xE1,0x60,0x30}
 },
 {// 3
   {0x00,0x00,0x0E,0x0E,0xC6,0xE6,0xF6,0xBE,0x1E,0x06},
   {0x00,0x60,0x70,0xF0,0xE0,0xE0,0xF1,0x7F,0x3F,0x0E}
 },
 {// 4
   {0x00,0xC0,0xE0,0x78,0x1C,0x0E,0x86,0xE2,0x10,0x00},
   {0x06,0x07,0x07,0x06,0x06,0xE6,0xFF,0xFF,0x0E,0x0E}
 },
 {// 5
   {0x00,0x70,0xF8,0xFE,0xCE,0xC6,0xC6,0x86,0x86,0x00},
   {0x00,0x30,0x71,0x71,0xE0,0xE0,0xF0,0x7F,0x3F,0x0E}
 },
 {// 6
   {0xC0,0xF8,0xFC,0x8E,0x86,0xC7,0xC3,0x87,0x82,0x00},
   {0x0F,0x3F,0x7F,0x71,0xE0,0xE0,0x61,0x7B,0x3F,0x0E}
 },
 {// 7
   {0x00,0x00,0x0E,0x0E,0x06,0x86,0xE6,0x7E,0x1E,0x0E},
   {0x00,0x00,0x00,0x38,0x7F,0xFF,0xE1,0x40,0x00,0x00}
 },
 {// 8
   {0x18,0x7E,0xE6,0xC3,0xC3,0xC3,0xE6,0xBE,0x18,0x00},
   {0x18,0x7E,0x7F,0xE1,0xE1,0xE1,0xF3,0x7F,0x7F,0x1C}
 },
 {// 9
   {0xFC,0xCE,0x06,0x03,0x03,0x07,0x86,0xFE,0xFC,0xE0},
   {0x41,0xE3,0xE3,0xE3,0x63,0x73,0x79,0x3F,0x1F,0x03}
 }
};

void updateTime( void)
{
  second = (second -1);
  if ( second == -1 )
  { // new minute
  	second = 59;
    minute = (minute - 1);
    if ( minute == -1)
    { // new hour
      minute = 59;
      hour = (hour -1);
      if ( hour == -1)
      { // new day
      	hour = 23;
      	day = (day -1);
      	displayString[0] = day / 10;
      	displayString[1] = day % 10;
      }
      displayString[3] = hour   / 10;
      displayString[4] = hour   % 10;
    }
    displayString[6] = minute / 10;
    displayString[7] = minute % 10; 
  }
  displayString[9] = second / 10;
  displayString[10] = second % 10;
}

void LCDString(unsigned char *character, unsigned int start, unsigned int layer, unsigned int stop)
{
  int i;
  for(i=start;i<(stop+1);i++)
  {
    int j;
    for (j = 0; j < 10; j++)
    {
      LCDSend(nums[character[i]][layer][j], SEND_CHR);
    }
  }
}

void LCDSend(unsigned char data, unsigned char cd) {

  volatile unsigned char bits;
  unsigned short cnt=8;
  // assume clk is hi
  // Enable display controller (active low).
  SCEPORT &= ~SCE;  //RESET SCE

  // command or data
  if(cd == SEND_CHR) {
    DCPORT |= DC;  //set to send data
  }
  else {  // reset to send command
    DCPORT &= ~DC;
  }

  ///// SEND SPI /////
  bits=0x80; // bits is mask to select bit to send. select bit msb first
 
  //send data
  while (0<cnt--)
  {
    // put bit on line
    // cycle clock
    SCKPORT &= ~SCK;
    if ((data & bits)>0) SDINPORT |= SDIN; else SDINPORT &= ~SDIN;
    //Delay(1);
    SCKPORT |= SCK;
    //Delay(2);
    // SHIFT BIT MASK 1 right
    bits >>= 1;
  }
   
  // Disable display controller.
  SCEPORT |= SCE;

}

void LCDClear(void) {
  int i,j;
      
  LCDSend(0x80, SEND_CMD );
  LCDSend(0x40, SEND_CMD );
  
  for (i=0;i<6;i++)  // number of rows
    for (j=0;j<LCD_X_RES;j++)  // number of columns
      LCDSend(0x00, SEND_CHR);
}

void LCDInit(void)
{ // assume ports set up and initialized to output

  // Reset LCD
  SCEPORT &= ~SCE;          // RESET SCE to enable 
  // toggle RES
  RESPORT |= RES;           // Set RES
  char l;
  for(l=0;l<10;l++)
    l=l;
  RESPORT &= ~RES;          // reset RES
  for(l=0;l<100;l++)
    l=l;
  RESPORT |= RES;           // Set RES
  
  // Cycle Clock
  SCKPORT &= ~SCK;
  SCKPORT |= SCK;
 
 // Disable display controller.
  SCEPORT |= SCE;           // bring high to disable 
  
  for(l=0;l<100;l++)
    l=l;

  // Send sequence of command
  LCDSend( 0x21, SEND_CMD );  // LCD Extended Commands.
  LCDSend( 0xBf, SEND_CMD );  // Set LCD Vop (Contrast).
  LCDSend( 0x06, SEND_CMD );  // Set Temp coefficent to 2.
  LCDSend( 0x13, SEND_CMD );  // LCD bias mode 1:100.
  LCDSend( 0x20, SEND_CMD );  // LCD Standard Commands, Horizontal addressing mode.
  LCDSend( 0x08, SEND_CMD );  // LCD blank
  LCDSend( 0x0C, SEND_CMD );  // LCD in inverse mode.
  
  LCDClear();
 

}

void LCDCurs(unsigned char x, unsigned char y)
{
	LCDSend(0x40|(y&0x07),SEND_CMD);
	LCDSend(0x80|(x&0x7f),SEND_CMD);
}

void LCDDot()
{
  int lm;
  LCDSend(0x00,SEND_CHR);
  LCDSend(0x00,SEND_CHR);
  LCDSend(0x00,SEND_CHR);
  for(lm=0;lm<3;lm++)
    LCDSend(0x3c,SEND_CHR);
}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  subsec = (subsec + 1) % 32;
  if (subsec == 0)
  { // New second
  	updateTime();
  }
}

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;	// Stop WDT
  P1DIR = 0xFF;
  GNDPORT &= ~GND;
  CARDPORT |= CARD;
  
  BCSCTL1 = CALBC1_1MHZ;                   // Set range
  DCOCTL = CALDCO_1MHZ;                    // Set DCO step + modulation
  
  BCSCTL2 = 0xF8;
  BCSCTL3 = LFXT1S_0 + XCAP_3;
  
  subsec=0x00;
  second = 0x05;
  minute = 0x00;
  hour = 0x16;
  day = 0x03;
  CCTL0 = CCIE;
  CCR0 = 0;
  TACCR0 = 0x3FF;
  TACTL = 0x0211;
  
  LCDInit();
  
  _BIS_SR(GIE);
  
  LCDCurs(34,3);
  int lm;
  int lm2;
  for(lm=0;lm<2;lm++)
  {
  	for(lm2=0;lm2<41;lm2++)
  	{
      LCDSend(ofYouth[lm][lm2],SEND_CHR);
  	}
  	LCDCurs(34,4);
  }
  
  //LCDCurs(0,0);
  while(1)
  {
    if(subsec==0)
    {
      if(day==-1)
      {
      	day=minute=second=0;
      	hour=1;
      	CARDPORT &= ~CARD;
      } 
      if(minute==58)
      {
      	CARDPORT |= CARD;
      }
      LCDCurs(60,0);
      LCDString(displayString,9,0,10);
      LCDCurs(60,1);
      LCDString(displayString,9,1,10);
      if(second==59)
      {
      	LCDCurs(30,0);
        LCDString(displayString,6,0,7);
        LCDDot();
        LCDCurs(30,1);
        LCDString(displayString,6,1,7);
        LCDDot();
      }
      if(minute==59)
      {
      	LCDCurs(0,0);
      	LCDString(displayString,3,0,4);
        LCDDot();
      	LCDCurs(0,1);
      	LCDString(displayString,3,1,4);
        LCDDot();
      }
      if(hour==23)
      {
        LCDCurs(7,3);
        LCDString(displayString,0,0,1);
        LCDCurs(7,4);
        LCDString(displayString,0,1,1);
      }
    }
  }
}

