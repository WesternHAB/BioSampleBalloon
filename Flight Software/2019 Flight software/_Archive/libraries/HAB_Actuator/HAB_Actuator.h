/*
*	Author	:	Stephen Amey
*	Date	:	June 10, 2019
*	Purpose	: 	This library is used to interface with a linear actuator with position feedback. 
*				It is specifically tailored to the Western University HAB project.
*/


#ifndef Morse_h
#define Morse_h


//--------------------------------------------------------------------------\
//								    Imports					   				|
//--------------------------------------------------------------------------/


	#include "Arduino.h"
	#include "HAB_Actuator.h"
	#include <Wire.h>
	#include <SPI.h>


class HAB_Actuator {

	//--------------------------------------------------------------------------\
	//								  Definitions					   			|
	//--------------------------------------------------------------------------/
		private:
		
		#ifndef OPEN
			#define OPEN 10 //0 most open
		#endif
		#ifndef CLOSED
			#define CLOSED 1020 //1023 most closed, give some leeway here
		#endif
	
	
	//--------------------------------------------------------------------------\
	//								   Variables					   			|
	//--------------------------------------------------------------------------/
		
		
		//Actuator pins
		uint8_t act_en, act_push, act_pull, act_pos;
		uint8_t heat_en, thermistor;
		
		//Enable pins
		bool move_enabled, preheat_enabled;
		
		//Opening and closing altitudes
		double open_alt, close_alt;
		
		//If pod has opened already (used when descending)
		bool hasOpened;
		
		//Telnet override
		bool telnet_override, telnet_open;
		
		//Trend object
	
	
	//--------------------------------------------------------------------------\
	//								  Constructor					   			|
	//--------------------------------------------------------------------------/
		public:
	
		HAB_Actuator(uint8_t act_en, uint8_t act_push, uint8_t act_pull,
				uint8_t act_pos, uint8_t heat_en, uint8_t thermistor, double open_alt, double close_alt
		);
		
		
	//--------------------------------------------------------------------------\
	//								   Functions					   			|
	//--------------------------------------------------------------------------/
	
	
		//--------------------------------------------------------------------------------\
		//Getters-------------------------------------------------------------------------|
			double getOpenAlt();
			double getCloseAlt();			
			bool getMoveEnabled();
			bool getPreheatEnabled();
			bool isClosed();
			bool isFullyOpen();	
		
		
		//--------------------------------------------------------------------------------\
		//Setters-------------------------------------------------------------------------|
			void setMoveEnabled(boolean move_enabled);
			void setPreheatEnabled(boolean preheat_enabled);
			void setHeating(boolean heat_enabled);
		
		
		//--------------------------------------------------------------------------------\
		//Miscellaneous-------------------------------------------------------------------|
			void extend();
			void retract();
			void halt();
			void enableHeating();
			void disableHeating();
};

#endif