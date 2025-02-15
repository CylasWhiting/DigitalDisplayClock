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

#include "SevSeg.h"
#include <DS3231.h>
#include <time.h>
//#include <MemoryFree.h>;
//#define MAX_MEMORY 32768//

SevSeg sevseg;  //Initiate a seven segment controller object
DS3231 rtc;     //Initiate an RTC controller object

bool PM_time;  //True if it's PM, False if it's AM
bool _12Hour;  //True configures the clock to 12 hour representation


void setup() {
  Wire.begin();

  const byte digitPins[] = { 2, 3, 4, 6 };
  const byte segmentPins[] = { 5, 1, 7, 9, 10, 0, 13, 14 };

  _12Hour = true;
  bool outputSQW = true;
  bool batteryUseAllowed = true;
  byte outputFrequency = 0;

  sevseg.begin(COMMON_ANODE, 4, digitPins, segmentPins, 0,1);
  sevseg.setBrightness(1);

  // sets clock to pulse at 1 hz
  rtc.enableOscillator(outputSQW, batteryUseAllowed, outputFrequency);


  pinMode(8, OUTPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  rtc.setClockMode(_12Hour);                                        //set clock to 12 hour mode
  attachInterrupt(digitalPinToInterrupt(A2), updateClock, RISING);  //Attatch interrupt to watch for the 1 hz clock pulse we set earlier

  /* while(!Serial);                      //for testing ram usage
  Serial.println(F("Free RAM: "));
  Serial.println(freeMemory());
  Serial.println(F("Used RAM: "));
  Serial.println(MAX_MEMORY - freeMemory());*/
}

void loop() {
  sevseg.refreshDisplay();                          //Keeps display on
  if (Serial.available()) {                         //If serial information is available, set the minutes and hours. Data must arrive in HHMM 24 hour format.
    uint16_t* serialInput = (uint16_t*)malloc(14);  //saves 2 bits
    *serialInput = Serial.parseInt();
    rtc.setHour((*serialInput - rtc.getMinute()) / 100);
    rtc.setMinute(*serialInput - ((*serialInput / 100) * 100));
    rtc.setSecond(0);
    free(serialInput);
    updateClock();  //frees memory
  }
  
}

void updateClock() {  //Function called by 1 hz clock interrupt
  if (PM_time) {      //Turns LED on if its PM
    digitalWrite(8, HIGH);
  } else {
    digitalWrite(8, LOW);
  }
  //Blanks the display, then sets new number , before turning the display
  sevseg.blank();
  sevseg.setNumber((rtc.getHour(_12Hour, PM_time) * 100) + rtc.getMinute(), 2);
  sevseg.refreshDisplay();
}
