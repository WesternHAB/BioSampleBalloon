static const int RXPin = 5, TXPin = 6; //4: Uno Tx, GPS Rx [[Tx to Rx and vice versa]]
static const uint32_t GPSBaud = 9600;

#include <Wire.h>
#include <SPI.h>
#include <SoftwareSerial.h>  // make sure this is NewSoftSerial beta 11 - now called SoftwareSerial
#include <TinyGPS++.h>       // you can get TinyGPS from the same place as NewSoftSerial - Mikal Hart's site.

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS module
SoftwareSerial ss(RXPin, TXPin);
//SoftwareSerial ss(TXPin, RXPin);

bool receivedLock = false;

void setup(){
  Serial.begin(9600);
  ss.begin(9600);
}

void loop(){
  while (ss.available() > 0){
    // get the byte data from the GPS
    byte gpsData = ss.read();
    Serial.write(gpsData);

    //gps.encode(ss.read());
    gps.encode(gpsData);
    //Serial.print("numSats= "); 
    //Serial.println(gps.satellites.value());

    /*if(receivedLock){
        digitalWrite(7, HIGH);
    }
    else{
        digitalWrite(7, LOW);
    }*/

    if (gps.location.isUpdated() || gps.satellites.value()){
    //    receivedLock = true;
        Serial.print("Lat/lng : ");
        Serial.println(gps.location.lat());
        Serial.println(gps.location.lng());
        Serial.println();
    }

    //delay(1000);
  }
}
