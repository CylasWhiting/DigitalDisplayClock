/*
Written by Cylas Whiting
May 20th, 2025


Atmega 328p 
            Stats
---------------------------------
Total size: 1218 bytes
Global Variable Memory usage: 28
  Pin Constants
  Pin Name   Pin Desc.            Pin#  +-----+  Pin#        Pin Desc.      
                          /RESET  (  )--¦     ¦--(19)  C5    Digit 4         
  SER        Segment Data    RXD  ( 0)--¦     ¦--(18)  C4    Digit 3         
  OE         Chip Enable     TXD  ( 1)--¦  A  ¦--(17)  C3    Digit 2     
  Rck        Latch Pin        D2  ( 2)--¦  T  ¦--(16)  C2    Digit 1
  SRclk      Clock Pin        D3  ( 3)--¦  m  ¦--(15)  C1    LED    
                              D4  ( 4)--¦  e  ¦--(14)  C0      
  5VDC                       VCC  (  )--¦  g  ¦--(  )  GND
  GND                        GND  (  )--¦  a  ¦--(  )  AREF
                           XTAL1  (  )--¦  3  ¦--(  )  AVCC
                           XTAL2  (  )--¦  2  ¦--(13)  SCK   NotUsedPin[6]
  PA0                         D5  ( 5)--¦  8  ¦--(12)  MISO  NotUsedPin[5]
  J1, PB4     NotUsedPin[1]   D6  ( 6)--¦  P  ¦--(11)  MOSI  NotUsedPin[4]
  PB2                         D7  ( 7)--¦     ¦--(10)  B2        
  PB0         NotUsedPin[3]   B0  ( 8)--¦     ¦--( 9)  OC1A  
                                        +-----+                                                  */


unsigned long int seconds = 86380; //43200 base value, 86380 starts 20 seconds before time changes from 11:59 to 12:00
bool PM_time = false;  //True if it's PM, False if it's AM


/****************************************************************************************
ISR(vector)
Interrupt function tied to internal timer register.
Interrupts every 250ms
Parameters:             Description:              Range:
Vector                  Interrupt vector          N/A

Returns: void
*****************************************************************************************/

volatile long count = 0;
ISR(TIMER2_OVF_vect) {                  //Need more education on overflow vectors
  TCNT2 = 6; //start count of timer
  if (count >= 1000){ // 1 second 
    if (seconds == 86399) {   //~24 hours in seconds
      seconds = 43200;        //12 hours in seconds
      PM_time = !PM_time;
    } else {
      seconds++;
    }
    count = 0;
  } else {
    count++;
  }
}


static const uint8_t digitCodeMap[] = {
  //ABCDEFG    Segments      7-segment map:
  0b11111100, // 0   "0"          AAA
  0b01100000, // 1   "1"         F   B
  0b11011010, // 2   "2"         F   B
  0b11110010, // 3   "3"          GGG
  0b01100110, // 4   "4"         E   C
  0b10110110, // 5   "5"         E   C
  0b10111110, // 6   "6"          DDD
  0b11100000, // 7   "7"
  0b11111110, // 8   "8"
  0b11110110  // 9   "9"
};

void setup() {
  DDRC |= (31 << 1);  //00111110 (PC1, PC2, PC3, PC4, PC5 enabled for output)
  DDRD |= 15;         //00001111 (PD0, PD1, PD2, PD3 enabled for output)
  PORTC |= (15 << 2); //00111100 (All digits high (diplay off), LED low)
  PORTD &= (1 << 1);  //00000010 (OE LOW (Shift reg output on))
  
  /*****************CODE SECTION NOT FULLY UNDERSTOOD*********Written by Jai Krishna******************************************************/
  cli(); //Disable Global Interrupts
  TCCR2A = 0; //Normal Mode
  TCCR2B = 0;                              //??? why are 2 TCCR2 registers needed
  TIMSK2 |= (1 << TOIE2); //Enable overflow interrupt
  sei(); //Enable Global Interrupts
  TCNT2 = 6; //Load value
  TCCR2B |= (1 << CS22); // Prescaler set to 64
  /**************MORE EDUCATION ON TIMER REGISTERS NEEDED*****Source: https://textzip.github.io/posts/AVR-Diaries-03-Timers/**************/
}

void loop() {
  
  byte hours = seconds / 3600;              //Block converts seconds into hours and minutes
  byte minutes = (seconds % 3600) / 60;
  if (hours > 12) {
    hours -= 12;
  }
  byte digitValues[4];
  digitValues[0] = hours / 10;       //Block converts hours & minutes to individual digits
  digitValues[1] = hours % 10;
  digitValues[2] = minutes / 10;
  digitValues[3] = minutes % 10;
  for (byte i = 0; i < 4; i++) {
  flashDisplay(digitValues[i], i);   //lights up segment display
  delay(1);                          //keeps digit on for 1 millisecond to increase brightness
  }
  
}


/****************************************************************************************************
void flashDisplay(byte segValue, byte activeDigit)
Lights up the digit selected by activeDigit with data in segValue
Parameters:                   Description:               Range:
byte segValue         The number to appear on display.    0-9
byte activeDigit        The digit to display on.          0-3 

Returns: void
******************************************************************************************************/

void flashDisplay(byte segValue, byte activeDigit) {
  PORTC = (15 << 2);                 //00111100 Turns off all digits to prevent ghosting
  if (PM_time){
      PORTC |= (1 << 1);             //00000010 Turns on LED
    }
  
  byte mask = 0;
    for (int n = 0; n < 8; n++){
      byte mask = digitCodeMap[segValue];  //sets mask to segment byte code
      if (activeDigit == 1) {       //Turns on LED
      mask |= 1;                    //00000001
    }
      if (mask & (1<<n)){           //Checks against the segment array to see if the bit (segment) is high 
        PORTD |= 1;                 //00000001 if true, send 1 to the SER PIN               
      } else {
        PORTD &= ~(1);              //11111110 if false, send 0 to the SER PIN
      }
      PORTD ^= (1 << 3);            //00001000 flips SRclk to high
      PORTD ^= (1 << 3);            //00001000 flips SRclk back low
    }
    PORTD ^= (1 << 2);              //00000100 flips Rck high (latches storage register)
    PORTD ^= (1 << 2);              //00000100 flips Rck low
    mask = (15 << 2);               //00111100
    mask ^= (1 << activeDigit << 2);//flips the bit for the selected digit
    if (PM_time){
      mask |= (1 << 1);             //00000010 Turns on LED
    }
    PORTC = mask;                   //sets digit port
}