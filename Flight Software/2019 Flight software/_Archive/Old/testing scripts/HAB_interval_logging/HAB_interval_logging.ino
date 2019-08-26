//---------------------------------------------------------------------------------------------\
//                                           Libraries                                         |
//---------------------------------------------------------------------------------------------/


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


//---------------------------------------------------------------------------------------------\
//                                           Variables                                         |
//---------------------------------------------------------------------------------------------/


    //The interface of the BME sensor should be I2C
    Adafruit_BME280 bme; // I2C
    //Adafruit_BME280 bme(BME_SS); // hardware SPI
    //Adafruit_BME280 bme(BME_SS, BME_MOSI, BME_MISO, BME_SCK); // software SPI
    
    //Declare BME sensor reading object
    //BMEObject bmeSensorReading;
    
    //Declare TinyGPS object
    TinyGPSPlus gps;
    
    //Declare global GPS variable
    //GPSObject gpsReading;
    
    //Declare Serial Connection to GPS Module
    SoftwareSerial ss(RX_PIN, TX_PIN);
          
    //Delay of each loop
    int _delayTime;

    //Actuators
    #define ACT_ARR_SIZE 7
    ActProps _actArray[ACT_ARR_SIZE] {
        ActProps(ACT1_EN, ACT1_PUSH, ACT1_PULL, 0),
        ActProps(ACT2_EN, ACT2_PUSH, ACT2_PULL, 1),
        ActProps(ACT3_EN, ACT3_PUSH, ACT3_PULL, 2),
        ActProps(ACT4_EN, ACT4_PUSH, ACT4_PULL, 3),
        ActProps(ACT5_EN, ACT5_PUSH, ACT5_PULL, 4),
        ActProps(ACT6_EN, ACT6_PUSH, ACT6_PULL, 5),
        ActProps(ACT7_EN, ACT7_PUSH, ACT7_PULL, 6)
    };

    //TEMP
    float altitudeCounter = 0;
    bool ascending = true;
    int timeCounter = 0;
    float ascentSpeed = 100;
    float openTime = 0;
    

//---------------------------------------------------------------------------------------------\
//                                            Setup                                            |
//---------------------------------------------------------------------------------------------/


    void setup() {       
        // Begin serial communication to BME
        Serial.begin(9600);
        Serial.println(F("BME280 test"));
    
        // Begin Serial Communication to GPS Module
        ss.begin(GPS_BAUD);
    
        // Start Talking to BME Sensor
        bool status = bme.begin(); 

        //If the sensor could not be found, halt
        if (!status) {    
            Serial.println("Could not find a valid BME280 sensor, check wiring!");
            while (1);   
        }
    
        //Check presence of SD Card
        if (!SD.begin(4)) {
            Serial.println("Card failed, or not present");
            //while (1);
        }
        else {
            Serial.println("Found SD card!");
        }

        //Initialize the Excel file columns
        initExcelFile();
        
        Serial.println("-- Default Test --");
        _delayTime = 1000;
        
        Serial.println();  
    }


//---------------------------------------------------------------------------------------------\
//                                            Loop                                             |
//---------------------------------------------------------------------------------------------/


    /*
     * Get actuator temperature, heat if before interval if needed (altitude + speed)
     * If actuator scheduled to move, take pics as it moves (few snapshots) (opening/closing)
     * Get tempertaure before it opens/closes (need to heatup potentially when closing too)
     * About 20 seconds to warm up? (alt_heat_start = interval_altitude - speed*20)
     * 
     * Currently, just GPS and LEDs for actuators
     */

    //Make a separate GPS tester

    void loop() {
        //Get the BME readings
        BMEObject bmeReadings = readBMESensor();

        //Get the GPS readings
        GPSObject gpsReadings = readGPS();

        //Write the readings to the Excelfile
            gpsReadings.time.second = timeCounter; //Remove these later once we actually have the GPS
            gpsReadings.altitude = altitudeCounter;  
            gpsReadings.speed = ascentSpeed;      
        writeToSD(bmeReadings, gpsReadings);


        //Only upon ascent will we open the pods
        //Should probably 'else' to always make sure they're closed
        if(ascending){
            openPods(gpsReadings, &openTime);
        }

        //Delay
        delay(_delayTime);






        Serial.print("[2J");
        Serial.print(gpsReadings.time.hour);      Serial.print(":"); 
        Serial.print(gpsReadings.time.minute);    Serial.print(":"); 
        Serial.println(gpsReadings.time.second);
        Serial.println(gpsReadings.altitude);
        Serial.println(gpsReadings.speed);
        Serial.println(gpsReadings.longitude);
        Serial.println(gpsReadings.latitude); 
        Serial.println(bmeReadings.temperature);
        Serial.println(bmeReadings.pressure);
        Serial.println(bmeReadings.humidity);
        for(int i = 0; i != ACT_ARR_SIZE; i++){
            Serial.print(i);
            Serial.print(" : ");
            Serial.println(_actArray[i].status);
        }      
        Serial.println();
        Serial.println();

        altitudeCounter += ascentSpeed;
        timeCounter++;
    }


//---------------------------------------------------------------------------------------------\
//                                          Functions                                          |
//---------------------------------------------------------------------------------------------/


    /*-------------------------------------------------------------------------------------------------------------------------*\ 
    |    This function reads the BME Sensor and stores the value in the BME struct declared globally.                           |
    |                                                                                                                           |
    |   Accepts: None                                                                                                          |
    |   Returns: None                                                                                                           |
    \*-------------------------------------------------------------------------------------------------------------------------*/
        BMEObject readBMESensor() {
            BMEObject readings;
                readings.temperature = bme.readTemperature();
                readings.pressure = bme.readPressure() / 100.0F;
                readings.humidity = bme.readHumidity();

            return readings;
        }


    /*-------------------------------------------------------------------------------------------------------------------------*\    
    |    Initialized the Excel file on the SD card.                                                                             |
    |                                                                                                                           |
    |    Accepts: None                                                                                                          |
    |    Returns: None                                                                                                          |
    \*-------------------------------------------------------------------------------------------------------------------------*/
        void initExcelFile() {
            File dataFile = SD.open("datalog.txt", FILE_WRITE);

            //If the file exists,
            if(dataFile) {
                //Write the column headers
                dataFile.print("Time(s),Altitude(m),Speed(m/s),Longitude(deg),Latitude(deg),Temperature(C),Pressure(hPa),Humidity(%)");
                for(int i = 0; i != ACT_ARR_SIZE; i++){
                   dataFile.print(",");
                   dataFile.print(i);
                   dataFile.print("_status");
                }

                dataFile.println();

                //Close the file
                dataFile.close();
            }
            else{      
                Serial.println("error opening datalog.txt");       
            }
        }

    /*-------------------------------------------------------------------------------------------------------------------------*\    
    |    This function writes to the SD Card on the ethernet shield. The Slave Select pin is declared as pin 53 on the MEGA.    |
    |                                                                                                                           |
    |    Accepts: None                                                                                                          |
    |    Returns: None                                                                                                          |
    \*-------------------------------------------------------------------------------------------------------------------------*/
        void writeToSD(BMEObject bmeReadings, GPSObject gpsReadings) {
        
            File dataFile = SD.open("datalog.txt", FILE_WRITE);
        
            if(dataFile) {               
                //Time (H:M:S), Altitude, Speed, Longitude, Latitude, Temperature, Pressure, Humidity
                dataFile.print(gpsReadings.time.hour);      dataFile.print(":"); 
                dataFile.print(gpsReadings.time.minute);    dataFile.print(":"); 
                dataFile.print(gpsReadings.time.second);    dataFile.print(",");
                dataFile.print(gpsReadings.altitude);       dataFile.print(",");
                dataFile.print(gpsReadings.speed);          dataFile.print(",");
                dataFile.print(gpsReadings.longitude);      dataFile.print(",");
                dataFile.print(gpsReadings.latitude);       dataFile.print(",");
                dataFile.print(bmeReadings.temperature);    dataFile.print(",");
                dataFile.print(bmeReadings.pressure);       dataFile.print(",");
                dataFile.print(bmeReadings.humidity);

                //Actuator statuses
                for(int i = 0; i != ACT_ARR_SIZE; i++){
                   dataFile.print(",");
                   dataFile.print(_actArray[i].status);
                }

                //Print New Line
                dataFile.println();

                //close the file
                dataFile.close();       
            }
            else{        
                Serial.println("error opening datalog.txt");       
            }
        }


    /*-------------------------------------------------------------------------------------------------------------------------*\    
    |   This function gets serial data from the GPS module and stores longitude, latitude, altitude,                            |
    |   speed, and time into the GPS global object.                                                                             |
    |                                                                                                                           |
    |    Accepts: None                                                                                                          |
    |    Returns: None                                                                                                          |
    \*-------------------------------------------------------------------------------------------------------------------------*/
        GPSObject readGPS() {
            GPSObject readings;
            
            //If there are any bytes available, read
            if (ss.available() > 0) {      
                if(gps.encode(ss.read())){
                    readings.longitude = 0; //gps.location.lng(); //gpsReading was gpsObject
                    readings.latitude = 0; //gps.location.lat();
                    readings.altitude = 0; //gps.altitude.meters();
                    readings.speed = 0; //gps.speed.mps();
                    readings.time.hour = 0; //gps.time.hour();
                    readings.time.minute = 0; //gps.time.minute();
                    readings.time.second = 0; //gps.time.second();   
                }  
            }

            return readings;
        }


    /*-------------------------------------------------------------------------------------------------------------------------*\    
    |   This function reads the current altitude and returns a value for which altitude interval the payload is in.             |
    |   This signals which chambers to open and close.                                                                          |
    |                                                                                                                           |
    |    Accepts: None                                                                                                          |
    |    Returns: Integer                                                                                                       |
    \*-------------------------------------------------------------------------------------------------------------------------*/
        int getAltInterval(GPSObject gpsReadings) {

            int interval = (int)(gpsReadings.altitude / ALTITUDE_INTERVAL);

            return ((interval < 0 || interval > 6) ? -1 : interval);
        
        	/*switch(gpsReadings.altitude) {
        
        		case 0 ... 1*ALTITUDE_INTERVAL-1:
        			return 0;
        
        		case 1*ALTITUDE_INTERVAL ... 2*ALTITUDE_INTERVAL-1:
        			return 1;
        
        		case 2*ALTITUDE_INTERVAL ... 3*ALTITUDE_INTERVAL-1:
        			return 2;
        
        		case 3*ALTITUDE_INTERVAL ... 4*ALTITUDE_INTERVAL-1:
        			return 3;
        
        		case 4*ALTITUDE_INTERVAL ... 5*ALTITUDE_INTERVAL-1:
        			return 4;
        
        		case 5*ALTITUDE_INTERVAL ... 6*ALTITUDE_INTERVAL-1:
        			return 5;
        
        		case 7*ALTITUDE_INTERVAL ... 8*ALTITUDE_INTERVAL-1: //Integer overflow error?
        			return 6;
        
        		default:
        			return -1;
        
        	}*/

         //return 0;
        }


    /*-------------------------------------------------------------------------------------------------------------------------*\    
    |   Opens and closes the appropriate holdings based on the altitude                                                         |
    |                                                                                                                           |
    |    Accepts: Float(altitude)                                                                                               |
    |    Returns: void                                                                                                          |
    \*-------------------------------------------------------------------------------------------------------------------------*/
        /*void openPods(float altitude){
            int interval = (int)(altitude / ALTITUDE_INTERVAL);
                interval = ((interval < 0 || interval > 6) ? -1 : interval);
    
                //Actuators
                for(int i = 0; i != ACT_ARR_SIZE; i++){
                    if(_actArray[i].alt_interval == interval && _actArray[i].status == false){
						//Enable actuator and set the status
                        _actArray[i].status = true;
						digitalWrite(_actArray[i].enable_pin, HIGH); //enable actuator
						
						//Set the push and pull pins
						digitalWrite(_actArray[i].push_pin, HIGH); //Set PUSH pin high
						digitalWrite(_actArray[i].pull_pin, LOW); //Set PULL pin low 					
                    }
                    else if(_actArray[i].alt_interval != interval && _actArray[i].status == true){
                        //Enable actuator and set the status
                        _actArray[i].status = false;
						digitalWrite(_actArray[i].enable_pin, HIGH); //enable actuator
						
						//Set the push and pull pins
						digitalWrite(_actArray[i].push_pin, LOW); //Set PUSH pin low
						digitalWrite(_actArray[i].pull_pin, HIGH); //Set PULL pin high 
                    }
                }
    
                Serial.print("Interval : ");
                Serial.println(interval);
        }*/



        //GPS TIME ROLLS OVER SO FIX THIS
        void openPods(GPSObject gpsReadings, float* openTime){
            int interval = (int)(gpsReadings.altitude / ALTITUDE_INTERVAL);
                interval = ((interval < 0 || interval > 6) ? -1 : interval);
    
            Serial.print("Interval : ");
            Serial.println(interval);

            float TIME_HEAT = 5;
            float TIME_ACTUATE = 20; //Treats it like 24s for some reason..?
            


            //Actuators
            for(int i = 0; i != ACT_ARR_SIZE; i++){

                //If it is the next interval
                if(_actArray[i].alt_interval == (interval + 1)){
                    if((_actArray[i].alt_interval*ALTITUDE_INTERVAL - gpsReadings.altitude)/gpsReadings.speed <= TIME_HEAT){ //Factor in temperature
                        Serial.print("Heating actuator : ");
                        Serial.println(i);
                    }
                    else{
                        //Serial.print("Not heating actuator : ");
                        //Serial.println(i);
                    }
                }
                else if (_actArray[i].alt_interval == interval){ //Else if current interval
                    if(gpsReadings.altitude < (((interval + 1)*ALTITUDE_INTERVAL) - gpsReadings.speed*(TIME_HEAT + TIME_ACTUATE))){ //If we're not at the altitude where we heat it up before closing, open it
                        //Write heat low to ensure it isnt on during the entire interval
                        if(_actArray[i].status == false){
                            _actArray[i].status = true;
                            digitalWrite(_actArray[i].enable_pin, HIGH); //enable actuator
                        
                            //Set the push and pull pins
                            digitalWrite(_actArray[i].push_pin, HIGH); //Set PUSH pin high
                            digitalWrite(_actArray[i].pull_pin, LOW); //Set PULL pin low        
                            
                            Serial.print("Opening pod : ");
                            Serial.println(i);
                            *openTime = gpsReadings.time.second; //FIX THIS LATER
                        } 
                    }
                    else if (*openTime != 0 && gpsReadings.time.second > (*openTime + TIME_ACTUATE)){ //If the actuation time has elapsed after opening the pod, disable the actuator
                        *openTime = 0;
                        Serial.print("GOT IN HERE SOME HOW");
                        digitalWrite(_actArray[i].enable_pin, LOW); //disable actuator
                    }
                    else if (gpsReadings.altitude < ((interval + 1)*ALTITUDE_INTERVAL - gpsReadings.speed*TIME_ACTUATE)){ //If we're in the heating period before closing, heat it up
                        Serial.print("Heating actuator : ");
                        Serial.println(i);
                    }
                    else{
                        //write heat low
                        if(_actArray[i].status == true){
                            _actArray[i].status = false;
                            digitalWrite(_actArray[i].enable_pin, HIGH); //enable actuator
                        
                            //Set the push and pull pins
                            digitalWrite(_actArray[i].push_pin, LOW); //Set PUSH pin low
                            digitalWrite(_actArray[i].pull_pin, HIGH); //Set PULL pin high
                            
                            Serial.print("Closing pod : ");
                            Serial.println(i);
                        }
                    }
                }
                else{
                    digitalWrite(_actArray[i].enable_pin, LOW); //disable actuator                
                }
            }

            Serial.print("openTime : ");
            Serial.println(*openTime);
            Serial.print("gpsTime : ");
            Serial.println(gpsReadings.time.second);
        }





