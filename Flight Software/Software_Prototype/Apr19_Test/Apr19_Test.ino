// Ethernet Shield and SD Card Library
#include <Ethernet.h>
#include <SD.h>
#include <WesternUHAB.h>
// GPS library. TinyGPS++ needs install
#include <SoftwareSerial.h>

// SPI and Sensor Libraries
#include <Wire.h>
#include <SPI.h>

#include <Adafruit_BME280.h>

//The interface of the BME sensor should be I2C
Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_SS); // hardware SPI
//Adafruit_BME280 bme(BME_SS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

// Declare BME sensor reading object
BMEObject bmeSensorReading;
bool isActuator1Activated = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(SD_CHIPSELECT, OUTPUT);
  
  // Begin serial communication to BME
  Serial.begin(9600);
  Serial.println(F("BME280 and Actuator test"));
  Serial.println(F("temp,press,hum,act1"));
  bool status;

  // Start Talking to BME Sensor
  status = bme.begin();

  if (!status) {

    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);

  }

//  // Check presence of SD Card
//  if (!SD.begin(SD_CHIPSELECT)) {
//      Serial.println("Card failed, or not present");
//      while (1);
//  }



}

void loop() {
  
  readBMESensor();
  extendActuator();
//  writeToSD();
  writeToMonitor();
//  dumpFromSD();
  delay(10000);

  readBMESensor();
  retractActuator();
//  writeToSD();
  writeToMonitor();
//  dumpFromSD();
  delay(10000);

}

void writeToMonitor(){
  Serial.print(bmeSensorReading.temperature);
  Serial.print(',');
  Serial.print(bmeSensorReading.pressure);
  Serial.print(',');
  Serial.print(bmeSensorReading.humidity);
  Serial.print(',');
  Serial.print(isActuator1Activated);
  Serial.println();
}

void dumpFromSD(){

  char stringBuffer[50];
   
  File dataFile = SD.open("datalog.csv", FILE_READ);
  dataFile.read(stringBuffer, 50);
  Serial.println(stringBuffer);
  
}

void writeToSD() {

    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    if(dataFile) {
        dataFile.print(bmeSensorReading.temperature);
        dataFile.print(',');
        dataFile.print(bmeSensorReading.pressure);
        dataFile.print(',');
        dataFile.print(bmeSensorReading.humidity);
        dataFile.print(',');
        dataFile.print(isActuator1Activated);
        dataFile.println();

        dataFile.close();

    } else {

        Serial.println("error opening datalog.txt");

    }
}

void readBMESensor() {
//  Serial.print("Temperature = ");
//  Serial.print(bme.readTemperature());
//  Serial.println(" *C");
//
//  Serial.print("Pressure = ");
//
//  Serial.print(bme.readPressure() / 100.0F);
//  Serial.println(" hPa");
//
//  Serial.print("Humidity = ");
//  Serial.print(bme.readHumidity());
//  Serial.println(" %");
//
//  Serial.println();

  bmeSensorReading.temperature = bme.readTemperature();
  bmeSensorReading.pressure = bme.readPressure() / 100.0F;
  bmeSensorReading.humidity = bme.readHumidity();

//  Serial.print(bmeSensorReading.temperature
}

void extendActuator() {
  digitalWrite(24, HIGH); //enable actuator
  isActuator1Activated = true;
  digitalWrite(22, HIGH);
  digitalWrite(23, LOW);     
}

void retractActuator() {
  digitalWrite(24, HIGH); //enable actuator
  isActuator1Activated = false;
  digitalWrite(22, LOW);
  digitalWrite(23, HIGH);
}
