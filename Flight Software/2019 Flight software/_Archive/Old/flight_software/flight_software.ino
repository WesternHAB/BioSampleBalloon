/*
	OCCAISIONAL SOFTWARE BACKUP
	
	Some things left to do:
		-Integration with CSA's PRISM system.
		-Testing with a dedicated groundstation server program.
		-Long-duration tests to ensure no memory issues arise.
		-Simulated GPS readings test with all components attached.
		-General cleanup of main program/libraries for better readability.
	etc.
*/
 

//---------------------------------------------------------------------------------------------\
//                                           Imports                                           |
//---------------------------------------------------------------------------------------------/


    #ifndef HAB_Actuator_h
        #include <HAB_Actuator.h>
    #endif
    #include <HAB_Camera.h>
    #include <HAB_GPS.h>
    #ifndef HAB_Logging_h
        #include <HAB_Logging.h>
    #endif
        
    //Ethernet Shield Library
    #include <Ethernet.h>
    #include <EthernetUdp.h>
    
    //SPI and Sensor Libraries (BME uses I2C)
    #include <Wire.h>
    #include <SPI.h>
    #include <Adafruit_Sensor.h>
    #include <Adafruit_BME280.h>

    //Mission Specific Library. Struct definitions and Constants are in this library
    #ifndef HAB_Structs_h
        #include <HAB_Structs.h>
    #endif

    //This one MUST be included LAST in order to override any redeclared definitions
    #include <HAB_Definitions.h>


//---------------------------------------------------------------------------------------------\
//                                           Variables                                         |
//---------------------------------------------------------------------------------------------/


	//Holds a reference to the logging stringPtr
    char* genStringPtr;
	
	//Is the HAB descending?
    bool isDescending = false; //Currently not properly implemented

	//----------------------------------------------------------\
	//Sensors and camera----------------------------------------|
		//The interface of the BME sensor should be I2C
		Adafruit_BME280 _bme;
		BMEReadings _BMEreadings;
		
		//Declare GPS object
		HAB_GPS _gps; //Can just straight up query this but I set the results to the pointer below, which will be updated in the background
		GPSReadings _GPSreadings;

		//Camera object, image name
		HAB_Camera _cam;
		int imgCount = 0;
		char imgNamePtr[14];
		
		//Heating temperatures
		float minTemp = MIN_ACTUATOR_TEMP;
		float maxTemp = MAX_ACTUATOR_TEMP;
		
		//Readings timing
		unsigned long lastReadingsTime = 0;  
    
	//----------------------------------------------------------\
	//Actuators-------------------------------------------------|
		int act_arr_len;
		int activeIndex = 0;
		HAB_Actuator _actArray[] = { //Order matters! We will give each atleast a 1.5Km buffer
			HAB_Actuator("POD_1", ACT1_EN, ACT1_PUSH, ACT1_PULL, ACT1_POS, HEAT1_EN, THERMISTOR1, 10000, 20000),
			HAB_Actuator("POD_2", ACT2_EN, ACT2_PUSH, ACT2_PULL, ACT2_POS, HEAT2_EN, THERMISTOR2, 20000, 30000),
			HAB_Actuator("POD_3", ACT3_EN, ACT3_PUSH, ACT3_PULL, ACT3_POS, HEAT3_EN, THERMISTOR1, 30000, 40000),
			HAB_Actuator("POD_4", ACT4_EN, ACT4_PUSH, ACT4_PULL, ACT4_POS, HEAT4_EN, THERMISTOR1, 40000, 39000), //If closing alt below opening alt, must wait until we're below instead
		};
		actuatorReadings _actReadingsArray[] = {
			actuatorReadings(),
			actuatorReadings(),
			actuatorReadings(),
			actuatorReadings()
		};	
		//THESE TWO ARRAYS MUST BE THE SAME SIZE
		//Put a startup check for this

		//Used when our closing altitude is lower than our opening altitude
		bool enteredInterval = false;

    //----------------------------------------------------------\
    //Ethernet, UDP, telemetry----------------------------------|
        //Connection status
        bool noConnection = true; //Initially set to true

        //Last heartbeat and initialization attempt times
        unsigned long lastHeartbeat = 0;
        unsigned long lastInit = 0;

        //Buffer to hold incoming/outgoing packets
        char rcvBuffer[UDP_TX_PACKET_MAX_SIZE];
        char sendBuffer[UDP_TX_PACKET_MAX_SIZE];

        //Command arguments
        char firstArg[ARGUMENT_MAX_LENGTH];
        char secondArg[ARGUMENT_MAX_LENGTH];
        char thirdArg[ARGUMENT_MAX_LENGTH];
        
        //Creates the UDP connection object, IP address
        EthernetUDP _conn;
            //Balloon address
            IPAddress _localIP(LOCAL_IP_O1, LOCAL_IP_O2, LOCAL_IP_O3, LOCAL_IP_O4);
            byte _localMAC[] = MAC; 
    
            //Groundstation address
            IPAddress _GSIP(GS_IP_O1, GS_IP_O2, GS_IP_O3, GS_IP_O4);
            
            //PRISM address
            IPAddress _PRISMIP(PRISM_IP_O1, PRISM_IP_O2, PRISM_IP_O3, PRISM_IP_O4);

        //Creates and joins the UDP multicast group
		//HERE

        //Startup check bypasses
        bool podBypass = false;
        bool loggingBypass = false;
        bool cameraBypass = false;
        bool bmeBypass = false;
        bool gpsBypass = false;
        //bool finalVerification = false; //Bad in case program crashes and restarts   
		
		
//---------------------------------------------------------------------------------------------\
//                                            Setup                                            |
//---------------------------------------------------------------------------------------------/


    void setup() {
        //Serial setup
        Serial.begin(9600);

        //----------------------------------------------------------\
        //Setup objects---------------------------------------------|
            //Set up logging
            HAB_Logging::setChip(4);
            genStringPtr = HAB_Logging::getStringPtr();
    
            //Start message
            printHeader(); //Print the header
    
            //Gets the length of the actuator array
            act_arr_len = sizeof(_actArray) / sizeof(_actArray[0]);
    
            //Set up the Excel file
            HAB_Logging::initExcelFile(act_arr_len);
       
            //Sets up the GPS
            _gps = HAB_GPS(GPS_RX_PIN, GPS_TX_PIN);
    
            //Sets up the camera
            _cam = HAB_Camera(SD_CHIPSELECT, CAM1_RX_PIN, CAM1_TX_PIN);
                _cam.emptyImageBuffer(); //Ensures the buffer is empty beforehand

        //----------------------------------------------------------\
        //Begin and check startup conditions------------------------|  
            checkStartupConditions();

        //----------------------------------------------------------\
        //Startup checks passed, begin program----------------------|
            HAB_Logging::printLog("\r\n                     *!GPS lock obtained!*\r\n", "");
            printInfo();
    }


//---------------------------------------------------------------------------------------------\
//                                            Loop                                             |
//---------------------------------------------------------------------------------------------/


    void loop() {
        
        //----------------------------------------------------------\
        //Actuators-------------------------------------------------|
            //This will open/close pods based on the altitudes specified in the actuator constructor calls
            if(activeIndex < act_arr_len){ //Makes sure the actuator exists
                handleActuator(_actArray + activeIndex);
            }

        //----------------------------------------------------------\
        //Telecommands-----------------------------------------------|
             recievePacketsUDP();
       
        //----------------------------------------------------------\
        //GPS readings----------------------------------------------|
             if(_gps.getLockStatus()){
                //_GPSreadings = _gps.getReadings();
                _GPSreadings.second = _gps.getReadings()->time.second();
                _GPSreadings.minute = _gps.getReadings()->time.minute();
                _GPSreadings.hour = _gps.getReadings()->time.hour();
                _GPSreadings.speed = _gps.getReadings()->speed.mps();
                _GPSreadings.altitude = _gps.getReadings()->altitude.meters();
                _GPSreadings.latitude = _gps.getReadings()->location.lat();
                _GPSreadings.longitude = _gps.getReadings()->location.lng();               
             }
             else{
                //Get readings from the multicast group here
                //_GPSreadings must be put in a form usable by the rest of the functions then
                //Must implement a function for reading telemetry from the multicast group

                _GPSreadings.second = millis()/1000;
                _GPSreadings.minute = millis()/1000;
                _GPSreadings.hour = millis()/1000;
                _GPSreadings.speed = 200;
                _GPSreadings.altitude = millis()/1000*_GPSreadings.speed;
                _GPSreadings.latitude = 30;
                _GPSreadings.longitude = -123;      
             }

        //----------------------------------------------------------\
        //Log and transmit readings---------------------------------|
            if((millis() - lastReadingsTime) > READINGS_TIME_STEP){
                //Sets the new last readings time
                lastReadingsTime = millis();
                
                //BME readings----------------------------------------------|
                    _BMEreadings.temperature = _bme.readTemperature();
                    _BMEreadings.pressure = _bme.readPressure();
                    _BMEreadings.humidity = _bme.readHumidity();
        
                //Actuator readings-----------------------------------------|
                    for(int i = 0; i != act_arr_len; i++){
                        _actReadingsArray[i].position = _actArray[i].getPosition();
                        _actReadingsArray[i].temperature = _actArray[i].getTemperature();
                        strcpy(_actReadingsArray[i].actuatorStatusPtr, (_actArray[i].isActuatorOverridden() ? (_actArray[i].isActuatorOverrideOpen() ? "OVR_OPEN" : "OVR_CLOSE") : "AUTO"));
                        strcpy(_actReadingsArray[i].heaterStatusPtr, (_actArray[i].isHeaterOverridden() ? (_actArray[i].isHeaterOverrideEnabled() ? "OVR_ENABLED" : "OVR_DISABLED") : "AUTO"));
                    }
    
                //Logging and telemetry-------------------------------------|
                    //Section for handling logging
                     HAB_Logging::writeToExcel(_BMEreadings, _GPSreadings, _actReadingsArray, act_arr_len);   
                     sendTelemetry();
            }

        //----------------------------------------------------------\
        //GPS feed and camera writing-------------------------------|
            //Feeds input to the GPS receiver to get new data
            _gps.feedReceiver();
    
            //If there is data in the camera buffer, writes it to the SD card
            _cam.writeImage();

        //----------------------------------------------------------\
        //Check if the balloon has descended------------------------|
            //THIS IS CURRENTLY NEVER TRIGGERED
            if(_GPSreadings.altitude < STOP_ALTITUDE && isDescending){
                //This feels slightly unsafe to do
                //exit(0);
            }

        Serial.print("Alt : "); Serial.println(_GPSreadings.altitude); //REMOVE THIS LATER
        delay(1000);
    }


//---------------------------------------------------------------------------------------------\
//                                          Functions                                          |
//---------------------------------------------------------------------------------------------/


    /*-------------------------------------------------------------------------------------*\
    |   Name:       handleActuator                                                          |
    |   Purpose:    Used to open and close the pods, and maintain proper temperature        |
    |               of the actuators when they are required to move.                        |
    |   Arguments:  Actuator                                                                |
    |   Returns:    Void                                                                    |
    \*-------------------------------------------------------------------------------------*/
        void handleActuator(HAB_Actuator* actuator){

            //----------------------------------------------------------\
            //Actuator movement-----------------------------------------|           
                //Handle opening-----------------------------------------------//
                    //If in interval (or overridden open), not fully open, not opening, has not opened before: start opening
                    if((actuator->isInInterval(_GPSreadings.altitude) || (actuator->isActuatorOverridden() && actuator->isActuatorOverrideOpen())) && !actuator->isFullyOpen() && !actuator->isMoveEnabled() && !actuator->getHasOpened()){
                        //Starts opening the pod
                        actuator->retract();
                                                   
                        //Optional picture
                    }
                    //Else if in interval (or overridden open), fully opened, and is moving: halt movement
                    else if((actuator->isInInterval(_GPSreadings.altitude) || (actuator->isActuatorOverridden() && actuator->isActuatorOverrideOpen())) && actuator->isFullyOpen() && actuator->isMoveEnabled()){
                        //Halts the actuator
                        actuator->halt();
    
                        //Creates the name of the image and attempts capture
                        strcpy(imgNamePtr, "IMAGE_"); strcat(imgNamePtr, itoa(imgCount++, genStringPtr, 10));
                        strcat(imgNamePtr, "_"); strcat(imgNamePtr, actuator->getName()); strcat(imgNamePtr, "_OPEN.JPG");
                        _cam.captureImage(imgNamePtr, 0);
                    }

                //Handle closing-----------------------------------------------//
                    //If not in interval (or overridden close), open, not closing: start closing
                    if((!actuator->isInInterval(_GPSreadings.altitude) || (actuator->isActuatorOverridden() && !actuator->isActuatorOverrideOpen())) && !actuator->isClosed() && !actuator->isMoveEnabled()){
                        //Starts closing the pod
                        actuator->extend();

                        //Sets hasOpened if it hasn't already (e.g. program crashed)
                        actuator->setHasOpened(true);
                                                
                        //Optional picture
                    }
                    //Else if not in interval (or overridden close), and is closed
                    else if((!actuator->isInInterval(_GPSreadings.altitude) || (actuator->isActuatorOverridden() && !actuator->isActuatorOverrideOpen())) && actuator->isClosed()){

                        //If overridden close or not overridden, and move is enabled: halts (avoids disabling in case of overridden-open), take picture
                        if((!actuator->isActuatorOverridden() || (actuator->isActuatorOverridden() && !actuator->isActuatorOverrideOpen())) && actuator->isMoveEnabled()){
                            actuator->halt();

                            //Creates the name of the image and attempts capture
                            strcpy(imgNamePtr, "IMAGE_"); strcat(imgNamePtr, itoa(imgCount++, genStringPtr, 10));
                            strcat(imgNamePtr, "_"); strcat(imgNamePtr, actuator->getName()); strcat(imgNamePtr, "_CLOSED.JPG");
                            _cam.captureImage(imgNamePtr, 0);
                        }

                        //If not overridden, and move not enabled
                        if(!actuator->isActuatorOverridden() && !actuator->isMoveEnabled()){

                            //If beyond the closing altitude, disables and gets next actuator
                            if((actuator->getCloseAlt() > actuator->getOpenAlt()) && (_GPSreadings.altitude > actuator->getCloseAlt())){
                                actuator->deactivateAll();
                                enteredInterval = false;
                                activeIndex++;
                            }
                            else if((actuator->getCloseAlt() <= actuator->getOpenAlt()) && (_GPSreadings.altitude < actuator->getCloseAlt())){
                                actuator->deactivateAll();
                                enteredInterval = false;
                                activeIndex++;
                            }                              
                        }
                    }        

            //----------------------------------------------------------\
            //Heating---------------------------------------------------|
                //If its not overridden
                if(!actuator->isHeaterOverridden()){
                    //If we're below the temperature limit and not currently heating: start heating
                    if(actuator->getTemperature() <= minTemp && !actuator->isHeatEnabled()){
                         actuator->startHeating();            
                    }
                    //Else if we're above the temperature limit and currently heating: stop heating
                    else if(actuator->getTemperature() > maxTemp && actuator->isHeatEnabled()){
                        actuator->stopHeating();
                    }
                }
                else{
                    //If heater is not already doing what we want (check its status first), then set it
                    if(actuator->isHeaterOverrideEnabled() && !actuator->isHeatEnabled()){
                        actuator->startHeating();
                    }
                    else if(!actuator->isHeaterOverrideEnabled() && actuator->isHeatEnabled()){
                        actuator->stopHeating();
                    }
                }
        }

    /*-------------------------------------------------------------------------------------*\
    |   Name:       getPodIndex                                                             |
    |   Purpose:    Gets the index of a pod by name. -1 if no such pod.                     |
    |   Arguments:  char*                                                                   |
    |   Returns:    int8_t                                                                  |
    \*-------------------------------------------------------------------------------------*/
        int8_t getPodIndex(char* podName){
            //If "NONE" given, returns a not used index
            if(strcmp(podName, "NONE") == 0){ return act_arr_len; }

            //Else searches for the pod name
            for(int i = 0; i != act_arr_len; i++){ 
                //If it finds the actuator return its index
                if(strncmp(podName, _actArray[i].getName(), sizeof(podName)) == 0){
                    return i;
                }
                //Else return -1 (not found)
                return -1;
            }
        }

    /*-------------------------------------------------------------------------------------*\
    |   Name:       recievePacketsUDP                                                       |
    |   Purpose:    Reads in UDP packets and deals with them accordingly.                   |
    |   Arguments:  void                                                                    |
    |   Returns:    void                                                                    |
    \*-------------------------------------------------------------------------------------*/
        void recievePacketsUDP(){
            //Gets the size of the packet (0 if no packet)
            int pktSize = _conn.parsePacket();

            //If there was a packet
            if(pktSize){
                //Read the packet from the buffer
                _conn.read(rcvBuffer, UDP_TX_PACKET_MAX_SIZE);

                //Converts all ASCII characters in the packet to upper case and ends the string if non-ascii characters found
                for(uint16_t i = 0; sizeof(rcvBuffer); i++){
                    //If its a non-ascii character, replace it with the null terminator and break
                    if((uint32_t)rcvBuffer[i] > 255){
                        rcvBuffer[i] = '\0';
                        break;
                    }

                    //Converts the character to upper case
                    rcvBuffer[i] = toupper(rcvBuffer[i]);
                 }

                //If it was a heartbeat packet, record the last time
                if(strcmp(rcvBuffer, "HBT") == 0){
                    lastHeartbeat = millis();
                    if(noConnection){
                        HAB_Logging::printLogln("Connection obtained!");
                        //Print some info about the connection? Also at the start
                        noConnection = false;
                    }                   
                }
                //If not a heartbeat, attempt to interpret it as a command
                else{
                    handleCommand(rcvBuffer);
                }

                //Wipes the buffer
                memset(rcvBuffer, 0, UDP_TX_PACKET_MAX_SIZE);
            }
            //No packet
            else{
                if((millis() - lastHeartbeat) > HEARTBEAT_TIMEOUT && !noConnection){               
                    noConnection = true;
                    HAB_Logging::printLogln("Connection lost!");

                    //Releases any actuator overrides
                    if(_actArray[activeIndex].isActuatorOverridden()){
                        _actArray[activeIndex].overrideActuatorRelease();
                    }
                    //Releases heater overrides if its set to 'OFF' (does not if overridden to 'ON')
                    if(_actArray[activeIndex].isHeaterOverridden() && !_actArray[activeIndex].isHeaterOverrideEnabled()){
                        _actArray[activeIndex].overrideHeaterRelease(); 
                    }                  
                }
            }

            //If no connection, attempts to reinitialize it every second
            if((millis() - lastInit) > RECONNECT_DELAY && noConnection){
                lastInit = millis();
                sendGSmessage("INIT");
            }
        }

    /*-------------------------------------------------------------------------------------*\
    |   Name:       handleCommands                                                          |
    |   Purpose:    Interprets the given string and executes it if it is a command.         |
    |   Arguments:  char*                                                                   |
    |   Returns:    void                                                                    |
    \*-------------------------------------------------------------------------------------*/        
        void handleCommand(char* command){

            //----------------------------------------------------------\
            //Parse the command-----------------------------------------|
                //Used for replying back the result
                bool validCommand = true;

                //Outputs the recieved command
                HAB_Logging::printLog("GROUNDSTATION : ");
                HAB_Logging::printLogln(command, "");
                
                //Gets the first argument
                char* argPtr = strtok(command, DELIMITER);
                strncpy(firstArg, argPtr, ARGUMENT_MAX_LENGTH);
    
                //Gets the second argument
                argPtr = strtok(NULL, DELIMITER);
                strncpy(secondArg, argPtr, ARGUMENT_MAX_LENGTH);
    
                //Gets the third argument
                argPtr = strtok(NULL, DELIMITER);
                strncpy(thirdArg, argPtr, ARGUMENT_MAX_LENGTH);

            //----------------------------------------------------------\
            //Execute the command---------------------------------------|
                //Bypasses--------------------------------------------------|
                    if(!strcmp(firstArg, "POD_BYPASS"))         { podBypass     = true; }
                    else if(!strcmp(firstArg, "LOGGING_BYPASS")){ loggingBypass = true; }
                    else if(!strcmp(firstArg, "CAMERA_BYPASS")) { cameraBypass  = true; }
                    else if(!strcmp(firstArg, "BME_BYPASS"))    { bmeBypass     = true; }
                    else if(!strcmp(firstArg, "GPS_BYPASS"))    { gpsBypass     = true; }
    
                //Actuators-------------------------------------------------|
                    else if(!strcmp(firstArg, "SET_ACTIVE")){
                        if(strcmp(secondArg, "") != 0 && strcmp(thirdArg, "") != 0 && getPodIndex(secondArg) != -1 && atof(thirdArg) >= 0){ validCommand = false; }
                        else{ activeIndex = getPodIndex(secondArg); }
                    }  
                    else if(!strcmp(firstArg, "SET_OPEN_ALT")){
                        if(strcmp(secondArg, "") != 0 && strcmp(thirdArg, "") != 0 && getPodIndex(secondArg) != -1 && atof(thirdArg) >= 0){ validCommand = false; }
                        else{ _actArray[getPodIndex(secondArg)].setOpenAltitude(atof(thirdArg)); }
                    }
                    else if(!strcmp(firstArg, "SET_CLOSE_ALT")){
                        if(strcmp(secondArg, "") != 0 && strcmp(thirdArg, "") != 0 && getPodIndex(secondArg) != -1 && atof(thirdArg) >= 0){ validCommand = false; }
                        else{ _actArray[getPodIndex(secondArg)].setOpenAltitude(atof(thirdArg)); }
                    }
                    else if(!strcmp(firstArg, "SET_OPEN_ALT")){
                        if(strcmp(secondArg, "") != 0 && strcmp(thirdArg, "") != 0 && getPodIndex(secondArg) != -1 && atof(thirdArg) >= 0){ validCommand = false; }
                        else{ _actArray[getPodIndex(secondArg)].setOpenAltitude(atof(thirdArg)); }
                    }           
                    else if(!strcmp(firstArg, "OVR_ACT_OPEN"))   { _actArray[activeIndex].overrideActuatorOpen();    }
                    else if(!strcmp(firstArg, "OVR_ACT_CLOSE"))  { _actArray[activeIndex].overrideActuatorClose();   }
                    else if(!strcmp(firstArg, "OVR_ACT_RELEASE")){ _actArray[activeIndex].overrideActuatorRelease(); }
                    
                //Heaters---------------------------------------------------|
                    else if(!strcmp(firstArg, "SET_MIN_TEMP")){
                        if(strcmp(secondArg, "") != 0 && atof(secondArg) >= -20 && atof(secondArg) <= 30){ validCommand = false; } //We allow a 50 degree range. This sets the minimum for ALL heaters.
                        else{ minTemp = atof(secondArg); }
                    }
                    else if(!strcmp(firstArg, "SET_MAX_TEMP")){
                        if(strcmp(secondArg, "") != 0 && atof(secondArg) >= -20 && atof(secondArg) <= 30){ validCommand = false; } //We allow a 50 degree range. This sets the maximum for ALL heaters.
                        else{ maxTemp = atof(secondArg); }
                    }
                    else if(!strcmp(firstArg, "OVR_HEAT_ENABLE")) { _actArray[activeIndex].overrideHeaterEnable();  }
                    else if(!strcmp(firstArg, "OVR_HEAT_DISABLE")){ _actArray[activeIndex].overrideHeaterDisable(); }
                    else if(!strcmp(firstArg, "OVR_HEAT_RELEASE")){ _actArray[activeIndex].overrideHeaterRelease(); }

                //Else if not any of those, it is invalid
                else{ validCommand = false; }

            //----------------------------------------------------------\
            //Send result message---------------------------------------|
                HAB_Logging::printLogln(validCommand ? "Command executed!" : "Invalid command!");
                sendGSmessage(validCommand ? "Command executed!" : "Invalid command!");         
        }

    /*-------------------------------------------------------------------------------------*\
    |   Name:       sendGSmessage                                                           |
    |   Purpose:    Sends a message to the ground station.                                  |
    |   Arguments:  char*                                                                   |
    |   Returns:    void                                                                    |
    \*-------------------------------------------------------------------------------------*/
        void sendGSmessage(const char* msg){
            //Send a timestamp as well?
            _conn.beginPacket(_GSIP, GS_PORT);
            _conn.write(msg);
            _conn.endPacket();
        }
        
    /*-------------------------------------------------------------------------------------*\
    |   Name:       sendTelemetry                                                           |
    |   Purpose:    Sends telemetry station to our groundstation and CSA's PRISM.           |
    |   Arguments:  void                                                                    |
    |   Returns:    void                                                                    |
    \*-------------------------------------------------------------------------------------*/
        void sendTelemetry(){
            //Formats the packet to PRISM's standards
            strcpy(sendBuffer, ",,");
            strcat(sendBuffer, _gps.getDate(HAB_Logging::getStringPtr()));
            strcat(sendBuffer, " ");
            strcat(sendBuffer, HAB_Logging::getTimeFormatted());
            strcat(sendBuffer, ",HAB,");
            strcat(sendBuffer, dtostrf(_GPSreadings.altitude,    6, 3, genStringPtr)); strcat(sendBuffer, ",");
            strcat(sendBuffer, dtostrf(_GPSreadings.speed,       6, 3, genStringPtr)); strcat(sendBuffer, ",");
            strcat(sendBuffer, dtostrf(_GPSreadings.longitude,   6, 3, genStringPtr)); strcat(sendBuffer, ",");
            strcat(sendBuffer, dtostrf(_GPSreadings.latitude,    6, 3, genStringPtr)); strcat(sendBuffer, ",");
            strcat(sendBuffer, dtostrf(_BMEreadings.temperature, 6, 3, genStringPtr)); strcat(sendBuffer, ",");
            strcat(sendBuffer, dtostrf(_BMEreadings.pressure,    6, 3, genStringPtr)); strcat(sendBuffer, ",");
            strcat(sendBuffer, dtostrf(_BMEreadings.humidity,    6, 3, genStringPtr));
			//Statuses of each actuator
			for(int i = 0; i != act_arr_len; i++){
				strcat(sendBuffer, ",");
                strcat(sendBuffer, dtostrf(_actReadingsArray[i].position, 6, 3, genStringPtr));
				strcat(sendBuffer, ",");
                strcat(sendBuffer, dtostrf(_actReadingsArray[i].temperature, 4, 0, genStringPtr));
				strcat(sendBuffer, ",");
				//Status of actuator override: auto(none), open, close
                strcat(sendBuffer, (_actArray[i].isActuatorOverridden() ? (_actArray[i].isActuatorOverrideOpen() ? "1" : "0") : "2")); //OVR_OPEN(1), OVR_CLOSE(0), AUTO(2)
                strcat(sendBuffer, ",");
                //Status of heater override: auto(none), enabled, disabled
                strcat(sendBuffer, (_actArray[i].isHeaterOverridden() ? (_actArray[i].isHeaterOverrideEnabled() ? "1" : "0") : "2")); //OVR_ENABLE(1), OVR_DISABLE(0), AUTO(2)
                strcat(sendBuffer, ",");
            }

            //Appends the end of the packet
            strcat(sendBuffer, "\r\n");
            //strcat(sendBuffer, 0x0D); strcat(rcvBuffer, 0x0A);
            
            //Sends the packet to our groundstation
            _conn.beginPacket(_GSIP, GS_PORT);
            _conn.write(sendBuffer);
            _conn.endPacket();

            //Sends the packet to PRISM
            _conn.beginPacket(_PRISMIP, PRISM_PORT);
            _conn.write(sendBuffer);
            _conn.endPacket();
			
            //Format:			
			//,,2017-04-08 05:10:02,HAB,[altitude],[speed],[londitude],[latitude],[temperature],[pressure],[humidity],[act1_pos],[act1_temp],[act1_override status],[htr1_override status],[act2_pos],[act2_temp],[act2_override status],[htr2_override status],[act3_pos],[act3_temp],[act3_override status],[htr3_override status],[act4_pos],[act4_temp],[act4_override status],[htr4_override status]
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
    |   Name:       printInfo                                                               |
    |   Purpose:    Prints some useful info.                                                |
    |   Arguments:  void                                                                    |
    |   Returns:    void                                                                    |
    \*-------------------------------------------------------------------------------------*/
        void printInfo(){
            //Print author and team info
            HAB_Logging::printLogln("\r\nAuthor:", ""); HAB_Logging::printLogln("---------------", ""); HAB_Logging::printLogln("Stephen Amey", "");
            //Add rest of info here
        
            //Print the date from the GPS
            HAB_Logging::printLogln("\r\n\r\nFlight date:", ""); HAB_Logging::printLogln("---------------", ""); HAB_Logging::printLogln(_gps.getDate(genStringPtr), "");
        
            //Print out the camera info
            HAB_Logging::printLogln("\r\n\r\nCamera info:", ""); HAB_Logging::printLogln("---------------", ""); HAB_Logging::printLogln(_cam.getInfo(genStringPtr), "");
        
            //Print out GPS info
            HAB_Logging::printLogln("\r\n\r\n", "");
            _gps.printInfo();
               
            //Print out final header
            HAB_Logging::printLogln("\r\n##############################################################", "");
            HAB_Logging::printLogln(    "#                       Logging begins                       #", "");
            HAB_Logging::printLogln(    "##############################################################\r\n\r\n", "");
        }


    /*-------------------------------------------------------------------------------------*\
    |   Name:       checkStartupConditions                                                  |
    |   Purpose:    Holds the program until all startup conditions passed.                  |
    |   Arguments:  void                                                                    |
    |   Returns:    void                                                                    |
    \*-------------------------------------------------------------------------------------*/
        void checkStartupConditions(){

            //----------------------------------------------------------\
            //Telemetry check-------------------------------------------|
                //Sets up the ethernet
                Ethernet.begin(_localMAC, _localIP);
                while(!_conn.begin(LOCAL_PORT));
                HAB_Logging::printLogln("UDP OKAY");

                //Obtain a connection to the ground station
                while(noConnection){
                    recievePacketsUDP();
                }
                HAB_Logging::printLogln("CONNECTION OKAY");
                sendGSmessage("CONNECTION OKAY");

            //----------------------------------------------------------\
            //Pod check-------------------------------------------------|
                for(int i = 0; i != act_arr_len; i++){
                    if(!_actArray[i].isClosed()){
                        strcpy(genStringPtr, _actArray[i].getName());
                        strcat(genStringPtr, " is not closed.");
                        HAB_Logging::printLogln(genStringPtr);              
                        sendGSmessage(genStringPtr);
                        while(!podBypass){ recievePacketsUDP(); }
                            podBypass = false;
                    }
                }
                HAB_Logging::printLogln("PODS OKAY");              
                sendGSmessage("PODS OKAY");
                //Set status light
            
            //----------------------------------------------------------\
            //Logging check---------------------------------------------|
                while(!HAB_Logging::checkReady() && !loggingBypass){ recievePacketsUDP(); }
                    HAB_Logging::printLogln("LOGGING OKAY");              
                    sendGSmessage("LOGGING OKAY");
                    //Set status light
    
            //----------------------------------------------------------\
            //Camera check----------------------------------------------|
                while(!(_cam.getReadyStatus() && !_cam.getBufferStatus()) && !cameraBypass){ recievePacketsUDP(); }
                    HAB_Logging::printLogln("CAMERA OKAY");              
                    sendGSmessage("CAMERA OKAY");
                    //Set status light
    
            //----------------------------------------------------------\
            //BME check-------------------------------------------------|
                while(!_bme.begin() && !bmeBypass){ recievePacketsUDP(); }
                    HAB_Logging::printLogln("BME OKAY");              
                    sendGSmessage("BME OKAY");
                    //Set status light
                    //_bme.begin(); //This isn't needed?
    
            //----------------------------------------------------------\
            //GPS lock check--------------------------------------------|
                while(!_gps.getLockStatus() && !gpsBypass){ recievePacketsUDP(); }
                    HAB_Logging::printLogln("GPS LOCK OKAY");              
                    sendGSmessage("GPS LOCK OKAY");
                    //Set status light
        }