static const int RXPin = 5, TXPin = 6; //4: Uno Tx, GPS Rx [[Tx to Rx and vice versa]]

#include <Wire.h>
#include <SPI.h>
#include <HAB_GPS.h>
#include <HAB_Logging.h>

HAB_GPS gps1 = HAB_GPS(RXPin, TXPin);

void setup(){
  Serial.begin(9600);
  HAB_Logging::setChip(4);

  //Wait until GPS lock
  while(!gps1.getLockStatus());
  gps1.printInfo();
}

void loop(){
  Serial.print("Show : ");
  Serial.println(gps1.getReadings()->location.lng());
  Serial.println();
  
  gps1.feedReceiver();
}
