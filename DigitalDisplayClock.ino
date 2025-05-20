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
byte currentNum = 0;
volatile long count = 0;
 /*TIMER CODE TAKEN FROM JAI KRISHNA DUE TO LACK OF UNDERSTANDING WILL REWRITE LATER
    SOURCE: https://textzip.github.io/posts/AVR-Diaries-03-Timers/*/
ISR(TIMER2_OVF_vect) {
  TCNT2 = 6;
  if (count >= 1000){ // 1 second 
      if (currentNum < 9) {
        currentNum++;
      } else {
        currentNum = 0;
      }
    
    count = 0;
  } else {
    count++;
  }
}

bool PM_time;  //True if it's PM, False if it's AM
 //True configures the clock to 12 hour representation
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
  /*TIMER CODE TAKEN FROM JAI KRISHNA DUE TO LACK OF UNDERSTANDING WILL REWRITE LATER
    SOURCE: https://textzip.github.io/posts/AVR-Diaries-03-Timers/*/
  cli(); //Disable Global Interrupts
  TCCR2A = 0; //Normal Mode
  TCCR2B = 0;
  TIMSK2 |= (1 << TOIE2); //Enable overflow interrupt
  sei(); //Enable Global Interrupts
  TCNT2 = 6; //Load value
  TCCR2B |= (1 << CS22); // Prescaler set to 64
}

void loop() {
  for (byte i = 0; i < 4; i++){
    flashDisplay(currentNum, i);
  }
}
void flashDisplay(byte segValue, byte activeDigit) {
  byte mask = 0;
    for (int n = 0; n < 8; n++){
      byte mask = digitCodeMap[segValue];  //sets mask to segment byte code
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