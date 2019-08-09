/*
*	Author	:	Stephen Amey
*	Date	:	June 25, 2019
*	Purpose	: 	This library is used to interface a GPS receiver.
*				It is specifically tailored to the Western University HAB project.
*/


#ifndef HAB_GPS_h
#define HAB_GPS_h


//--------------------------------------------------------------------------\
//								    Imports					   				|
//--------------------------------------------------------------------------/


	#include "Arduino.h"
	#include <SoftwareSerial.h>
	#include <SPI.h>
	#include <TinyGPS++.h>
	#include <HAB_Logging.h>
	

class HAB_GPS {
	
	
	//--------------------------------------------------------------------------\
	//								  Definitions					   			|
	//--------------------------------------------------------------------------/
	
	
		#ifndef GPS_MAX_AGE
			#define GPS_MAX_AGE 10000
		#endif
		#ifndef GPS_BAUD
			#define GPS_BAUD 9600
		#endif
	

	//--------------------------------------------------------------------------\
	//								   Variables					   			|
	//--------------------------------------------------------------------------/
		private:
			
		//Holds gps data
		TinyGPSPlus gpsData;
		
		//Reference to the passed serial
		SoftwareSerial gpsSerial;
		
		//Holds a reference to the logging stringPtr
		char* stringPtr;
     
	
	//--------------------------------------------------------------------------\
	//								  Constructor					   			|
	//--------------------------------------------------------------------------/
		public:
	
		HAB_GPS(uint8_t rxPin, uint8_t txPin);
		HAB_GPS(void);
		

	//--------------------------------------------------------------------------\
	//								   Functions					   			|
	//--------------------------------------------------------------------------/
	
	
		//--------------------------------------------------------------------------------\
		//Getters-------------------------------------------------------------------------|
			char* getInfo(char* stringPtr);	
			char* getDate(char* stringPtr);
			char* getTime(char* stringPtr);
			bool getLockStatus();		
			TinyGPSPlus* getReadings();
		
		
		//--------------------------------------------------------------------------------\
		//Setters-------------------------------------------------------------------------|
		
		
		//--------------------------------------------------------------------------------\
		//Miscellaneous-------------------------------------------------------------------|
			void feedReceiver();
			void printInfo();
};

#endif
