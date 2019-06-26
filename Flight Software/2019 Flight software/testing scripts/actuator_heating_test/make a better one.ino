// thermistor-1.ino Simple test program for a thermistor for Adafruit Learning System
// https://learn.adafruit.com/thermistor/using-a-thermistor by Limor Fried, Adafruit Industries
// MIT License - please keep attribution and consider buying parts from Adafruit

#include <Wire.h>
#include <SPI.h>
 
// the value of the 'other' resistor
#define SERIESRESISTOR 10000    
 
// What pin to connect the sensor to
#define THERMISTORPIN A0 

#define THERMISTORNOMINAL 10000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer

// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
 
 
 
 
#define HEAT_EN 37 //Enable A
#define HEAT_ON 24 //Heat A

#define ACT_EN 36 //Enable B
#define ACT_PUSH 22 //Push B
#define ACT_PULL 23 //Pull B
#define ACT_POS A15 //Positive rail position

bool extending;
bool heating;
float reading;
 
 
void setup(void) {
    //Pin setup
    pinMode(ACT_POS, INPUT); //Initalise potentiometer

    //Variable setup
    extending = true;
    heating = false;

    //Serial setup
    Serial.begin(9600);
}
 
 
 
 
void loop(void) {

    //-------------------------------------------------------------------------\
    //Actuator------------------------------------------------------------------|
        digitalWrite(ACT_EN, HIGH);

        //Get reading
        reading = analogRead(ACT_POS);
    
        //Actuator direction
        if(extending){
            digitalWrite(ACT_PUSH, LOW);
            digitalWrite(ACT_PULL, HIGH);
            if(reading > 1000){
                extending = false;
            }
            Serial.print("(extending) ");
        }
        else{
            digitalWrite(ACT_PUSH, HIGH);
            digitalWrite(ACT_PULL, LOW);
            if(reading < 10){
                extending = true;
            }
            Serial.print("(retracting) ");
        }

        //Output reading
        Serial.print("Position : "); 
        Serial.println(reading);   
        
        //Min: 0
        //Max: 1023 (Dipped to 1021)


    //-------------------------------------------------------------------------\
    //Heating------------------------------------------------------------------|
        //Get reading
        reading = analogRead(THERMISTORPIN);
        reading = (1023 / reading)  - 1;     // (1023/ADC - 1) 
        reading = SERIESRESISTOR / reading;  // 10K / (1023/ADC - 1)

        //Get temperature
        float temp;
        temp = reading / THERMISTORNOMINAL;     // (R/Ro)
        temp = log(temp);                  // ln(R/Ro)
        temp /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
        temp += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
        temp = 1.0 / temp;                 // Invert
        temp -= 273.15;                         // convert to C

        //Turn heating on if less than -10*C
        if(temp < -10){
            heating = true;
        }

        //Keep heater on until temperature reaches 0*C
        if(heating && temp < 0){
            digitalWrite(HEAT_EN, HIGH); //Heater
            digitalWrite(HEAT_ON, HIGH);
            Serial.print("(on) ");
        }
        else {
            digitalWrite(HEAT_EN, LOW); //Heater
            digitalWrite(HEAT_ON, LOW);    
            Serial.print("(off) ");
            heating = false; 
        }

        //Output reading
        Serial.print("Temperature : "); 
        Serial.print(temp);
        Serial.println(" *C");

    Serial.println("");
    delay(1000);
}
