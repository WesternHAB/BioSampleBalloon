/*
*	Author	:	Stephen Amey
*	Date	:	June 25, 2019
*	Purpose	: 	This library is used to interface with one or more GPS receivers.
*				It is specifically tailored to the Western University HAB project.
*/

//--------------------------------------------------------------------------\
//								    Imports					   				|
//--------------------------------------------------------------------------/


	#include "HAB_GPS.h"
	
	
//--------------------------------------------------------------------------\
//								  Constructor					   			|
//--------------------------------------------------------------------------/


	HAB_GPS::HAB_GPS(uint8_t rxPin, uint8_t txPin) : gpsSerial (rxPin, txPin){
		gpsSerial.begin(GPS_BAUD);
		
		//Gets a reference to the logging stringPtr
		stringPtr = HAB_Logging::getStringPtr();
	}
	
	HAB_GPS::HAB_GPS() : gpsSerial (0, 0){
		gpsSerial.begin(GPS_BAUD);
		
		//Gets a reference to the logging stringPtr
		stringPtr = HAB_Logging::getStringPtr();
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
			char* HAB_GPS::getInfo(char* stringPtr){
				//If no camera found
				/*if(!cameraFound){ 
					strcpy(stringPtr, "No camera found.");
				}	
				else{			
					strcpy(stringPtr, cam.getVersion());
					if(!stringPtr){ strcpy(stringPtr, "Failed to get version"); }
				}
				return stringPtr;*/
				
				return stringPtr;
			}

		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		getDate																	|
		|	Purpose: 	Returns the current date.												|
		|	Arguments:	void																	|
		|	Returns:	char*																|
		\*-------------------------------------------------------------------------------------*/
			char* HAB_GPS::getDate(char* stringPtr){
				//Sets to the stringPtr pointer
				sprintf(stringPtr, "%d/%d/%d (UTC) ", gpsData.date.day(), gpsData.date.month(), gpsData.date.year());	
				return stringPtr;
			}
			
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		getTime																	|
		|	Purpose: 	Returns the current time.												|
		|	Arguments:	void																	|
		|	Returns:	char*																|
		\*-------------------------------------------------------------------------------------*/
			char* HAB_GPS::getTime(char* stringPtr){
				//Sets to the stringPtr pointer
				sprintf(stringPtr, "%d:%d:%d (UTC) ", gpsData.time.hour(), gpsData.time.minute(), gpsData.time.second());	
				return stringPtr;
			}
			
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		getReadings																|
		|	Purpose: 	Returns the most recent readings.										|
		|	Arguments:	void																	|
		|	Returns:	Readings																|
		\*-------------------------------------------------------------------------------------*/
			TinyGPSPlus* HAB_GPS::getReadings(){
				return &gpsData;
			}
			
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		getLockStatus															|
		|	Purpose: 	Checks to see if the receiver has a complete lock.						|
		|	Arguments:	void																	|
		|	Returns:	bool																	|
		\*-------------------------------------------------------------------------------------*/
			bool HAB_GPS::getLockStatus(){	
				feedReceiver();
				
				//Returns once we have enough valid data
				return (
					gpsData.location.age() < GPS_MAX_AGE
					&& gpsData.location.isValid()
					&& gpsData.date.isValid()
					&& gpsData.time.isValid()
					&& gpsData.altitude.isValid()
					&& gpsData.speed.isValid()
					&& gpsData.course.isValid()
					&& gpsData.satellites.isValid()
				);
			}
			
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		isAscending																|
		|	Purpose: 	Checks to see if the GPS is ascending.									|
		|	Arguments:	void																	|
		|	Returns:	bool																	|
		\*-------------------------------------------------------------------------------------*/
			bool HAB_GPS::isAscending(){	
				
				//Need to record last several altitude and check if e.g. slope is positive?
				
				return true;
			}

            
	//--------------------------------------------------------------------------------\
	//Setters-------------------------------------------------------------------------|
	

	//--------------------------------------------------------------------------------\
	//Miscellaneous-------------------------------------------------------------------|	
			
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		feedReceiver															|
		|	Purpose: 	Feed the GPS object with data from the receiver.						|
		|				Update readings when valid.												|
		|	Arguments:	void																	|
		|	Returns:	void																	|
		\*-------------------------------------------------------------------------------------*/
			void HAB_GPS::feedReceiver(){				
				while(gpsSerial.available()){
					gpsData.encode(gpsSerial.read());
				}
				
				//If Alt does not equal last alt, add it
				
				//Add it to the 'trend'
				//Trend can only hold e.g. 5 readings defined by a #define 5
				
				
				
				
				
				
				
			}
				
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		printInfo																|
		|	Purpose: 	Prints the gps info														|
		|	Arguments:	void																	|
		|	Returns:	void																	|
		\*-------------------------------------------------------------------------------------*/
			void HAB_GPS::printInfo(){
				HAB_Logging::printLogln("*** GPS data dump ***", "");
				HAB_Logging::printLogln("", "");
			
			
				HAB_Logging::printLog("\tDate(UTC)  : ", "");
					HAB_Logging::printLog(itoa(gpsData.date.day(), stringPtr, 10), "");
					HAB_Logging::printLog(itoa(gpsData.date.month(), stringPtr, 10), "/");
					HAB_Logging::printLogln(itoa(gpsData.date.year(), stringPtr, 10), "/");
				HAB_Logging::printLog("\tTime(UTC)  : ", "");
					HAB_Logging::printLog(itoa(gpsData.time.hour(), stringPtr, 10), "");
					HAB_Logging::printLog(itoa(gpsData.time.minute(), stringPtr, 10), ":");
					HAB_Logging::printLogln(itoa(gpsData.time.second(), stringPtr, 10), ":");
				HAB_Logging::printLogln("\tLocation   : ", "");
					HAB_Logging::printLogln(dtostrf(gpsData.location.lat(), 10, 6, stringPtr), "\t\tLatitude(deg)  : ");
					HAB_Logging::printLogln(dtostrf(gpsData.location.lng(), 10, 6, stringPtr), "\t\tLongitude(deg) : ");
					HAB_Logging::printLogln(dtostrf(gpsData.altitude.meters(), 10, 6, stringPtr), "\t\tAltitude(m)    : ");
				HAB_Logging::printLogln("\tVelocity   : ", "");
					HAB_Logging::printLogln(dtostrf(gpsData.speed.mps(), 10, 6, stringPtr), "\t\tSpeed(m/s)     : ");
					HAB_Logging::printLogln(dtostrf(gpsData.course.deg(), 10, 6, stringPtr), "\t\tCourse(deg)    : ");
				HAB_Logging::printLog("\tSatellites : ", "");
					HAB_Logging::printLogln(itoa(gpsData.satellites.value(), stringPtr, 10), "");
				HAB_Logging::printLog("\tH-Dim.     : ", "");
					HAB_Logging::printLogln(itoa(gpsData.hdop.value(), stringPtr, 10), "");
			}