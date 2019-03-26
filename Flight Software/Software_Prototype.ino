
// Mission Specific Library. Struct definitions and Constants are in this library
#include <WesternUHAB.h>

// Ethernet Shield and SD Card Library
#include <Ethernet.h>
#include <SD.h>

// GPS library. TinyGPS++ needs install
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// SPI and Sensor Libraries
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//The interface of the BME sensor should be I2C
Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_SS); // hardware SPI
//Adafruit_BME280 bme(BME_SS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

// Declare BME sensor reading object
BMEObject bmeSensorReading;

// Declare TinyGPS object
TinyGPSPlus gps;

// Declare global GPS variable
GPSObject gpsReading;

// Declare Serial Connection to GPS Module
SoftwareSerial ss(RX_PIN, TX_PIN);

void setup() {
    
    // Begin serial communication to BME
    Serial.begin(9600);
    Serial.println(F("BME280 test"));

    // Begin Serial Communication to GPS Module
    ss.begin(GPS_BAUD);

    bool status;

    // Start Talking to BME Sensor
    status = bme.begin(); 
     
    if (!status) {

        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);

    }

    // Check presence of SD Card
    if (!SD.begin(SD_CHIPSELECT)) {
        Serial.println("Card failed, or not present");
        while (1);
    }
    
    Serial.println("-- Default Test --");
    delayTime = 1000;
    
    Serial.println();

}

void loop() {

    readBMESensor();
    readGPS();
    writeToSD();
    delay(delayTime);

}

/*

This function reads the BME Sensor and stores the value in the BME struct declared globally.

Accepts: None
Returns: None

*/
void readBMESensor() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();

    bmeSensorReading.temperature = bme.readTemperature();
    bmeSensorReading.pressure = bme.readPressure() / 100.0F;
    bmeSensorReading.humidity = bme.readHumidity();
}

/*

This function writes to the SD Card on the ethernet shield. The Slave Select pin is declared as pin 53 on the MEGA.

Accepts: None
Returns: None

*/
void writeToSD() {

    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    if(dataFile) {

        dataFile.println("Temperature = ");
        dataFile.println(bmeSensorReading.temperature);
        dataFile.println(" C");

        dataFile.println("Pressure = ");
        dataFile.println(bmeSensorReading.pressure);
        dataFile.println(" hPa");

        dataFile.println("Humidity = ")
        dataFile.println(bmeSensorReading.humidity);
        dataFile.println(" %");

        dataFile.println("Longitude = ");
        dataFile.println(gpsReading.longitude);
        dataFile.println(" Deg");

        dataFile.println("Latitude = ");
        dataFile.println(gpsReading.latitude);
        dataFile.println(" Deg");

        dataFile.println("Altitude = ")
        dataFile.println(gpsReading.altitude);
        dataFile.println(" m");

        dataFile.println("Speed = ")
        dataFile.println(gpsReading.speed);
        dataFile.println(" m/s");

        dataFile.println("Time = ")
        dataFile.println(gpsReading.time.hour);
        dataFile.println(":");
        dataFile.println(gpsReading.time.minute);
        dataFile.println(":");
        dataFile.println(gpsReading.time.second);
        dataFile.println(":");

        // Print New Line
        dataFile.println();

        dataFile.close();

    } else {

        Serial.println("error opening datalog.txt");

    }

}

/*

This function gets serial data from the GPS module and stores longitude, latitude, altitude, speed, and time into the GPS global object.

Accepts: None
Returns: None

*/
void readGPS() {

    if (ss.available() > 0) {

        if(gps.encode(ss.read)){

                gpsObject.longitude = gps.location.lng();
                gpsObject.latitude = gps.location.lat();
                gpsObject.altitude = gps.altitude.meters();
                gpsObject.speed = gps.speed.mps();
                gpsObject.time.hour = gps.time.hour();
                gpsObject.time.minute = gps.time.minute();
                gpsObject.time.second = gps.time.second();

        }  

    }

}


/*

This function reads the current altitude and returns a value for which altitude interval the payload is in. 
This signals which chambers to open and close.

Accepts: None
Returns: int

*/
int altitudeIntervalCheck() {

	switch(gpsObject.altitude) {

		case 0 ... 1*ALTITUDE_INTERVAL:
			return 0;

		case 1*ALTITUDE_INTERVAL ... 2*ALTITUDE_INTERVAL:
			return 1;

		case 2*ALTITUDE_INTERVAL ... 3*ALTITUDE_INTERVAL:
			return 2;

		case 3*ALTITUDE_INTERVAL ... 4*ALTITUDE_INTERVAL:
			return 3;

		case 4*ALTITUDE_INTERVAL ... 5*ALTITUDE_INTERVAL:
			return 4;

		case 5*ALTITUDE_INTERVAL ... 6*ALTITUDE_INTERVAL:
			return 5;

		case 7*ALTITUDE_INTERVAL ... 8*ALTITUDE_INTERVAL:
			return 6;

		default:
			return -1;

	}

}
