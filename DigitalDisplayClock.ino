/*
Written by Cylas Whiting
February 10th, 2025

Arduino MKR WiFi 1010 Stats
Total size: 26420 bytes
Total memory usage: 4973 bytes

Arduino Uno Stats
Total size: 7260 bytes
Total memory usage: 545 bytes
*/

#include <time.h>
//#include <MemoryFree.h>;
//#define MAX_MEMORY 32768//
unsigned long int seconds = 86380; //43200 base value
bool PM_time = false;  //True if it's PM, False if it's AM

volatile long count = 0;
ISR(TIMER2_OVF_vect) {                  //Need more education on overflow vectors
  TCNT2 = 6; //start count of timer
  if (count >= 1000){ // 1 second 
    if (seconds == 86399) {
      seconds = 43200;
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
  
  byte hours = seconds / 3600;
  if (hours > 12) {
    hours -= 12;
  }
  byte minutes = (seconds % 3600) / 60;
  byte digitValues[4];
  digitValues[0] = hours / 10;
  digitValues[1] = hours % 10;
  digitValues[2] = minutes / 10;
  digitValues[3] = minutes % 10;
  for (byte i = 0; i < 4; i++) {
  flashDisplay(digitValues[i], i);
  delay(1);
  }
  
}

void flashDisplay(byte segValue, byte activeDigit) {
  PORTC = (15 << 2);
  if (PM_time){
      PORTC |= (1 << 1);             //Turns on LED
    }
  
  byte mask = 0;
    for (int n = 0; n < 8; n++){
      byte mask = digitCodeMap[segValue];  //sets mask to segment byte code
      if (activeDigit == 1) {
      mask |= 1;
    }
      if (mask & (1<<n)){           //Checks against the segment array to see if the bit (segment) is high 
        PORTD |= 1;                 //if true, send 1 to the SER PIN               
      } else {
        PORTD &= ~(1);              //if false, send 0 to the SER PIN
      }
      PORTD ^= (1 << 3);            //flips SRclk to high
      PORTD ^= (1 << 3);            //flips SRclk back low
    }
    PORTD ^= (1 << 2);              //flips Rck high (latches storage register)
    PORTD ^= (1 << 2);              //flips Rck low
    mask = (15 << 2);               //sets mask to 00111100
    mask ^= (1 << activeDigit << 2);//flips the bit for the selected digit
    if (PM_time){
      mask |= (1 << 1);             //Turns on LED
    }
    PORTC = mask;
}
/*void updateClock() {  //Function called by 1 hz clock interrupt
  if (PM_time) {      //Turns LED on if its PM
    digitalWrite(8, HIGH);
  } else {
    digitalWrite(8, LOW);
  }
  //Blanks the display, then sets new number , before turning the display
}
*/