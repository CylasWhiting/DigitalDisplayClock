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

#include <SPI.h>
#include <time.h>
//#include <MemoryFree.h>;
//#define MAX_MEMORY 32768//

bool PM_time;  //True if it's PM, False if it's AM
bool _12Hour;  //True configures the clock to 12 hour representation


void setup() {
  

  const byte digitPins[] = { 2, 3, 4, 6 };
  const byte segmentPins[] = { 5, 1, 7, 9, 10, 0, 13, 14 };

  _12Hour = true;
  bool outputSQW = true;
  bool batteryUseAllowed = true;
  byte outputFrequency = 0;

 SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE1));
  

  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(10, OUTPUT);
                                       //set clock to 12 hour mode
  //attachInterrupt(digitalPinToInterrupt(A2), updateClock, RISING);  //Attatch interrupt to watch for the 1 hz clock pulse we set earlier

  /* while(!Serial);                      //for testing ram usage
  Serial.println(F("Free RAM: "));
  Serial.println(freeMemory());
  Serial.println(F("Used RAM: "));
  Serial.println(MAX_MEMORY - freeMemory());*/
  digitalWrite(10, HIGH);
  digitalWrite(16, LOW);
  digitalWrite(17, LOW);
  digitalWrite(18, LOW);
  digitalWrite(19, LOW);
  digitalWrite(20, LOW);
  digitalWrite(21, LOW);
}

void loop() {
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE1));
  digitalWrite(10, LOW);
  SPI.transfer(0b00111111);
  digitalWrite(16, HIGH);
  digitalWrite(16, LOW);
  //digitalWrite(17, HIGH);
  SPI.endTransaction();
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