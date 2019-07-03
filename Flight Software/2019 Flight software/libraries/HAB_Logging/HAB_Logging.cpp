/*
*	Author	:	Stephen Amey
*	Date	:	June 17, 2019
*	Purpose	: 	This library is used to write log messages to an SD card. 
*				It is specifically tailored to the Western University HAB project.
*/

//--------------------------------------------------------------------------\
//								    Imports					   				|
//--------------------------------------------------------------------------/


	#include "HAB_Logging.h"
 

//--------------------------------------------------------------------------\
//                                 Variables                                |
//--------------------------------------------------------------------------/


   uint8_t chipSelect;
   char stringPtr[100] = "";
   char timestampPtr[15];

	
//--------------------------------------------------------------------------\
//								   Functions					   			|
//--------------------------------------------------------------------------/


	/*-------------------------------------------------------------------------------------*\
	| 	Name: 		setChip																	|
	|	Purpose: 	Sets the value for chipSelect											|
	|	Arguments:	integer																	|
	|	Returns:	void																	|
	\*-------------------------------------------------------------------------------------*/
		void HAB_Logging::setChip(uint8_t _chipSelect){
			chipSelect = _chipSelect;
          
            //Start with the new chipSelect
            if(SD.begin(chipSelect)) {
                Serial.println("Card found\r\n");
            }
            else{
                Serial.println("Card failed, or not present");
            }
		}
				
	/*-------------------------------------------------------------------------------------*\
	| 	Name: 		printLog																|
	|	Purpose: 	Logs the given string to the log file (no newline)						|
	|	Arguments:	char*																	|
	|	Returns:	void																	|
	\*-------------------------------------------------------------------------------------*/
		void HAB_Logging::printLog(const char* msg, const char* prepend){
            //If no SD card
            if(chipSelect == 0){
				Serial.println("No chip set!"); 
            }
			//If SD card, write to it
			else{
				//Prints to the SD card
				File dataFile = SD.open("log.txt", FILE_WRITE);
				dataFile.print(prepend);			
				dataFile.print(msg);	
				dataFile.close();
			}
			
			//Prints to serial
			Serial.print(prepend);
			Serial.print(msg);      
		}
		
	/*-------------------------------------------------------------------------------------*\
	| 	Name: 		printLogln																|
	|	Purpose: 	Logs the given string to the log file (with newline)					|
	|	Arguments:	char*																	|
	|	Returns:	void																	|
	\*-------------------------------------------------------------------------------------*/		
		void HAB_Logging::printLogln(const char* msg, const char* prepend){
            //If no SD card
            if(chipSelect == 0){
				Serial.println("No chip set!"); 
            }
			//If SD card, write to it
			else{
				//Prints to the SD card
				File dataFile = SD.open("log.txt", FILE_WRITE);
				dataFile.print(prepend);				
				dataFile.println(msg);	
				dataFile.close();
			}
			
			//Prints to serial
			Serial.print(prepend);
			Serial.println(msg);      
		}

		
	/*-------------------------------------------------------------------------------------*\
	| 	Name: 		getTimestamp															|
	|	Purpose: 	Returns a pointer to a character array containg the up-time				|
	|	in a [xx:xx:xx] format.																|
	|	Arguments:	void																	|
	|	Returns:	char*																	|
	\*-------------------------------------------------------------------------------------*/
		char* HAB_Logging::getTimestamp(){
			unsigned long uptime = millis()/1000;
				
			uint16_t hours = uptime / 3600;
				uptime = uptime % 3600;
			uint8_t minutes = uptime / 60;
				uptime = uptime % 60;
			uint8_t seconds = uptime;
			
			//Sets to the timestamp pointer
			sprintf(timestampPtr, "[%02d:%02d:%02d] ", hours, minutes, seconds);
			
			//Returns the pointer to the timestamp character array
			return timestampPtr;
		}
		
	/*-------------------------------------------------------------------------------------*\
	| 	Name: 		getStringPtr															|
	|	Purpose: 	Returns the pointer used for strings, for general usage.				|
	|	Arguments:	void																	|
	|	Returns:	char*																	|
	\*-------------------------------------------------------------------------------------*/
		char* HAB_Logging::getStringPtr(){
			return stringPtr;
		}