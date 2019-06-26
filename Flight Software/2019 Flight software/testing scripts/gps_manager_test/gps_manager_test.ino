static const int RXPin = 5, TXPin = 6; //4: Uno Tx, GPS Rx [[Tx to Rx and vice versa]]
static const uint32_t GPSBaud = 9600;

#include <Wire.h>
#include <SPI.h>
#include <HAB_GPS_Manager.h>


//SoftwareSerial ss(TXPin, RXPin);

bool receivedLock = false;

void setup(){
  Serial.begin(9600);
  //ss.begin(9600);





  
}

void loop(){
  
}
