/*
*	Author	:	Stephen Amey
*	Date	:	June 25, 2019
*	Purpose	: 	This library is used to interface with one or more GPS receivers.
*				It is specifically tailored to the Western University HAB project.
*/


#ifndef HAB_GPS_Manager_h
#define HAB_GPS_Manager_h


//--------------------------------------------------------------------------\
//								    Imports					   				|
//--------------------------------------------------------------------------/


	#include "Arduino.h"
	#include <SoftwareSerial.h>
	#include <SPI.h>
	#include <TinyGPS++.h>
	#include <SD.h>
	#include <HAB_Logging.h>
	
	
	
	
	
	
	//Logging
	//TinyGPS
	
	
	
	//array of set size, stores GPS declarations
	//Constructor, pins (rx/tx)
	
	
	//Wait until lock / hasLock, numLocks?
	//getTime?
	
	
	
	//Structs have to be returned via reference anyways? Private pointer to last reading / update this one?
	
	//Return straight up values (Would need to gothrough entire GPS list each time)
	
	//Returns reference to GPS (can't add to trend stuff)
	
	//Return reference to structReadings (would need multiple?) (create one massive struct, add stuff later if needed)
	
	
	
	
	
	
	
	
	//Can get single-time reference to the readings, it gets updated occaisionally but without our knowledge when?
	
	//Continuously get new value, still no idea when updated?
	
	//GPS receivers need to be 'fed'
	
	
	
	
	//gps_instance.getLatitude() //Pulls from last reading before next encode, or does it save it?
	//gps_instance.getLongitude()
	
	//gps_instance.feed(); (on new full sentence, add to trend)
	
	
	
	
	
	//if/while ss.available, we got new data. loop through all of this, read it and encode it. One full sentence!
	//Might be safe to just use while, it operates pretty fast? How about when writing with a camera? How long does the full sentence take?
	
	//
	
	
	
	//Feed: for each
	//If serial available, read the data in

	

class HAB_GPS_Manager {
	
	//--------------------------------------------------------------------------\
	//								  Definitions					   			|
	//--------------------------------------------------------------------------/
	
	
		#ifndef GPS_ARRAY_SIZE
			#define GPS_ARRAY_SIZE 5
		#endif
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
		SoftwareSerial gpsSerials [GPS_ARRAY_SIZE]; //In constructor, initialize to NULL
		
		
		
		//Position/speed
		double latitude;
		double longitude;
		double altitude;
		double speed;
		double course;
		
		//Date
		uint16_t year;
		uint8_t month;
		uint8_t day;
			
		//Time
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
		
		//Misc
		uint32_t satellites;
				
		//For storing response strings
		char stringPtr[100] = "";
     
	
	//--------------------------------------------------------------------------\
	//								  Constructor					   			|
	//--------------------------------------------------------------------------/
		public:
	
		HAB_GPS_Manager(uint8_t rxPin, uint8_t txPin);
		HAB_GPS_Manager(void);
		

	//--------------------------------------------------------------------------\
	//								   Functions					   			|
	//--------------------------------------------------------------------------/
	
	
		//--------------------------------------------------------------------------------\
		//Getters-------------------------------------------------------------------------|
			char* getInfo();	
			char* getDate();
			//Readings* getReadings();
			char* getReadings();
			
			//getLatitude
			//getLongitude
			//getAlt
			//etc
		
		
		//--------------------------------------------------------------------------------\
		//Setters-------------------------------------------------------------------------|
		
		
		//--------------------------------------------------------------------------------\
		//Miscellaneous-------------------------------------------------------------------|
			void addReceiver(uint8_t rxPin, uint8_t txPin);
			void feedReceivers(); //Updates the readings variable when possible
			//void waitUntilLocationUpdate(); //Continuously calls feedReceivers? and checks if updated. 
			//Just check if the location data is valid / initialLock?
};

#endif
