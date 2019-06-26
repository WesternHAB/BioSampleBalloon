//Most of the work done is in the HAB libraries listed in the imports section below.

/*
 * What is and isn't implemented in this test software:
 * 
 *  -Pod opening and closing
 *  -Heating before/during moving (does not currently check temperature of actuator, heats the entire time)
 *  -GPS position/altitude/speed
 *  -Partial SD-card logging
 *  -No camera imaging
 *  -No BME280 readings
 *  -No logging of readings to Excel file
 *  -No Telnet override
 *  
 *  
 * The missing components (e.g. Camera, actuator temperature reading, Excel logging, etc.) have working code implementation/libraries
 * created and already working, and just requires integration into the next flight software test file. The only one lacking code/testing is Telnet.
 */


//---------------------------------------------------------------------------------------------\
//                                           Imports                                           |
//---------------------------------------------------------------------------------------------/


    //Mission Specific Library. Struct definitions and Constants are in this library
    #include <HAB_Definitions.h>

    //Include Actuator, GPS (will have the prediction stuff, trend too)
    #include <HAB_Actuator.h>
    #include <HAB_Camera.h>
    #include <HAB_GPS.h>
    #ifndef HAB_Logging_h
        #include <HAB_Logging.h>
    #endif
        
    // Ethernet Shield Library
    #include <Ethernet.h> //For Telnet
    
    // SPI and Sensor Libraries
    #include <Wire.h>
    #include <SPI.h>
    #include <Adafruit_Sensor.h>
    #include <Adafruit_BME280.h>


//---------------------------------------------------------------------------------------------\
//                                          Definitions                                        |
//---------------------------------------------------------------------------------------------/


    #define MOVE_TIME 30
    #define HEAT_TIME 30


//---------------------------------------------------------------------------------------------\
//                                           Variables                                         |
//---------------------------------------------------------------------------------------------/


    //The interface of the BME sensor should be I2C
    Adafruit_BME280 _bme; // I2C
    
    //Declare GPS object
    HAB_GPS _gps; //Can just straight up query this but I set the results to the pointer below, which will be updated in the background
    TinyGPSPlus* _readings;
    
    //Holds a reference to the logging stringPtr
    char* logStringPtr;

    //Actuators
    #define ACT_ARR_SIZE 7
    HAB_Actuator _actArray[] = { //ACT_ARR_SIZE
        HAB_Actuator(ACT1_EN, ACT1_PUSH, ACT1_PULL, ACT1_POS, HEAT1_EN, THERMISTOR1, 10000, 20000),
        HAB_Actuator(ACT2_EN, ACT2_PUSH, ACT2_PULL, ACT2_POS, HEAT2_EN, THERMISTOR2, 20000, 30000),
        HAB_Actuator(ACT3_EN, ACT3_PUSH, ACT3_PULL, ACT3_POS, HEAT3_EN, THERMISTOR1, 30000, 40000),
        HAB_Actuator(ACT4_EN, ACT4_PUSH, ACT4_PULL, ACT4_POS, HEAT4_EN, THERMISTOR1, 40000, 999999999), //Perhaps give a max time as well?
    };

    #define CAM_ARR_SIZE 1
    HAB_Camera _camArray[] = {
        //HAB_Camera(4, A14, 47) //Actual setup
        //HAB_Camera(4, 7, 47)
    };
    

//---------------------------------------------------------------------------------------------\
//                                            Setup                                            |
//---------------------------------------------------------------------------------------------/


	//camera: 10s before move, 10s after move
	//No external as of yet

    void setup() {
        //Serial setup
        Serial.begin(9600);

        //Set up logging
        HAB_Logging::setChip(4);
        logStringPtr = HAB_Logging::getStringPtr();

        //Start message
        printHeader(); //Print the header
   
        //General setup...    
            //Thermistor pins
            //GPS pins (don't think this is req'd)?
            //Heater pings
            //Actuator pins
            //Actuator position pins
    
            //for(int i = 0; i != ACT_ARR_SIZE; i++){
                //_actArray[i] to set up the pins
            //}
    
            _gps = HAB_GPS(5, 6); //Rx, Tx


        //Hold the program until a lock is obtained
        while(!_gps.getLockStatus());

        //Lock has been obtained, print rest of startup info
        HAB_Logging::printLog("\r\n                     *!GPS lock obtained!*\r\n", "");
        printHeader();
        printInfo(_gps);
    }


//---------------------------------------------------------------------------------------------\
//                                            Loop                                             |
//---------------------------------------------------------------------------------------------/


    void loop() {
        //This will open/close pods based on the altitudes specified in the actuator constructor calls
        //for(int i = 0; i != ACT_ARR_SIZE; i++){
            //handleActuator(_actArray[i]);    
        //}

        /*for(int i = 0; i != cameraSize; i++){
            handleCamera(.....[i]);
            if(camera.eventNotHandledYet && cur_alt > event_alt){
                //Take picture
                //Some SD card logging
            }
        }*/

        _gps.feedReceiver(); //Replace with gps_manager feed here
    }


//---------------------------------------------------------------------------------------------\
//                                          Functions                                          |
//---------------------------------------------------------------------------------------------/


    /*-------------------------------------------------------------------------------------*\
    |   Name:       handleActuator                                                          |
    |   Purpose:    Used to open and close the pods, and maintain proper temperature        |
    |               of the actuators when they are required to move.                        |
    |   Arguments:  Actuator, GPSreadings                                                   |
    |   Returns:    Void                                                                    |
    \*-------------------------------------------------------------------------------------*/
        void handleActuator(HAB_Actuator _act){

            //Wrap most of this in a Telnet override and keep heater stuff automatic
            
            //Or just add 'OR' conditions to each, e.g. telnet_open == true?
            //This will set move_enabled to true

            //E.g. telNet command 'open pod 3'
                //actArray[2] boolean for telnet_override is set to true
                //telnet_open is set to true
            //E.g. telNet command 'close pod 3'
                //actArray[2] boolean for telnet_override is set to true
                //telnet_open is set to false
            //E.g. telnet command 'release pod 3'
                //actArray[2] boolean for telnet_override is set to false

            
    
            //----------------------------------------------------------\
            //Actuator movement-----------------------------------------|
    
                //Handle opening-----------------------------------------------//
                    //If in interval and not open, retract actuator to open
                    if(inInterval(_act.getOpenAlt(), _act.getCloseAlt() - _readings->speed.mps()*(MOVE_TIME)) && _act.isFullyOpen() == false){
                        _act.setMoveEnabled(true);
                        _act.retract(); //Can also set move_enabled in here
                    }
                    //If in interval and open, disable
                    if(inInterval(_act.getOpenAlt(), _act.getCloseAlt() - _readings->speed.mps()*(MOVE_TIME)) && _act.isFullyOpen() == true){
                        _act.setMoveEnabled(false);
                        _act.halt();
                    }
        
                //Handle closing-----------------------------------------------//
                    //If outside interval and not closed, extend actuator to close
                    if(!inInterval(_act.getOpenAlt(), _act.getCloseAlt() - _readings->speed.mps()*(MOVE_TIME)) && _act.isClosed() == false){ // && _act.enabled = false : This was removed because of the case where pod only partially opened (e.g. failed heater) and act.move_enabled was never disabled
                        _act.setMoveEnabled(true);
                        _act.extend();
                    }
                    //If outside interval and closed, disable
                    else if(!inInterval(_act.getOpenAlt(), _act.getCloseAlt() - _readings->speed.mps()*(MOVE_TIME)) && _act.isClosed() == true){ // && _act.enabled = true : removed for similar reasons
                        _act.setMoveEnabled(false);
                        _act.halt();
                    }
                      
            //----------------------------------------------------------\
            //Heating---------------------------------------------------|
    
                //Handle pre-heat period---------------------------------------//
                    //If the period before opening, heat it up
                    if(inInterval(_act.getOpenAlt() - _readings->speed.mps()*(HEAT_TIME), _act.getOpenAlt())){
                        _act.setPreheatEnabled(true);
                    }       
                    //Else if the period before closing, heat it up
                    else if(inInterval(_act.getCloseAlt() - _readings->speed.mps()*(HEAT_TIME + MOVE_TIME), _act.getCloseAlt())){
                        _act.setPreheatEnabled(true);
                    }
                    //If not in either of these intervals, disable preheat
                    else{
                        _act.setPreheatEnabled(false);
                    }
    
                //Handle heating-----------------------------------------------//
                    //If in preheat period or the actuator is currently enabled, heat to the appropriate temperature
                    if(_act.getPreheatEnabled() || _act.getMoveEnabled()){ //Take into account position trend of actuator, adjust heat if necessary (only if act.enabled, preheat will make it look like its 'stuck')                     
                        //Here, check the temperature of the actuator and make the decision
                        _act.enableHeating();
                    }
                    else{
                        _act.disableHeating();
                    }
        }

    /*-------------------------------------------------------------------------------------*\
    |   Name:       inInterval                                                              |
    |   Purpose:    Returns true if the balloon is currently in the given                   |
    |               altitude interval.                                                      |
    |   Arguments:  (Double) open altitude, (Double) close altitude, GPSreadings            |
    |   Returns:    Boolean                                                                 |
    \*-------------------------------------------------------------------------------------*/
        bool inInterval(double open_alt, double close_alt){
            return (_readings->altitude.meters() >= open_alt && _readings->altitude.meters() < close_alt);
        }

    /*-------------------------------------------------------------------------------------*\
    |   Name:       printHeader                                                             |
    |   Purpose:    Prints a nice header.                                                   |
    |   Arguments:  void                                                                    |
    |   Returns:    void                                                                    |
    \*-------------------------------------------------------------------------------------*/
        void printHeader(){
            HAB_Logging::printLogln(" ############################################################", "");
            HAB_Logging::printLogln("##                                                          ##", "");
            HAB_Logging::printLogln("#               Western University HAB Project               #", "");
            HAB_Logging::printLogln("#                                                            #", "");
            HAB_Logging::printLogln("#                    Flight Software Logs                    #", "");
            HAB_Logging::printLogln("##                                                          ##", "");
            HAB_Logging::printLogln(" ############################################################", "");
        }

    /*-------------------------------------------------------------------------------------*\
    |   Name:       printInfo                                                                |
    |   Purpose:    Prints some useful info.                                                |
    |   Arguments:  void                                                                    |
    |   Returns:    void                                                                    |
    \*-------------------------------------------------------------------------------------*/
        void printInfo(HAB_GPS gps){
            //Print author and team info
            HAB_Logging::printLogln("\r\nAuthor:", ""); HAB_Logging::printLogln("---------------", ""); HAB_Logging::printLogln("Stephen Amey", "");
            //Add rest of info here
        
            //Print the date from the GPS
            HAB_Logging::printLogln("\r\n\r\nFlight date:", ""); HAB_Logging::printLogln("---------------", ""); HAB_Logging::printLogln(_gps.getDate(HAB_Logging::getStringPtr()), "");
        
            //if(_camArray[0] != NULL){
            //    //Print out the camera info
            //    HAB_Logging::printLogln("\r\n\r\nCamera info:", ""); HAB_Logging::printLogln("---------------", ""); HAB_Logging::printLogln(newCam.getInfo(), "");
            //}
        
            //Print out GPS info
            HAB_Logging::printLogln("\r\n\r\n", "");
            _gps.printInfo();
               
            //Print out final header
            HAB_Logging::printLogln("\r\n##############################################################", "");
            HAB_Logging::printLogln(    "#                       Logging begins                       #", "");
            HAB_Logging::printLogln(    "##############################################################\r\n\r\n", "");
        }
