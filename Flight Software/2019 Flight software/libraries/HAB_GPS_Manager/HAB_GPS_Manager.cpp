/*
*	Author	:	Stephen Amey
*	Date	:	June 25, 2019
*	Purpose	: 	This library is used to interface with one or more GPS receivers.
*				It is specifically tailored to the Western University HAB project.
*/


//Could instead just use the other GPS library and instantiate multiple.
//then check if they have locks and base it on that.

//--------------------------------------------------------------------------\
//								    Imports					   				|
//--------------------------------------------------------------------------/


	#include "HAB_GPS_Manager.h"
	
	
//--------------------------------------------------------------------------\
//								  Constructor					   			|
//--------------------------------------------------------------------------/


	HAB_GPS_Manager::HAB_GPS_Manager(uint8_t rxPin, uint8_t txPin){
		//Initialize the first gps serial
		gpsSerials[0] = SoftwareSerial(rxPin, txPin);
		
		//Initialize the rest to NULL
		for(int i = 1; i != GPS_ARRAY_SIZE; i++){
			gpsSerials[i] = NULL;
		}
	}
	
	HAB_GPS_Manager::HAB_GPS_Manager(){
		//Initialize all gps serials to NULL
		for(int i = 0; i != GPS_ARRAY_SIZE; i++){
			gpsSerials[i] = NULL;
		}
	}	
	
	
//--------------------------------------------------------------------------\
//								   Functions					   			|
//--------------------------------------------------------------------------/


	//--------------------------------------------------------------------------------\
	//Getters-------------------------------------------------------------------------|
		
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		getInfo																	|
		|	Purpose: 	Returns the gps info													|
		|	Arguments:	void																	|
		|	Returns:	char*																	|
		\*-------------------------------------------------------------------------------------*/
			char* HAB_GPS_Manager::getInfo(){
				//If no camera found
				/*if(!cameraFound){ 
					strcpy(stringPtr, "No camera found.");
				}	
				else{			
					strcpy(stringPtr, cam.getVersion());
					if(!stringPtr){ strcpy(stringPtr, "Failed to get version"); }
				}
				return stringPtr;*/
				
				return "";
			}

		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		getDate																	|
		|	Purpose: 	Returns the current date.												|
		|	Arguments:	void																	|
		|	Returns:	char*																|
		\*-------------------------------------------------------------------------------------*/
			char* HAB_GPS_Manager::getDate(){
				//return (cameraFound && sdFound);
				return "";
			}
			
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		getReadings																|
		|	Purpose: 	Returns the most recent readings.										|
		|	Arguments:	void																	|
		|	Returns:	Readings																	|
		\*-------------------------------------------------------------------------------------*/
			char* HAB_GPS_Manager::getReadings(){
				//return pointer/address
				//return (cameraFound && sdFound);
				
				return "";
			}

            
	//--------------------------------------------------------------------------------\
	//Setters-------------------------------------------------------------------------|
	

	//--------------------------------------------------------------------------------\
	//Miscellaneous-------------------------------------------------------------------|	
			
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		addReceiver																|
		|	Purpose: 	Add a new GPS receiver to the manager.									|
		|	Arguments:	rxPin(int), txPin(int)													|
		|	Returns:	void																	|
		\*-------------------------------------------------------------------------------------*/
			void HAB_GPS_Manager::addReceiver(uint8_t rxPin, uint8_t txPin){
				for(int i = 0; i != GPS_ARRAY_SIZE; i++){
					//If an empty spot, add it and return
					if(gpsSerials[i] == NULL){
						gpsSerials[i] = SoftwareSerial(rxPin, txPin);
						return;
					}
				}
				HAB_Logging::println("GPS manager receiver limit reached. Could not add new GPS receiver.");
			}			
			
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		feedReceivers															|
		|	Purpose: 	Feed the GPS object with data from the receivers.						|
		|				Update readings when valid.												|
		|	Arguments:	void																	|
		|	Returns:	void																	|
		\*-------------------------------------------------------------------------------------*/
			void HAB_GPS_Manager::feedReceivers(){	
				for(int i = 0; i != GPS_ARRAY_SIZE; i++){
					if(gpsSerials[i] == NULL){ break; }

					while(gpsSerials[i].available()){
						gpsData.encode(ss.read());
						
						//If new and valid sentence, update readings
						//if(gpsData.location.isValid() && gpsData.location.age() < GPS_MAX_AGE){
						if(gpsData.location.isUpdated() && gpsData.location.isValid()){
							//Position/speed
							latitude = gps.location.lat();
							longitude = gps.location.lng();
							altitude = gps.altitude.meters();
							speed = gps.speed.mps();
							course = gps.course.deg();
						}
						if(gpsData.date.isUpdated() && gpsData.date.isValid()){
							//Date
							year = gps.date.year();
							month = gps.date.month();
							day = gps.date.day();
						}
						if(gpsData.time.isUpdated() && gpsData.time.isValid()){
							//Time
							hour = gps.time.hour();
							minute = gps.time.minute();
							second = gps.time.second();
						}
						if(gpsData.satellites.isUpdated() && gpsData.satellites.isValid()){
							//Misc
							satellites = gps.satellites.value();
						}
					}
					
				}
			}