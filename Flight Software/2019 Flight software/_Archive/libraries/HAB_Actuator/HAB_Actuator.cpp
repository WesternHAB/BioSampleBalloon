/*
*	Author	:	Stephen Amey
*	Date	:	June 10, 2019
*	Purpose	: 	This library is used to interface with a linear actuator with position feedback. 
*				It is specifically tailored to the Western University HAB project.
*/

//--------------------------------------------------------------------------\
//								    Imports					   				|
//--------------------------------------------------------------------------/


	#include "HAB_Actuator.h"


//--------------------------------------------------------------------------\
//								  Constructor					   			|
//--------------------------------------------------------------------------/


	HAB_Actuator::HAB_Actuator(uint8_t act_en, uint8_t act_push, uint8_t act_pull,
		uint8_t act_pos, uint8_t heat_en, uint8_t thermistor, double open_alt, double close_alt
	){
		this->act_en = act_en;
		this->act_push = act_push;
		this->act_pull = act_pull;
		this->act_pos = act_pos;
		this->heat_en = heat_en;
		this->thermistor = thermistor;
		this->open_alt = open_alt;
		this->close_alt = close_alt;
		move_enabled = false;
		preheat_enabled = false;
		hasOpened = false;
		telnet_override = false;
		telnet_open = false;
	}
	
	
//--------------------------------------------------------------------------\
//								   Functions					   			|
//--------------------------------------------------------------------------/


	//--------------------------------------------------------------------------------\
	//Getters-------------------------------------------------------------------------|
		
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		getMoveEnabled															|
		|	Purpose: 	Gets the move_enabled boolean variable value.							|
		|	Arguments:	void																	|
		|	Returns:	boolean																	|
		\*-------------------------------------------------------------------------------------*/
			bool HAB_Actuator::getMoveEnabled(){
				return move_enabled;
			}
		
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		getPreheatEnabled														|
		|	Purpose: 	Gets the preheat_enabled boolean variable value.						|
		|	Arguments:	void																	|
		|	Returns:	boolean																	|
		\*-------------------------------------------------------------------------------------*/
			bool HAB_Actuator::getPreheatEnabled(){
				return preheat_enabled;
			}
	
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		isClosed																|
		|	Purpose: 	Returns true if the pod is closed.										|
		|	Arguments:	void																	|
		|	Returns:	boolean																	|
		\*-------------------------------------------------------------------------------------*/
			bool HAB_Actuator::isClosed(){
				//Read current position
				//Check if in open/closed lims
				//How close to 0 to be closed? Perhaps check trend if within lim and stuck
				
				uint16_t reading = analogRead(act_pos);
				return (reading >= CLOSED); //Check trend here?	
			}
	
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		isFullyOpen																|
		|	Purpose: 	Returns true if the pod is fully open.									|
		|	Arguments:	void																	|
		|	Returns:	boolean																	|
		\*-------------------------------------------------------------------------------------*/
			bool HAB_Actuator::isFullyOpen(){
				unsigned int reading = analogRead(act_pos); //Check trend at all?
				return (reading <= OPEN);
			}
			
	
	//--------------------------------------------------------------------------------\
	//Setters-------------------------------------------------------------------------|
	
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		setMoveEnabled															|
		|	Purpose: 	Sets the move_enabled boolean variable.									|
		|	Arguments:	boolean																	|
		|	Returns:	void																	|
		\*-------------------------------------------------------------------------------------*/
			void HAB_Actuator::setMoveEnabled(boolean move_enabled){
				this->move_enabled = move_enabled;
			}
		
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		setPreheatEnabled														|
		|	Purpose: 	Sets the preheat_enabled boolean variable.								|
		|	Arguments:	boolean																	|
		|	Returns:	void																	|
		\*-------------------------------------------------------------------------------------*/
			void HAB_Actuator::setPreheatEnabled(boolean preheat_enabled){
				this->preheat_enabled = preheat_enabled;
			}


	//--------------------------------------------------------------------------------\
	//Miscellaneous-------------------------------------------------------------------|	
		
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		extend																	|
		|	Purpose: 	Extends the actuator to its maximum length								|
		|	Arguments:	void																	|
		|	Returns:	boolean																	|
		\*-------------------------------------------------------------------------------------*/
			void HAB_Actuator::extend(){
				digitalWrite(act_push, HIGH);
				digitalWrite(act_pull, LOW);
			}
			
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		retract																	|
		|	Purpose: 	Retracts the actuator to its minimum length								|
		|	Arguments:	void																	|
		|	Returns:	void																	|
		\*-------------------------------------------------------------------------------------*/
			void HAB_Actuator::retract(){
				digitalWrite(act_push, LOW);
				digitalWrite(act_pull, HIGH);
				hasOpened = true;				
				//E.g., don't run retract if hasOpened == true
			}
		
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		halt																	|
		|	Purpose: 	Halts movement of the actuator											|
		|	Arguments:	void																	|
		|	Returns:	void																	|
		\*-------------------------------------------------------------------------------------*/
			void HAB_Actuator::halt(){
				digitalWrite(act_push, LOW);
				digitalWrite(act_pull, LOW);
			}
			
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		enableHeating															|
		|	Purpose: 	Starts the heating of the actuator										|
		|	Arguments:	void																	|
		|	Returns:	void																	|
		\*-------------------------------------------------------------------------------------*/
			void HAB_Actuator::enableHeating(){
				digitalWrite(heat_en, HIGH);
			}
			
		/*-------------------------------------------------------------------------------------*\
		| 	Name: 		disableHeating															|
		|	Purpose: 	Stops the heating of the actuator										|
		|	Arguments:	void																	|
		|	Returns:	void																	|
		\*-------------------------------------------------------------------------------------*/
			void HAB_Actuator::disableHeating(){
				digitalWrite(heat_en, LOW);
			}