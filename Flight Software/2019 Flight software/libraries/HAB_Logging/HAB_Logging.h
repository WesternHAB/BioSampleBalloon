/*
*	Author	:	Stephen Amey
*	Date	:	June 17, 2019
*	Purpose	: 	This library is used to write log messages to an SD card. 
*				It is specifically tailored to the Western University HAB project.
*/


#ifndef HAB_Logging_h
#define HAB_Logging_h


//--------------------------------------------------------------------------\
//								    Imports					   				|
//--------------------------------------------------------------------------/
	
	
	#include "Arduino.h"
	#include <SD.h>
	

class HAB_Logging {


	//--------------------------------------------------------------------------\
	//								   Functions					   			|
	//--------------------------------------------------------------------------/
		public:
	
		static void setChip(uint8_t chipSelect);
		static void printLog(const char* msg, const char* prepend = HAB_Logging::getTimestamp());
		static void printLogln(const char* msg, const char* prepend = HAB_Logging::getTimestamp());
		static char* getTimestamp(void);
		static char* getStringPtr(void);
};

#endif